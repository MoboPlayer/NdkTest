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
//	jclass Class_string;
//	jmethodID mid_String, mid_getBytes;
//	jbyteArray bytes;
//	jbyte* log_utf8;
//	jstring codetype, jstr;
//	Class_string = (*env)->FindClass(env, "java/lang/String"); //获取class
////先将gbk字符串转为java里的string格式
//	mid_String = (*env)->GetMethodID(env, Class_string, "<init>",
//			"([BLjava/lang/String;)V");
//	bytes = (*env)->NewByteArray(env, strlen(tmpstr));
//	(*env)->SetByteArrayRegion(env, bytes, 0, strlen(tmpstr), (jbyte*) tmpstr);
//	codetype = (*env)->NewStringUTF(env, "utf-8");
//	jstr = (jstring) (*env)->NewObject(env, Class_string, mid_String, bytes,
//			codetype);
//
//	(*env)->DeleteLocalRef(env, bytes);
//
////再将string变utf-8字符串。
//	mid_getBytes = (*env)->GetMethodID(env, Class_string, "getBytes",
//			"(Ljava/lang/String;)[B");
//	codetype = (*env)->NewStringUTF(env, "utf-8");
//	bytes = (jbyteArray) (*env)->CallObjectMethod(env, jstr, mid_getBytes,
//			codetype);
//	log_utf8 = (*env)->GetByteArrayElements(env, bytes, JNI_FALSE);
//
//	return (*env)->NewStringUTF(env, log_utf8);

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
		JNIEnv* env, jobject thiz, jstring jfile, int stream_index) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int result = open_subtitle(file, stream_index);

	return (jint) result;

}

void Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_closeSubtitle(
		JNIEnv* env, jobject thiz) {
	close_subtitle();

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
		JNIEnv* env, jobject thiz, int current_time) {
	char *subtitle = get_subtitle_ontime(current_time);
	return (*env)->NewStringUTF(env, subtitle);

}
jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_isSubtitleExits(
		JNIEnv* env, jobject thiz, jstring jfile) {
	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int is_exits = is_subtitle_exits(file);
	return (jint) is_exits;

}

