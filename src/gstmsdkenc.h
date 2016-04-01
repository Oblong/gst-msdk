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

#ifndef __GST_MSDKENC_H__
#define __GST_MSDKENC_H__

#include <gst/gst.h>
#include <gst/video/gstvideoencoder.h>
#include "msdk.h"

G_BEGIN_DECLS

#define GST_TYPE_MSDKENC \
  (gst_msdkenc_get_type())
#define GST_MSDKENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MSDKENC,GstMsdkEnc))
#define GST_MSDKENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MSDKENC,GstMsdkEncClass))
#define GST_MSDKENC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_MSDKENC,GstMsdkEncClass))
#define GST_IS_MSDKENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MSDKENC))
#define GST_IS_MSDKENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MSDKENC))

#define MAX_EXTRA_PARAMS 8

typedef struct _GstMsdkEnc      GstMsdkEnc;
typedef struct _GstMsdkEncClass GstMsdkEncClass;
typedef struct _MsdkEncTask     MsdkEncTask;

struct _GstMsdkEnc
{
  GstVideoEncoder element;

  /* input description */
  GstVideoCodecState *input_state;

  /* List of frame/buffer mapping structs for
   * pending frames */
  GList *pending_frames;

  /* MFX context */
  MsdkContext *context;
  mfxVideoParam param;
  guint num_surfaces;
  mfxFrameSurface1 *surfaces;
  guint num_tasks;
  MsdkEncTask *tasks;
  guint next_task;

  mfxExtBuffer *extra_params[MAX_EXTRA_PARAMS];
  guint num_extra_params;

  /* element properties */
  gboolean hardware;

  guint async_depth;
  guint target_usage;
  guint rate_control;
  guint bitrate;
  guint qpi;
  guint qpp;
  guint qpb;
  guint gop_size;
  guint ref_frames;
  guint i_frames;
  guint b_frames;

  gboolean reconfig;
};

struct _GstMsdkEncClass
{
  GstVideoEncoderClass parent_class;

  gboolean       (*set_format)           (GstMsdkEnc *encoder);
  gboolean       (*configure)            (GstMsdkEnc *encoder);
  GstCaps *      (*set_src_caps)         (GstMsdkEnc *encoder);
};

struct _MsdkEncTask
{
  GstVideoCodecFrame *input_frame;
  mfxSyncPoint sync_point;
  mfxBitstream output_bitstream;
};

GType gst_msdkenc_get_type (void);

void gst_msdkenc_add_extra_param (GstMsdkEnc * thiz, mfxExtBuffer * param);

#if !GST_CHECK_VERSION (1,8,0)
static inline void
gst_element_class_add_static_pad_template (GstElementClass * klass,
    GstStaticPadTemplate * static_templ)
{
  gst_element_class_add_pad_template (klass,
      gst_static_pad_template_get (static_templ));
}
#endif

G_END_DECLS

#endif /* __GST_MSDKENC_H__ */
