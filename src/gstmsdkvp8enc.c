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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstmsdkvp8enc.h"
#include "mfxvp8.h"

#include <gst/pbutils/pbutils.h>

GST_DEBUG_CATEGORY_EXTERN (gst_msdkvp8enc_debug);
#define GST_CAT_DEFAULT gst_msdkvp8enc_debug

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-vp8, "
        "framerate = (fraction) [0/1, MAX], "
        "width = (int) [ 1, MAX ], height = (int) [ 1, MAX ], "
        "profile = (string) { 0, 1, 2, 3 }")
    );

#define gst_msdkvp8enc_parent_class parent_class
G_DEFINE_TYPE (GstMsdkVP8Enc, gst_msdkvp8enc, GST_TYPE_MSDKENC);

static gboolean
gst_msdkvp8enc_set_format (GstMsdkEnc * encoder)
{
  GstMsdkVP8Enc *thiz = GST_MSDKVP8ENC (encoder);
  GstCaps *template_caps;
  GstCaps *allowed_caps = NULL;

  thiz->profile = 0;

  template_caps = gst_static_pad_template_get_caps (&src_factory);
  allowed_caps = gst_pad_get_allowed_caps (GST_VIDEO_ENCODER_SRC_PAD (encoder));

  /* If downstream has ANY caps let encoder decide profile and level */
  if (allowed_caps == template_caps) {
    GST_INFO_OBJECT (thiz,
        "downstream has ANY caps, profile/level set to auto");
  } else if (allowed_caps) {
    GstStructure *s;
    const gchar *profile;

    if (gst_caps_is_empty (allowed_caps)) {
      gst_caps_unref (allowed_caps);
      gst_caps_unref (template_caps);
      return FALSE;
    }

    allowed_caps = gst_caps_make_writable (allowed_caps);
    allowed_caps = gst_caps_fixate (allowed_caps);
    s = gst_caps_get_structure (allowed_caps, 0);

    profile = gst_structure_get_string (s, "profile");
    if (profile) {
      if (!strcmp (profile, "3")) {
        thiz->profile = MFX_PROFILE_VP8_3;
      } else if (!strcmp (profile, "2")) {
        thiz->profile = MFX_PROFILE_VP8_2;
      } else if (!strcmp (profile, "1")) {
        thiz->profile = MFX_PROFILE_VP8_1;
      } else if (!strcmp (profile, "0")) {
        thiz->profile = MFX_PROFILE_VP8_0;
      } else {
        g_assert_not_reached ();
      }
    }

    gst_caps_unref (allowed_caps);
  }

  gst_caps_unref (template_caps);

  return TRUE;
}

static gboolean
gst_msdkvp8enc_configure (GstMsdkEnc * encoder)
{
  GstMsdkVP8Enc *thiz = GST_MSDKVP8ENC (encoder);

  encoder->param.mfx.CodecId = MFX_CODEC_VP8;
  encoder->param.mfx.CodecProfile = thiz->profile;
  encoder->param.mfx.CodecLevel = 0;

  return TRUE;
}

static inline const gchar *
profile_to_string (gint profile)
{
  switch (profile) {
    case MFX_PROFILE_VP8_3:
      return "3";
    case MFX_PROFILE_VP8_2:
      return "2";
    case MFX_PROFILE_VP8_1:
      return "1";
    case MFX_PROFILE_VP8_0:
      return "0";
    default:
      break;
  }

  return NULL;
}

static GstCaps *
gst_msdkvp8enc_set_src_caps (GstMsdkEnc * encoder)
{
  GstCaps *caps;
  GstStructure *structure;
  const gchar *profile;

  caps = gst_caps_new_empty_simple ("video/x-vp8");
  structure = gst_caps_get_structure (caps, 0);

  profile = profile_to_string (encoder->param.mfx.CodecProfile);
  if (profile)
    gst_structure_set (structure, "profile", G_TYPE_STRING, profile, NULL);

  return caps;
}

static void
gst_msdkvp8enc_class_init (GstMsdkVP8EncClass * klass)
{
  GstElementClass *element_class;
  GstMsdkEncClass *encoder_class;

  element_class = GST_ELEMENT_CLASS (klass);
  encoder_class = GST_MSDKENC_CLASS (klass);

  encoder_class->set_format = gst_msdkvp8enc_set_format;
  encoder_class->configure = gst_msdkvp8enc_configure;
  encoder_class->set_src_caps = gst_msdkvp8enc_set_src_caps;

  gst_element_class_set_static_metadata (element_class,
      "Intel MSDK VP8 encoder",
      "Codec/Encoder/Video",
      "VP8 video encoder based on Intel Media SDK",
      "Josep Torra <jtorra@oblong.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
}

static void
gst_msdkvp8enc_init (GstMsdkVP8Enc * thiz)
{
}
