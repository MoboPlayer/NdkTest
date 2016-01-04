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

#define max_image_buffer 4 * 1024 * 1024

jobject init_byte_buffer(JNIEnv* env, jobject thiz, int size) {
	char* classname = "java/nio/ByteBuffer";
	jclass byte_buffer_class = (*env)->FindClass(env, classname);
	jmethodID method_id = (*env)->GetStaticMethodID(env, byte_buffer_class,
			"allocateDirect", "(I)Ljava/nio/ByteBuffer;");
	jobject bitmap_data = (*env)->CallStaticObjectMethod(env, byte_buffer_class,
			method_id, size);
	return bitmap_data;
}
jobject create_bitmap(JNIEnv* env, jobject thiz, jobject bitmap_data, jstring size,
		jstring video_name) {
	jclass cls = (*env)->GetObjectClass(env, thiz);
	jmethodID createBitmap = (*env)->GetMethodID(env, cls, "createBitmap",
			"(Ljava/nio/ByteBuffer;Ljava/lang/String;Ljava/lang/String;)Landroid/graphics/Bitmap;");
//	(*env)->CallVoidMethod(env, thiz, createBitmap, bitmap_data, size,
//			video_name);
	return (*env)->CallObjectMethod(env, thiz, createBitmap, bitmap_data, size,
			video_name);
}

jobject create_bitmap_1(JNIEnv* env, jobject thiz, jstring file_name, jstring img_path, jint width, jint height) {
	jclass cls = (*env)->GetObjectClass(env, thiz);
	jmethodID createBitmap = (*env)->GetMethodID(env, cls, "createBitmap",
			"(Ljava/lang/String;Ljava/lang/String;II)Landroid/graphics/Bitmap;");
//	(*env)->CallVoidMethod(env, thiz, createBitmap, bitmap_data, size,
//			video_name);
	return (*env)->CallObjectMethod(env, thiz, createBitmap, file_name, img_path,
			width, height);
}

jobject create_bitmap_2(JNIEnv* env, jobject thiz, jstring video_path, jstring img_path, int8_t *data, int length, int width, int height) {//int res
	jclass cls = (*env)->GetObjectClass(env, thiz);
	jmethodID createBitmap = (*env)->GetMethodID(env, cls, "createBitmap",
			"(Ljava/lang/String;Ljava/lang/String;[BII)Landroid/graphics/Bitmap;");//I
	jbyteArray  jbarray = (*env)->NewByteArray(env, length);
	jbyte *jy=(jbyte*)data;
	(*env)->SetByteArrayRegion(env,jbarray, 0, length, jy);
	return (*env)->CallObjectMethod(env, thiz, createBitmap, video_path, img_path, jbarray, width, height);//res
}

void free_byte_buffer(JNIEnv* env, jobject bitmap_data) {
	void *buffer = (*env)->GetDirectBufferAddress(env,bitmap_data);
	(*env)->DeleteGlobalRef(env,bitmap_data);
	free(buffer);
}

//jstring get_thumbnail(JNIEnv* env, jobject thiz, jstring video_name,
//		jint gen_pos, jint width, jint height, int gen_IDR_frame) {
////	LOG("get_rgb24_picture---get_thumbnail start...");
//	void *b = 0;
//	jstring screen_shot_size;
//	jobject bitmap_data = NULL;
//	int img_width = (int) width <= 0 ? -1 : width;
//	int img_height = (int) height <= 0 ? -1 : height;
//	char *video_path = (*env)->GetStringUTFChars(env, video_name, 0);
//	AVPicture* av_picture = get_rgb24_picture(video_path, gen_pos, &img_width,
//			&img_height, gen_IDR_frame);
//	if (av_picture) {
//		int byte_count = av_picture->linesize[0] * img_height;
//		bitmap_data = init_byte_buffer(env, thiz, byte_count);
//		b = (*env)->GetDirectBufferAddress(env, bitmap_data);
////	LOG("get_rgb24_picture----%d,%d,%d", img_width, img_height, av_picture->linesize[0]);
//		memcpy(b, av_picture->data[0], byte_count);
//
//		free_avpicture(av_picture);
//	}
//	char res[50];
//	sprintf(res, "%d,%d", img_width, img_height);
//	screen_shot_size = (*env)->NewStringUTF(env, res);
//	create_bitmap(env, thiz, bitmap_data, screen_shot_size, video_name);
//	return screen_shot_size;
//}

jobject get_thumbnail_sync_2(JNIEnv* env, jobject thiz, jstring video_name, jstring image_name,
		jint gen_pos, jint width, jint height, int frameType){
	int save_img_res;
	void *b = 0;
	jstring screen_shot_size = NULL;
	jobject bitmap_data = NULL;
	int img_width = (int) width <= 0 ? -1 : width;
	int img_height = (int) height <= 0 ? -1 : height;
	int pic_fmt = AV_PIX_FMT_RGB565;

	int max_size = 4;
	if(pic_fmt == AV_PIX_FMT_RGB565)
		max_size = 2;
	else if(AV_PIX_FMT_RGB24 == pic_fmt)
		max_size = 3;
	while(max_image_buffer < img_width * img_height * max_size){
		img_width /= 2;
		img_height /= 2;
	}

	LOG("get_thumbnail_sync2 img_width=%d,img_height=%d",img_width,img_height);

	mobo_thumbnail_data *thumbnail_data = malloc(sizeof(mobo_thumbnail_data));
	if(!thumbnail_data)
		return NULL;
	memset(thumbnail_data, 0, sizeof(mobo_thumbnail_data));
	LOG("get_thumbnail_sync2 alloc thumb_data");
	char *video_path = (*env)->GetStringUTFChars(env, video_name, 0);
	LOG("get_thumbnail_sync2 create video name 1");
	char *image_path = (*env)->GetStringUTFChars(env, image_name, 0);
	LOG("get_thumbnail_sync2 create video name 2");
	int gen_IDR_frame = 0;
	if (frameType > 0)
		gen_IDR_frame = 1;
	else if (frameType < 0)
		thumbnail_data->need_key_frame = 1;
	else
		thumbnail_data->need_key_frame = 0;
	AVPicture* av_picture = get_rgb24_picture(video_path, gen_pos, &img_width,
			&img_height, gen_IDR_frame, thumbnail_data, pic_fmt);//AV_PIX_FMT_ARGB
	LOG("get_thumbnail_sync2 av_picture=%p", av_picture);
	if (av_picture) {
		save_img_res = write_png_file(image_path, img_width, img_height, av_picture->data[0]);
		if(save_img_res >= 0)
			bitmap_data = create_bitmap_1(env, thiz, video_name, image_name, img_width, img_height);
		else
			bitmap_data = create_bitmap_2(env, thiz, video_name, image_name, av_picture->data[0], av_picture->linesize[0] * img_height, img_width, img_height);//save_img_res
		free_avpicture(av_picture);
		LOG("get_thumbnail_sync2 release av_picture");
	}
//	jobject obj = create_bitmap_2(env, thiz, video_name, image_name, av_picture->data[0], av_picture->linesize[0] * img_height, img_width, img_height);//save_img_res
//	LOG("get_thumbnail_sync2 length = %d", av_picture->linesize[0] * img_height);
//	free_avpicture(av_picture);
	LOG("get_thumbnail_sync2 created bitmap");
    (*env)->ReleaseStringUTFChars(env, video_name, video_path);
    (*env)->ReleaseStringUTFChars(env, image_name, image_path);
	LOG("get_thumbnail_sync2 finished");
	free(thumbnail_data);
	return bitmap_data;
}

//deprecated
jobject get_thumbnail_sync(JNIEnv* env, jobject thiz, jstring video_name,
		jint gen_pos, jint width, jint height, int frameType){
	void *b = 0;
	jstring screen_shot_size = NULL;
	jobject bitmap_data = NULL;
	int img_width = (int) width <= 0 ? -1 : width;
	int img_height = (int) height <= 0 ? -1 : height;

	while(max_image_buffer < img_width * img_height *4){
		img_width /= 2;
		img_height /= 2;
	}

	LOG("get_thumbnail_sync img_width=%d,img_height=%d",img_width,img_height);

	mobo_thumbnail_data *thumbnail_data = malloc(sizeof(mobo_thumbnail_data));
	if(!thumbnail_data)
		return NULL;
	memset(thumbnail_data, 0, sizeof(mobo_thumbnail_data));
	char *video_path = (*env)->GetStringUTFChars(env, video_name, 0);
	int gen_IDR_frame = 0;
	if (frameType > 0)
		gen_IDR_frame = 1;
	else if (frameType < 0)
		thumbnail_data->need_key_frame = 1;
	AVPicture* av_picture = get_rgb24_picture(video_path, gen_pos, &img_width,
			&img_height, gen_IDR_frame, thumbnail_data, AV_PIX_FMT_ARGB);
	LOG("get_thumbnail_sync av_picture=%p", av_picture);
	if (av_picture) {
		int byte_count = av_picture->linesize[0] * img_height;
		LOG("get_thumbnail_sync byte_count=%d",byte_count);
		bitmap_data = init_byte_buffer(env, thiz, byte_count);
		LOG("get_thumbnail_sync allocat after");
		if (bitmap_data){
			b = (*env)->GetDirectBufferAddress(env, bitmap_data);
			memcpy(b, av_picture->data[0], byte_count);
			LOG("get_thumbnail_sync after memcpy");
		}
		free_avpicture(av_picture);
	}
	char res[50];
	sprintf(res, "%d,%d", img_width, img_height);
	screen_shot_size = (*env)->NewStringUTF(env, res);
	jobject obj = create_bitmap(env, thiz, bitmap_data, screen_shot_size, video_name);
    (*env)->ReleaseStringUTFChars(env, video_name, video_path);
	free(thumbnail_data);
	LOG("get_thumbnail_sync finished");
	return obj;
}

JNIEXPORT void Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_releaseByteBuffer(
		JNIEnv* env, jobject thiz, jobject buffer_data) {
	free_byte_buffer(env,buffer_data);
}

JNIEXPORT jobject Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getThumbnail(
		JNIEnv* env, jobject thiz, jstring video_name, jstring img_name, jint gen_pos, jint width,
		jint height) {
	if(VERSION_FLAG == 1)
		return NULL;
	return get_thumbnail_sync_2(env, thiz, video_name, img_name, gen_pos, width, height, 0);

}

JNIEXPORT jobject Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getKeyFrameThumbnail(
		JNIEnv* env, jobject thiz, jstring video_name, jint gen_pos, jint width,
		jint height) {
	if(VERSION_FLAG == 1)
		return NULL;
	return get_thumbnail_sync(env, thiz, video_name, gen_pos, width, height, -1);

}

JNIEXPORT jobject Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getKeyFrameThumbnail2(
		JNIEnv* env, jobject thiz, jstring video_name, jstring img_name, jint gen_pos, jint width,
		jint height) {
	if(VERSION_FLAG == 1)
		return NULL;
	return get_thumbnail_sync_2(env, thiz, video_name, img_name, gen_pos, width, height, -1);

}

JNIEXPORT jobject Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getIDRThumbnail(
		JNIEnv* env, jobject thiz, jstring video_name, jstring img_name, jint width, jint height) {
	if(VERSION_FLAG == 1)
		return NULL;
	return get_thumbnail_sync_2(env, thiz, video_name, img_name, 0, width, height, 1);
}

