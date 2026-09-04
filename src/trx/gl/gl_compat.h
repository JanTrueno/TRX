#pragma once

// Single switch point between desktop OpenGL (GLEW) and OpenGL ES 3.2.
// Everywhere else in the codebase includes this header instead of <GL/glew.h>
// directly, so the desktop/GLES split lives in one place rather than being
// sprinkled across every file that touches GL.

#ifndef TRX_GLES
    #define TRX_GLES 0
#endif

#if TRX_GLES

    #include <GLES3/gl32.h>
    #include <GLES2/gl2ext.h>

    #ifndef GL_BGRA
        // Same numeric value as desktop's GL_BGRA; upload relies on
        // GL_EXT_texture_format_BGRA8888, which is present on effectively
        // every real GLES implementation.
        #define GL_BGRA GL_BGRA_EXT
    #endif

    // GLES has no packed-int pixel types at all. On every little-endian
    // target this codebase ships for, pairing a format with
    // UNSIGNED_INT_8_8_8_8_REV puts the format's first-named component in
    // the lowest memory byte - the exact same in-memory byte order
    // UNSIGNED_BYTE already produces for that format. So this is a like-for-
    // like substitution, not just a stand-in to satisfy the compiler.
    #define GL_UNSIGNED_INT_8_8_8_8_REV GL_UNSIGNED_BYTE

    // GLES has no matrix/attribute stack, so glGetError() can never report
    // these desktop-only conditions.
    #ifndef GL_STACK_UNDERFLOW
        #define GL_STACK_UNDERFLOW 0x0504
    #endif
    #ifndef GL_STACK_OVERFLOW
        #define GL_STACK_OVERFLOW 0x0503
    #endif

    // glClearDepth(GLdouble) doesn't exist in GLES; glClearDepthf(GLfloat)
    // takes the same argument every call site already passes.
    #define glClearDepth glClearDepthf

    // glDrawBuffer(GLenum) doesn't exist in GLES; glDrawBuffers(count, list)
    // is the only way to select the draw target, even for a single one.
    static inline void TRX_GLCompat_DrawBuffer(GLenum buf)
    {
        glDrawBuffers(1, &buf);
    }
    #define glDrawBuffer TRX_GLCompat_DrawBuffer

    // glBindFragDataLocation doesn't exist in GLES: every fragment shader in
    // this codebase declares exactly one "out" variable, which GLSL ES
    // assigns to location 0 on its own, so the runtime binding call is a
    // no-op rather than something GLES can be asked to do.
    static inline void TRX_GLCompat_BindFragDataLocation(
        GLuint program, GLuint color_number, const GLchar *name)
    {
        (void)program;
        (void)color_number;
        (void)name;
    }
    #define glBindFragDataLocation TRX_GLCompat_BindFragDataLocation

    // glPolygonMode doesn't exist in GLES: the rasterizer has no line-fill
    // mode at all, so wireframe rendering simply isn't available under GLES.
    // GL_POLYGON_MODE/GL_FILL/GL_LINE have no other use in GLES and aren't
    // declared by its headers either; callers query/restore this
    // (nonexistent) state around the no-op below, so the values only need to
    // exist, not mean anything.
    #ifndef GL_POLYGON_MODE
        #define GL_POLYGON_MODE 0x0B40
    #endif
    #ifndef GL_FILL
        #define GL_FILL 0x1B02
    #endif
    #ifndef GL_LINE
        #define GL_LINE 0x1B01
    #endif
    static inline void TRX_GLCompat_PolygonMode(GLenum face, GLenum mode)
    {
        (void)face;
        (void)mode;
    }
    #define glPolygonMode TRX_GLCompat_PolygonMode

    // Several call sites save/restore GL_POLYGON_MODE around the no-op
    // above; querying it for real would just raise GL_INVALID_ENUM on every
    // one of those (frequent, per-draw) calls. Swallow that one pname here so
    // every other glGetIntegerv() call site is unaffected.
    static inline void TRX_GLCompat_GetIntegerv(GLenum pname, GLint *params)
    {
        if (pname == GL_POLYGON_MODE) {
            return;
        }
        glGetIntegerv(pname, params);
    }
    #define glGetIntegerv TRX_GLCompat_GetIntegerv

    // glTexImage2DMultisample doesn't exist in GLES; glTexStorage2DMultisample
    // takes the identical argument list and allocates immutable storage
    // instead, which is fine here since these textures are always freshly
    // (re)created rather than resized in place.
    #define glTexImage2DMultisample glTexStorage2DMultisample

    // GL_MULTISAMPLE doesn't exist in GLES: multisampling isn't a togglable
    // cap there, it's implicit in whether the bound framebuffer has more than
    // one sample. glEnable(GL_MULTISAMPLE) is a no-op on the one call site
    // that uses it either way, so swallow just that enum.
    #ifndef GL_MULTISAMPLE
        #define GL_MULTISAMPLE 0x809D
    #endif
    static inline void TRX_GLCompat_Enable(GLenum cap)
    {
        if (cap == GL_MULTISAMPLE) {
            return;
        }
        glEnable(cap);
    }
    #define glEnable TRX_GLCompat_Enable

    // glDrawElementsBaseVertex has no core GLES entry point. It's resolved at
    // runtime from the EXT/OES draw_elements_base_vertex extension by
    // TRX_GLES_LoadExtensions(); see gles_ext.h.
    typedef void(GL_APIENTRY *TRX_PFNGLDRAWELEMENTSBASEVERTEXPROC)(
        GLenum mode, GLsizei count, GLenum type, const void *indices,
        GLint basevertex);
    extern TRX_PFNGLDRAWELEMENTSBASEVERTEXPROC trx_glDrawElementsBaseVertex;
    #define glDrawElementsBaseVertex trx_glDrawElementsBaseVertex

    #ifndef GLAPIENTRY
        #define GLAPIENTRY GL_APIENTRY
    #endif

#else

    #include <GL/glew.h>

#endif
