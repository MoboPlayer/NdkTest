LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := myffmpegv7
LOCAL_SRC_FILES := ffmpeg/libffmpeg_armv7_neon.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include  
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := subtitle-jni
LOCAL_SRC_FILES := subtitle-jni.c cmp_ffmpeg.c mobo_open_subtitle.c
LOCAL_LDLIBS    := -llog
LOCAL_CFLAGS := -DNEED_ANDROID_LOG=1
include $(BUILD_SHARED_LIBRARY)