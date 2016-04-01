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

#include <gst/gst.h>

#include "gstmsdkh264enc.h"
#include "gstmsdkh265enc.h"
#include "gstmsdkmpeg2enc.h"
#include "gstmsdkvp8enc.h"

GST_DEBUG_CATEGORY (gst_msdkenc_debug);
GST_DEBUG_CATEGORY (gst_msdkh264enc_debug);
GST_DEBUG_CATEGORY (gst_msdkh265enc_debug);
GST_DEBUG_CATEGORY (gst_msdkmpeg2enc_debug);
GST_DEBUG_CATEGORY (gst_msdkvp8enc_debug);

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret;

  GST_DEBUG_CATEGORY_INIT (gst_msdkenc_debug, "msdkenc", 0, "msdkenc");
  GST_DEBUG_CATEGORY_INIT (gst_msdkh264enc_debug, "msdkh264enc", 0,
      "msdkh264enc");
  GST_DEBUG_CATEGORY_INIT (gst_msdkh264enc_debug, "msdkh265enc", 0,
      "msdkh265enc");
  GST_DEBUG_CATEGORY_INIT (gst_msdkh264enc_debug, "msdkmpeg2enc", 0,
      "msdkmpeg2enc");
  GST_DEBUG_CATEGORY_INIT (gst_msdkvp8enc_debug, "msdkvp8enc", 0,
      "msdkvp8enc");


  if (!msdk_is_available ())
    return FALSE;

  ret = gst_element_register (plugin, "msdkh264enc", GST_RANK_NONE,
      GST_TYPE_MSDKH264ENC);

  ret = gst_element_register (plugin, "msdkh265enc", GST_RANK_NONE,
      GST_TYPE_MSDKH265ENC);

  ret = gst_element_register (plugin, "msdkmpeg2enc", GST_RANK_NONE,
      GST_TYPE_MSDKMPEG2ENC);

  ret = gst_element_register (plugin, "msdkvp8enc", GST_RANK_NONE,
      GST_TYPE_MSDKVP8ENC);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    msdk,
    "Intel Media SDK encoders",
    plugin_init, VERSION, "LGPL", "Oblong", "http://oblong.com/")
