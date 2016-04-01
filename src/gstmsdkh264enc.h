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

#ifndef __GST_MSDKH264ENC_H__
#define __GST_MSDKH264ENC_H__

#include "gstmsdkenc.h"

G_BEGIN_DECLS

#define GST_TYPE_MSDKH264ENC \
  (gst_msdkh264enc_get_type())
#define GST_MSDKH264ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MSDKH264ENC,GstMsdkH264Enc))
#define GST_MSDKH264ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MSDKH264ENC,GstMsdkH264EncClass))
#define GST_IS_MSDKH264ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MSDKH264ENC))
#define GST_IS_MSDKH264ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MSDKH264ENC))

typedef struct _GstMsdkH264Enc      GstMsdkH264Enc;
typedef struct _GstMsdkH264EncClass GstMsdkH264EncClass;

struct _GstMsdkH264Enc
{
  GstMsdkEnc base;

  mfxExtCodingOption option;

  gint profile;
  gint level;

  gboolean cabac;
  gboolean lowpower;
};

struct _GstMsdkH264EncClass
{
  GstMsdkEncClass parent_class;
};

GType gst_msdkh264enc_get_type (void);

G_END_DECLS

#endif /* __GST_MSDKH264ENC_H__ */
