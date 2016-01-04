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

void exchange_image_type(AVSubtitleRect *ffregion, void* data_address);

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
JNIEXPORT jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_openSubtitleFileInJNI(
		JNIEnv* env, jobject thiz, jstring jfile, int stream_index, int subtiltle_index) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int result = open_subtitle(file, stream_index, subtiltle_index);

	return (jint) result;

}

JNIEXPORT void Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_closeSubtitle(
		JNIEnv* env, jobject thiz, int subtiltle_index) {
	close_subtitle(subtiltle_index);

}
JNIEXPORT jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_openSubtitleFileInJNI2(
		JNIEnv* env, jobject thiz, jstring jfile, int stream_index, int subtiltle_index) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int result = open_subtitle_2(file, stream_index, subtiltle_index);

	return (jint) result;

}

JNIEXPORT void Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_closeSubtitle2(
		JNIEnv* env, jobject thiz, int subtiltle_index) {
	close_subtitle_2(subtiltle_index);

}
JNIEXPORT jstring Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleLanguage(
		JNIEnv* env, jobject thiz, jstring jfile) {

	char *file = (*env)->GetStringUTFChars(env, jfile, 0);
//	char *result = get_subtitle_language(subtiltle_index);
	char text[500];
	sj_get_sa_info(file,text);
	LOGE("the language is strcpy   info:%s",text);
	return (*env)->NewStringUTF(env, text);

}

JNIEXPORT jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleType(
		JNIEnv* env, jobject thiz, int subtiltle_index) {


	int result = get_subtitle_type(subtiltle_index);

	return (jint)result;

}

/* This is a native method ,that is get subtitle by time ms.
 *
 */
JNIEXPORT jstring Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleByTime(
		JNIEnv* env, jobject thiz, int current_time, int subtiltle_index) {
	if(VERSION_FLAG == 1)
		return NULL;
	char *subtitle = get_subtitle_ontime(current_time, subtiltle_index);
	return (*env)->NewStringUTF(env, subtitle);

}
/* This is a native method ,that is get subtitle by time ms.
 *
 */
JNIEXPORT jstring Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleByTime2(
		JNIEnv* env, jobject thiz, int current_time, int subtiltle_index, int time_diff) {
	if(VERSION_FLAG == 1)
		return NULL;
	char *subtitle = get_subtitle_ontime_2(current_time, subtiltle_index,time_diff,NULL);
	return (*env)->NewStringUTF(env, subtitle);

}

jobject create_subtitle_bitmap(JNIEnv* env, jobject thiz, jobject bitmap_data, jint width,
		jint height) {
	jclass cls = (*env)->GetObjectClass(env, thiz);
	jmethodID createBitmap = (*env)->GetMethodID(env, cls, "createBitmapOfSubtitle",
			"(Ljava/nio/ByteBuffer;II)Landroid/graphics/Bitmap;");
//	(*env)->CallVoidMethod(env, thiz, createBitmap, bitmap_data, size,
//			video_name);
	return (*env)->CallObjectMethod(env, thiz, createBitmap, bitmap_data, width,
			height);
}

JNIEXPORT jobject Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_getSubtitleByTime3(
		JNIEnv* env, jobject thiz, int current_time, int subtiltle_index, int time_diff,jobject buffer) {
	if(VERSION_FLAG == 1)
		return NULL;
	AVPicture *av_picture;
	AVSubtitleRect *sub_rect = NULL;
	void *b = 0;
	jobject bitmap_data = NULL;
	sub_rect = (AVSubtitleRect *) malloc(sizeof(AVSubtitleRect));
	memset(sub_rect, 0, sizeof(AVSubtitleRect));
	get_subtitle_ontime_2(current_time, subtiltle_index,time_diff, sub_rect);
	LOG("open_subtitle-->get_subtitle_ontime_3 end");
	if(sub_rect){
		av_picture = &(sub_rect->pict);
		LOG("open_subtitle--> sub_rect->w = %d,sub_rect->h=%d", sub_rect->w, sub_rect->h);
		if (sub_rect->w > 0 && sub_rect->h > 0) {

		    void *b = NULL;
		    b = (*env)->GetDirectBufferAddress(env, buffer);
		    if (!b) {
		        return NULL;
		    }

		    exchange_image_type(sub_rect, b);

		    return create_subtitle_bitmap(env, thiz, buffer, sub_rect->w, sub_rect->h);

//			int byte_count = av_picture->linesize[0] * sub_rect->h;
//			bitmap_data = init_byte_buffer(env, thiz, byte_count);
//			b = (*env)->GetDirectBufferAddress(env, bitmap_data);
//			memcpy(b, av_picture->data[0], byte_count);
//			free_avpicture(av_picture);
//			return create_subtitle_bitmap(env, thiz, bitmap_data, sub_rect->w, sub_rect->h);

		}
	}
	return NULL;
}

void exchange_image_type(AVSubtitleRect *ffregion, void* data_address){
    uint32_t *byte_buffer_p = (uint32_t *)data_address;

    int x = 0;
    int y = 0;
    for (y = 0; y < ffregion->h; y++) {
        for (x = 0; x < ffregion->w; x++) {
            /*  I don't think don't have paletized RGB_A_ */
            const uint8_t index = ffregion->pict.data[0][y * ffregion->w+x];
            uint32_t color;
            memcpy(&color, &ffregion->pict.data[1][4*index], 4);

            uint32_t *p = &(byte_buffer_p[y * ffregion->w+x]);

            uint8_t *p_rgba = (uint8_t *)p;
            p_rgba[0] = (color >> 16) & 0xff;
            p_rgba[1] = (color >>  8) & 0xff;
            p_rgba[2] = (color >>  0) & 0xff;
            p_rgba[3] = (color >> 24) & 0xff;
        }
    }
}

JNIEXPORT jint Java_com_clov4r_moboplayer_android_nil_codec_SubtitleJni_isSubtitleExits(
		JNIEnv* env, jobject thiz, jstring jfile) {
	char *file = (*env)->GetStringUTFChars(env, jfile, 0);

	int is_exits = is_subtitle_exits(file);
	return (jint) is_exits;

}

