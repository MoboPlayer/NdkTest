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

/* This is a native method
 * open FFmpeg lib and open subtitle file.
 */
jstring
Java_com_clov4r_ndktest_SubtitleJni_openFFmpegAndSubtitleFileInJNI( JNIEnv* env,
                                                  jobject thiz ,
                                                  jstring jlib_path,
                                                  jstring jffmpeg_filename,
                                                  jstring jfile, int stream_index
                                                  )
{


    char *lib_path = (*env)->GetStringUTFChars(env, jlib_path, 0);
    char * ffmpeg_filename = (*env)->GetStringUTFChars(env, jffmpeg_filename, 0);
    char *file = (*env)->GetStringUTFChars(env, jfile, 0);



	LOGE("lib_path = %s", lib_path);

	init_ffmpeg(lib_path,ffmpeg_filename);
	//LOGI("lib_path = %s", lib_path);
//
	open_subtitle(file,stream_index);

    return (*env)->NewStringUTF(env,"true");

}
/* This is a native method ,that is get subtitle by time ms.
 *
 */
jstring
Java_com_clov4r_ndktest_SubtitleJni_getSubtitleByTime( JNIEnv* env,
                                                  jobject thiz ,
                                                  int current_time
                                                  )
{
	char *subtitle = get_subtitle_ontime(current_time);
    return (*env)->NewStringUTF(env,subtitle);

}
