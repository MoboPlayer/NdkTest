/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include "mobo_open_subtitle.h"

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 * com.clov4r.ndktest.HelloJni
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
jstring


Java_com_clov4r_ndktest_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz ,
                                                  char *lib_path,
                                                  char * ffmpeg_filename,
                                                  char *file, int stream_index,
                                                  int current_time
                                                  )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#else
   #define ABI "unknown"
#endif
//32
	init_ffmpeg(lib_path,ffmpeg_filename);
//
	open_subtitle(file,stream_index);
	char *subtitle = get_subtitle_ontime(current_time);
	close_subtitle();
//	g_sub_p->subtitles_array[0];
//	char * result = "aaaddfffffff\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
//	LOG("subtitle = %s", subtitle);
//	jbyte *by = (jbyte*)result;
//	jbyteArray jarray = env->NewByteArray(strlen(result));
//	env->SetByteArrayRegion(jarray,0,strlen(result),by);

    return (*env)->NewStringUTF(env,subtitle);

}

