#include <trx/gl/gles_ext.h>

#include <trx/core/log.h>
#include <trx/gl/gl_compat.h>

#include <SDL2/SDL_video.h>

TRX_PFNGLDRAWELEMENTSBASEVERTEXPROC trx_glDrawElementsBaseVertex = nullptr;

bool TRX_GLES_LoadExtensions(void)
{
    trx_glDrawElementsBaseVertex =
        (TRX_PFNGLDRAWELEMENTSBASEVERTEXPROC)SDL_GL_GetProcAddress(
            "glDrawElementsBaseVertexEXT");
    if (trx_glDrawElementsBaseVertex == nullptr) {
        trx_glDrawElementsBaseVertex =
            (TRX_PFNGLDRAWELEMENTSBASEVERTEXPROC)SDL_GL_GetProcAddress(
                "glDrawElementsBaseVertexOES");
    }
    if (trx_glDrawElementsBaseVertex == nullptr) {
        LOG_ERROR(
            "This GPU/driver lacks GL_EXT_draw_elements_base_vertex and "
            "GL_OES_draw_elements_base_vertex, both required by TRX's mesh "
            "batcher under GLES");
        return false;
    }
    return true;
}
