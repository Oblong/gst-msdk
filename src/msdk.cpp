/* GStreamer Intel MSDK plugin
 * Copyright (c) 2016, Oblong Industries, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/* TODO:
 *   - discover dri_path instead of having it hardcoded
 */

#include <dlfcn.h>
#include <fcntl.h>

#include "msdk.h"

GST_DEBUG_CATEGORY_EXTERN (gst_msdkenc_debug);
#define GST_CAT_DEFAULT gst_msdkenc_debug

#define INVALID_INDEX         ((guint) -1)
#define VA_STATUS_SUCCESS     0x00000000

typedef int VAStatus;
typedef void *VADisplay;

typedef VAStatus (*vaInitialize_type) (VADisplay, int *, int *);
typedef VAStatus (*vaTerminate_type) (VADisplay);
typedef VADisplay (*vaGetDisplayDRM_type) (int);

struct _MsdkContext
{
  mfxSession session;
  gint fd;
  VADisplay dpy;
};

static void *libva_so_handle = NULL;
static void *libva_drm_so_handle = NULL;
static vaInitialize_type vaInitialize_proxy = NULL;
static vaTerminate_type vaTerminate_proxy = NULL;
static vaGetDisplayDRM_type vaGetDisplayDRM_proxy = NULL;

static gboolean
msdk_libva_proxy_init ()
{
  if (libva_so_handle && libva_drm_so_handle && vaInitialize_proxy
      && vaTerminate_proxy && vaGetDisplayDRM_proxy)
    return TRUE;

  libva_so_handle = dlopen ("libva.so.1", RTLD_GLOBAL | RTLD_NOW);
  if (!libva_so_handle)
    return FALSE;

  libva_drm_so_handle = dlopen ("libva-drm.so.1", RTLD_GLOBAL | RTLD_NOW);
  if (!libva_drm_so_handle)
    return FALSE;

  vaInitialize_proxy =
      (vaInitialize_type) dlsym (libva_so_handle, "vaInitialize");
  vaTerminate_proxy = (vaTerminate_type) dlsym (libva_so_handle, "vaTerminate");
  vaGetDisplayDRM_proxy =
      (vaGetDisplayDRM_type) dlsym (libva_drm_so_handle, "vaGetDisplayDRM");

  return (vaInitialize_proxy && vaTerminate_proxy && vaGetDisplayDRM_proxy);
}

#if 0
void
msdk_libva_proxy_deinit ()
{
  if (libva_drm_so_handle)
    dlclose (libva_drm_so_handle);

  if (libva_so_handle)
    dlclose (libva_so_handle);

  libva_so_handle = NULL;
  libva_drm_so_handle = NULL;
  vaInitialize_proxy = NULL;
  vaTerminate_proxy = NULL;
  vaGetDisplayDRM_proxy = NULL;
}
#endif

static inline void
msdk_close_session (mfxSession session)
{
  mfxStatus status;

  if (!session)
    return;

  status = MFXClose (session);
  if (status != MFX_ERR_NONE)
    GST_ERROR ("Close failed (%s)", msdk_status_to_string (status));
}

static inline mfxSession
msdk_open_session (gboolean hardware)
{
  mfxSession session = NULL;
  mfxVersion version = { {1, 1} };
  mfxIMPL implementation;
  mfxStatus status;

  static const gchar *implementation_names[] = {
    "AUTO", "SOFTWARE", "HARDWARE", "AUTO_ANY", "HARDWARE_ANY", "HARDWARE2",
    "HARDWARE3", "HARDWARE4", "RUNTIME"
  };

  status = MFXInit (hardware ? MFX_IMPL_HARDWARE_ANY : MFX_IMPL_SOFTWARE,
      &version, &session);
  if (status != MFX_ERR_NONE) {
    GST_ERROR ("Intel Media SDK not available (%s)",
        msdk_status_to_string (status));
    goto failed;
  }

  MFXQueryIMPL (session, &implementation);
  if (status != MFX_ERR_NONE) {
    GST_ERROR ("Query implementation failed (%s)",
        msdk_status_to_string (status));
    goto failed;
  }

  MFXQueryVersion (session, &version);
  if (status != MFX_ERR_NONE) {
    GST_ERROR ("Query version failed (%s)", msdk_status_to_string (status));
    goto failed;
  }

  GST_INFO ("MSDK implementation: 0x%04x (%s)", implementation,
      implementation_names[MFX_IMPL_BASETYPE (implementation)]);
  GST_INFO ("MSDK version: %d.%d", version.Major, version.Minor);

  return session;

failed:
  msdk_close_session (session);
  return NULL;
}

gboolean
msdk_is_available ()
{
  mfxSession session = msdk_open_session (FALSE);
  if (!session) {
    return FALSE;
  }

  msdk_close_session (session);
  return TRUE;
}

static gboolean
msdk_use_vaapi_on_context (MsdkContext * context)
{
  gint fd;
  gint maj_ver, min_ver;
  VADisplay va_dpy = NULL;;
  VAStatus va_status;
  mfxStatus status;
  /* maybe /dev/dri/renderD128 */
  static const gchar *dri_path = "/dev/dri/card0";

  if (!msdk_libva_proxy_init ()) {
    GST_ERROR ("Couldn't open libva or libva-drm libraries");
    return FALSE;
  }

  fd = open (dri_path, O_RDWR);
  if (fd < 0) {
    GST_ERROR ("Couldn't open %s", dri_path);
    return FALSE;
  }

  va_dpy = vaGetDisplayDRM_proxy (fd);
  if (!va_dpy) {
    GST_ERROR ("Couldn't get a VA DRM display");
    goto failed;
  }

  va_status = vaInitialize_proxy (va_dpy, &maj_ver, &min_ver);
  if (va_status != VA_STATUS_SUCCESS) {
    GST_ERROR ("Couldn't initialize VA DRM display");
    goto failed;
  }

  status = MFXVideoCORE_SetHandle (context->session, MFX_HANDLE_VA_DISPLAY,
      (mfxHDL) va_dpy);
  if (status != MFX_ERR_NONE) {
    GST_ERROR ("Setting VAAPI handle failed (%s)",
        msdk_status_to_string (status));
    goto failed;
  }

  context->fd = fd;
  context->dpy = va_dpy;

  return TRUE;

failed:
  if (va_dpy)
    vaTerminate_proxy (va_dpy);
  close (fd);
  return FALSE;
}

MsdkContext *
msdk_open_context (gboolean hardware)
{
  MsdkContext *context = g_slice_new0 (MsdkContext);
  context->fd = -1;

  context->session = msdk_open_session (hardware);
  if (!context->session)
    goto failed;

  if (hardware) {
    if (!msdk_use_vaapi_on_context (context))
      goto failed;
  }

  return context;

failed:
  msdk_close_session (context->session);
  g_slice_free (MsdkContext, context);
  return NULL;
}

void
msdk_close_context (MsdkContext * context)
{
  if (!context)
    return;

  msdk_close_session (context->session);
  if (context->dpy)
    vaTerminate_proxy (context->dpy);
  if (context->fd >= 0)
    close (context->fd);
  g_slice_free (MsdkContext, context);
}

mfxSession
msdk_context_get_session (MsdkContext * context)
{
  return context->session;
}

static inline guint
msdk_get_free_surface_index (mfxFrameSurface1 * surfaces, guint size)
{
  if (surfaces) {
    for (guint i = 0; i < size; i++) {
      if (!surfaces[i].Data.Locked)
        return i;
    }
  }

  return INVALID_INDEX;
}

mfxFrameSurface1 *
msdk_get_free_surface (mfxFrameSurface1 * surfaces, guint size)
{
  guint idx = INVALID_INDEX;

  /* Poll the pool for a maximum of 20 milisecnds */
  for (guint i = 0; i < 2000; i++) {
    idx = msdk_get_free_surface_index (surfaces, size);

    if (idx != INVALID_INDEX)
      break;

    g_usleep (10);
  }

  return (idx == INVALID_INDEX ? NULL : &surfaces[idx]);
}

/* FIXME: Only NV12 is supported by now, add other YUV formats */
void
msdk_frame_to_surface (GstVideoFrame * frame, mfxFrameSurface1 * surface)
{
  guint8 *src, *dst;
  guint sstride, dstride;
  guint width, height;

  if (!surface->Data.MemId) {
    surface->Data.Y = GST_VIDEO_FRAME_COMP_DATA (frame, 0);
    surface->Data.UV = GST_VIDEO_FRAME_COMP_DATA (frame, 1);
    surface->Data.Pitch = GST_VIDEO_FRAME_COMP_STRIDE (frame, 0);
    return;
  }

  /* Y Plane */
  width = GST_VIDEO_FRAME_COMP_WIDTH (frame, 0);
  height = GST_VIDEO_FRAME_COMP_HEIGHT (frame, 0);
  src = GST_VIDEO_FRAME_COMP_DATA (frame, 0);
  sstride = GST_VIDEO_FRAME_COMP_STRIDE (frame, 0);
  dst = surface->Data.Y;
  dstride = surface->Data.Pitch;

  for (guint i = 0; i < height; i++) {
    memcpy (dst, src, width);
    src += sstride;
    dst += dstride;
  }

  /* UV Plane */
  height = GST_VIDEO_FRAME_COMP_HEIGHT (frame, 1);
  src = GST_VIDEO_FRAME_COMP_DATA (frame, 1);
  sstride = GST_VIDEO_FRAME_COMP_STRIDE (frame, 1);
  dst = surface->Data.UV;

  for (guint i = 0; i < height; i++) {
    memcpy (dst, src, width);
    src += sstride;
    dst += dstride;
  }
}

const gchar *
msdk_status_to_string (mfxStatus status)
{
  switch (status) {
      /* no error */
    case MFX_ERR_NONE:
      return "no error";
      /* reserved for unexpected errors */
    case MFX_ERR_UNKNOWN:
      return "unknown error";
      /* error codes <0 */
    case MFX_ERR_NULL_PTR:
      return "null pointer";
    case MFX_ERR_UNSUPPORTED:
      return "undeveloped feature";
    case MFX_ERR_MEMORY_ALLOC:
      return "failed to allocate memory";
    case MFX_ERR_NOT_ENOUGH_BUFFER:
      return "insufficient buffer at input/output";
    case MFX_ERR_INVALID_HANDLE:
      return "invalid handle";
    case MFX_ERR_LOCK_MEMORY:
      return "failed to lock the memory block";
    case MFX_ERR_NOT_INITIALIZED:
      return "member function called before initialization";
    case MFX_ERR_NOT_FOUND:
      return "the specified object is not found";
    case MFX_ERR_MORE_DATA:
      return "expect more data at input";
    case MFX_ERR_MORE_SURFACE:
      return "expect more surface at output";
    case MFX_ERR_ABORTED:
      return "operation aborted";
    case MFX_ERR_DEVICE_LOST:
      return "lose the HW acceleration device";
    case MFX_ERR_INCOMPATIBLE_VIDEO_PARAM:
      return "incompatible video parameters";
    case MFX_ERR_INVALID_VIDEO_PARAM:
      return "invalid video parameters";
    case MFX_ERR_UNDEFINED_BEHAVIOR:
      return "undefined behavior";
    case MFX_ERR_DEVICE_FAILED:
      return "device operation failure";
    case MFX_ERR_MORE_BITSTREAM:
      return "expect more bitstream buffers at output";
    case MFX_ERR_INCOMPATIBLE_AUDIO_PARAM:
      return "incompatible audio parameters";
    case MFX_ERR_INVALID_AUDIO_PARAM:
      return "invalid audio parameters";
      /* warnings >0 */
    case MFX_WRN_IN_EXECUTION:
      return "the previous asynchronous operation is in execution";
    case MFX_WRN_DEVICE_BUSY:
      return "the HW acceleration device is busy";
    case MFX_WRN_VIDEO_PARAM_CHANGED:
      return "the video parameters are changed during decoding";
    case MFX_WRN_PARTIAL_ACCELERATION:
      return "SW is used";
    case MFX_WRN_INCOMPATIBLE_VIDEO_PARAM:
      return "incompatible video parameters";
    case MFX_WRN_VALUE_NOT_CHANGED:
      return "the value is saturated based on its valid range";
    case MFX_WRN_OUT_OF_RANGE:
      return "the value is out of valid range";
    case MFX_WRN_FILTER_SKIPPED:
      return "one of requested filters has been skipped";
    case MFX_WRN_INCOMPATIBLE_AUDIO_PARAM:
      return "incompatible audio parameters";
    default:
      break;
  }
  return "undefiend error";
}
