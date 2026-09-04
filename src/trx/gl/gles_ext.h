#pragma once

// Resolves the GLES extension function pointers gl_compat.h maps GL calls
// onto. Call once, right after the GL context is current (mirrors what
// glewInit() does for the desktop build).
bool TRX_GLES_LoadExtensions(void);
