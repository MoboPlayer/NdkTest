/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 MoboPlayer.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <string.h>
#include <jni.h>
#include "mobo_open_subtitle.h"

/* This is a native method
 * open FFmpeg lib and open subtitle file.
 */
jstring
Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_openSubtitleFileInJNI( JNIEnv* env,
                                                  jobject thiz ,
                                                  jstring jfile, int stream_index
                                                  )
{


//    char *lib_path = (*env)->GetStringUTFChars(env, jlib_path, 0);
//    char * ffmpeg_filename = (*env)->GetStringUTFChars(env, jffmpeg_filename, 0);
    char *file = (*env)->GetStringUTFChars(env, jfile, 0);



//	LOGE("lib_path = %s", lib_path);

//	init_ffmpeg(lib_path,ffmpeg_filename);
	//LOGI("lib_path = %s", lib_path);
//
	open_subtitle(file,stream_index);

    return (*env)->NewStringUTF(env,"true");

}
/* This is a native method ,that is get subtitle by time ms.
 *
 */
jstring
Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleByTime( JNIEnv* env,
                                                  jobject thiz ,
                                                  int current_time
                                                  )
{
	char *subtitle = get_subtitle_ontime(current_time);
    return (*env)->NewStringUTF(env,subtitle);

}
