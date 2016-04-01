/* GStreamer Intel MSDK plugin
 * Copyright (C) 2016 Oblong Industries
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __MSDK_H__
#define __MSDK_H__

#include <string.h>
#include <unistd.h>

#include <gst/gst.h>
#include <gst/video/video.h>
#include "mfxvideo.h"

G_BEGIN_DECLS

typedef struct _MsdkContext MsdkContext;

gboolean msdk_is_available ();

MsdkContext *msdk_open_context (gboolean hardware);
void msdk_close_context (MsdkContext * context);
mfxSession msdk_context_get_session (MsdkContext * context);

mfxFrameSurface1 *msdk_get_free_surface (mfxFrameSurface1 * surfaces,
    guint size);
void msdk_frame_to_surface (GstVideoFrame * frame, mfxFrameSurface1 * surface);

const gchar *msdk_status_to_string (mfxStatus status);

G_END_DECLS

#endif /* __MSDK_H__ */
