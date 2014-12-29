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

jstring ctojstring(JNIEnv *env, char* tmpstr)
{

	jclass strClass = (*env)->FindClass(env, "[Ljava/lang/String;");
	jmethodID ctorID = (*env)->GetMethodID(env, strClass, "()", "([BLjava/lang/String;)V");
	jbyteArray bytes = (*env)->NewByteArray(env, strlen(tmpstr));
	(*env)->SetByteArrayRegion(env, bytes, 0, strlen(tmpstr), (jbyte*)tmpstr);
	jstring encoding = (*env)->NewStringUTF(env, "utf-8");
	return (jstring)(*env)->NewObject(env, strClass, ctorID, bytes, encoding);
}

/* This is a native method
 * open FFmpeg lib and open subtitle file.
 */
jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_openSubtitleFileInJNI(
		JNIEnv* env, jobject thiz, jstring jfile, int stream_index, int subtiltle_index) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int result = open_subtitle(file, stream_index, subtiltle_index);

	return (jint) result;

}

void Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_closeSubtitle(
		JNIEnv* env, jobject thiz, int subtiltle_index) {
	close_subtitle(subtiltle_index);

}

jstring Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleLanguage(
		JNIEnv* env, jobject thiz, jstring jfile, int stream_index) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	char *result = get_subtitle_language(file, stream_index);

	jstring jresult = ctojstring(env,result);
	LOGE("the language is strcpy jni:%s", jresult);
	return jresult;

}

/* This is a native method ,that is get subtitle by time ms.
 *
 */
jstring Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleByTime(
		JNIEnv* env, jobject thiz, int current_time, int subtiltle_index) {
	char *subtitle = get_subtitle_ontime(current_time, subtiltle_index);
	return (*env)->NewStringUTF(env, subtitle);

}
jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_isSubtitleExits(
		JNIEnv* env, jobject thiz, jstring jfile) {
	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int is_exits = is_subtitle_exits(file);
	return (jint) is_exits;

}

