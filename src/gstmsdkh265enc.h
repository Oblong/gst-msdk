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

#ifndef __GST_MSDKH265ENC_H__
#define __GST_MSDKH265ENC_H__

#include "gstmsdkenc.h"

G_BEGIN_DECLS

#define GST_TYPE_MSDKH265ENC \
  (gst_msdkh265enc_get_type())
#define GST_MSDKH265ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MSDKH265ENC,GstMsdkH265Enc))
#define GST_MSDKH265ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MSDKH265ENC,GstMsdkH265EncClass))
#define GST_IS_MSDKH265ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MSDKH265ENC))
#define GST_IS_MSDKH265ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MSDKH265ENC))

typedef struct _GstMsdkH265Enc      GstMsdkH265Enc;
typedef struct _GstMsdkH265EncClass GstMsdkH265EncClass;

struct _GstMsdkH265Enc
{
  GstMsdkEnc base;
};

struct _GstMsdkH265EncClass
{
  GstMsdkEncClass parent_class;
};

GType gst_msdkh265enc_get_type (void);

G_END_DECLS

#endif /* __GST_MSDKH265ENC_H__ */
