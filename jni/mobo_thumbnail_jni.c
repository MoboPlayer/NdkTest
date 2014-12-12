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

jstring Java_com_clov4r_moboplayer_android_nil_codec_ScreenShotLibJni_getThumbnail(JNIEnv* env,
		jobject thiz, jstring video_name, jobject bitmap_data, jint gen_pos,
		jint width, jint height) {
	void *b = 0;
	int img_width, img_height;
    char *video_path = (*env)->GetStringUTFChars(env, video_name, 0);
	b = (*env)->GetDirectBufferAddress(env,bitmap_data);
	if (b) {
		AVPicture* av_picture = get_rgb24_picture(video_path, gen_pos,
				&img_width, &img_height);
//		LOG("get_rgb24_picture----%d,%d,%d",img_width,img_height,av_picture->linesize[0]);
		memcpy(b,av_picture->data[0],av_picture->linesize[0]*img_height);
		free_avpicture(av_picture);
		char res[50];
		sprintf(res,"%d,%d",img_width,img_height);
	    return (*env)->NewStringUTF(env,res);
	}

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
