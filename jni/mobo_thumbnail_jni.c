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
 *
 */

#include "mobo_thumbnail.h"
#include <string.h>
#include <stdlib.h>
#include <jni.h>
#include "cmp_log.h"

jobject init_byte_buffer(JNIEnv* env, jobject thiz, int size) {
	char* classname = "java/nio/ByteBuffer";
	jclass byte_buffer_class = (*env)->FindClass(env, classname);
	jmethodID method_id = (*env)->GetStaticMethodID(env, byte_buffer_class,
			"allocateDirect", "(I)Ljava/nio/ByteBuffer;");
	jobject bitmap_data = (*env)->CallStaticObjectMethod(env, byte_buffer_class,
			method_id, size);
	if (bitmap_data)
		LOG("bytebuffer is not null");
	else
		LOG("bytebuffer is null");
	return bitmap_data;
}
void create_bitmap(JNIEnv* env, jobject thiz, jobject bitmap_data, jstring size,
		jstring video_name) {
	jclass cls = (*env)->GetObjectClass(env, thiz);
	jmethodID createBitmap = (*env)->GetMethodID(env, cls, "createBitmap",
			"(Ljava/nio/ByteBuffer;Ljava/lang/String;Ljava/lang/String;)V");
	(*env)->CallVoidMethod(env, thiz, createBitmap, bitmap_data, size,
			video_name);
}

jstring get_thumbnail(JNIEnv* env, jobject thiz, jstring video_name,
		jint gen_pos, jint width, jint height, int gen_IDR_frame) {
//	LOG("get_rgb24_picture---get_thumbnail start...");
	void *b = 0;
	jstring screen_shot_size;
	jobject bitmap_data = NULL;
	int img_width = (int) width <= 0 ? -1 : width;
	int img_height = (int) height <= 0 ? -1 : height;
	char *video_path = (*env)->GetStringUTFChars(env, video_name, 0);
	AVPicture* av_picture = get_rgb24_picture(video_path, gen_pos, &img_width,
			&img_height, gen_IDR_frame);
	if (av_picture) {
		int byte_count = av_picture->linesize[0] * img_height;
		bitmap_data = init_byte_buffer(env, thiz, byte_count);
		b = (*env)->GetDirectBufferAddress(env, bitmap_data);
//	LOG("get_rgb24_picture----%d,%d,%d", img_width, img_height, av_picture->linesize[0]);
		memcpy(b, av_picture->data[0], byte_count);

		free_avpicture(av_picture);
	} else
		LOG("gen_thumbnail---get picture data null...");
	char res[50];
	sprintf(res, "%d,%d", img_width, img_height);
	screen_shot_size = (*env)->NewStringUTF(env, res);
	create_bitmap(env, thiz, bitmap_data, screen_shot_size, video_name);
	return screen_shot_size;
}

jstring Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getThumbnail(
		JNIEnv* env, jobject thiz, jstring video_name, jint gen_pos, jint width,
		jint height) {
	return get_thumbnail(env, thiz, video_name, gen_pos, width, height, 0);

}

jstring Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getIDRThumbnail(
		JNIEnv* env, jobject thiz, jstring video_name, jint width, jint height) {
	jstring size = get_thumbnail(env, thiz, video_name, 0, width, height, 1);
	return size;
}

/**
 *
 extern jstring com_clov4r_ndktest_ScreenShotLib_nativeGetSubtitleBuffer(
 JNIEnv* env, jobject thiz, jint h_index, jint current_time) {
 char buffer_info[1024];
 memset(buffer_info, 0, sizeof(buffer_info));
 int cur_time_s = current_time / 1000;
 LOG("sj_get_subtitle_buffer called");
 sj_get_subtitle_buffer(buffer_info, cur_time_s, g_sub_data_p[h_index]);
 LOG("subtitle buffer %s", buffer_info);
 return env->NewStringUTF(buffer_info);
 }

 extern jint com_clov4r_ndktest_ScreenShotLib_nativeSetSubtitleBuffer(
 JNIEnv* env, jobject thiz, jobject buffer) {
 void *b = NULL;
 b = (*env)->GetDirectBufferAddress(buffer);
 if (b) {
 int h, w = 0;
 sprintf(sr, "%d,%d", w, h);
 //        sj_set_subtitle_buffer(b, g_sub_data_p[h_index]);
 AVPicture* av_picture = *get_rgb24_picture(&w);
 return 1;
 }

 return -1;
 }
 */
