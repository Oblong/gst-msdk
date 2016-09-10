
# gst-msdk

gst-msdk is a plugin for
[Intel Media SDK](https://software.intel.com/en-us/media-sdk), a
cross-platform API for developing media applications. The plugin has
multiple elements for video hardware encoding leveraging latest Intel
processors through Intel Media SDK.

- MPEG2 encoding (*msdkmpeg2enc*)

- H.264 encoding (*msdkh264enc*)

- H.265 encoding (*msdkh265enc*)

- VP8 encoding (*msdkvp8enc*)


It requires:

- Intel Media SDK

- GStreamer >= 1.6.0


# Installation

Download the latest release and untar it. Then, run the typical
sequence:

    $ ./configure --prefix=<gstreamer-prefix> --with-msdk-prefix=<msdk-prefix>
    $ make
    $ sudo make install

Where *gstreamer-prefix* should preferably be the same as your system
GStreamer installation directory. And *msdk-prefix* is the location of
your Intel Media SDK installation.

Once the plugin has been installed you can verify if the elements exist:

    $ gst-inspect-1.0 msdk

    Plugin Details:
      Name                     msdk
      Description              Intel Media SDK encoders
      Filename                 /home/msdk/gst/master/plugins/libgstmsdk.so
      Version                  1.0.2
      License                  BSD
      Source module            gst-msdk
      Binary package           Oblong
      Origin URL               http://oblong.com/

      msdkh264enc: Intel MSDK H264 encoder
      msdkh265enc: Intel MSDK H265 encoder
      msdkmpeg2enc: Intel MSDK MPEG2 encoder
      msdkvp8enc: Intel MSDK VP8 encoder

      4 features:
      +-- 4 elements


# Giving it a try

Encoding a simple video test source and saving it to a file.

    $ gst-launch-1.0 videotestsrc ! msdkh264enc ! filesink location=test.h264


# License

gst-mdk is freely available for download under the terms of the
[BSD3](https://github.com/Oblong/gst-msdk/blob/master/COPYING
"COPING") License.
