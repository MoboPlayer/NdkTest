# The MIT License (MIT)
# 
# Copyright (c) 2014 MoboPlayer.com
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

LOCAL_PATH := $(call my-dir)
 include $(CLEAR_VARS)
 LOCAL_MODULE := myffmpegv7
 LOCAL_SRC_FILES := ffmpeg/libffmpeg_armv7_neon.so
 LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include  
 include $(PREBUILT_SHARED_LIBRARY)
 
include $(CLEAR_VARS)
LOCAL_MODULE    := mobo_jni_util
LOCAL_C_INCLUDES:= F:\MoboPlayer\codec\ffmpeg\FFmpeg-master
LOCAL_SRC_FILES := mobo_subtitle_jni.c cmp_ffmpeg.c mobo_open_subtitle.c mobo_thumbnail_jni.c mobo_thumbnail.c mobo_load_ffmpeg.c mobo_download_jni.c \
                   mobo_download_remuxing.c #ffserver.c cmdutil.c ffserver_config.c
LOCAL_LDLIBS    := -llog
LOCAL_CFLAGS := -DNEED_ANDROID_LOG=1
include $(BUILD_SHARED_LIBRARY)

 #include $(CLEAR_VARS)
# LOCAL_MODULE := ffserver
# LOCAL_SRC_FILES := server/ffserver
# include $(BUILD_EXECUTABLE) 
