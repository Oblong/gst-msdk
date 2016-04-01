plugin_LTLIBRARIES = libgstmsdk.la

libgstmsdk_la_SOURCES = \
  msdk.cpp \
  gstmsdkh264enc.c \
  gstmsdkh265enc.c \
  gstmsdkmpeg2enc.c \
  gstmsdkvp8enc.c \
  gstmsdkenc.c \
  gstmsdk.c

libgstmsdk_la_CFLAGS = \
  $(MSDK_CFLAGS) \
  $(GST_CFLAGS)
libgstmsdk_la_CXXFLAGS = \
  $(MSDK_CFLAGS) \
  $(GST_CFLAGS)
libgstmsdk_la_LIBADD = \
  -ldl \
  $(GST_LIBS)
libgstmsdk_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) $(MSDK_LIBS)
libgstmsdk_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = \
  msdk.h \
  gstmsdkh264enc.h \
  gstmsdkh265enc.h \
  gstmsdkmpeg2enc.h \
  gstmsdkvp8enc.h \
  gstmsdkenc.h

