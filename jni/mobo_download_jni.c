#include <jni.h>
#include <string.h>
#include "cmp_log.h"
#include "mobo_download_remuxing.h"

#define MAX_STREAM_COUNT  15

JavaVM* jvm = NULL;
jobject java_object = NULL;

void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeStartDownload2(
		JNIEnv* env, jobject thiz, jstring streamingUrl, jstring fileSavePath,
		jint currentTime, jlong finishedSize) { //jlongArray ptsArray
	java_object = thiz;
	(*env)->GetJavaVM(env, &jvm);
	init_mutex_cond();
	set_download_flag(FLAG_DOWNLOAD_START);

	char *streaming_url = (*env)->GetStringUTFChars(env, streamingUrl, 0);
	char *save_path = (*env)->GetStringUTFChars(env, fileSavePath, 0);
	saving_network_media(streaming_url, save_path, currentTime, finishedSize);
	(*env)->DeleteLocalRef(env, streaming_url);
	(*env)->DeleteLocalRef(env, save_path);
}


void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeStartDownload(
		JNIEnv* env, jobject thiz, jstring streamingUrl, jstring fileSavePath,
		jlongArray ptsArray, jlong finishedSize) {
	java_object = thiz;
	(*env)->GetJavaVM(env, &jvm);
	init_mutex_cond();
	set_download_flag(FLAG_DOWNLOAD_START);

	jint array_len =
			ptsArray ? (*env)->GetArrayLength(env, ptsArray) : MAX_STREAM_COUNT;
	jlong p_array[array_len];
	int i;
	if (ptsArray) {
		(*env)->GetLongArrayRegion(env, ptsArray, 0, array_len, p_array);
		for (i = 0; i < array_len; i++) {
			LOG("download_streaming-->jni---pst_array[%d]=%lld...", i, p_array[i]);
		}
	} else
		for (i = 0; i < array_len; i++) {
			p_array[i] = 0;
		}

	char *streaming_url = (*env)->GetStringUTFChars(env, streamingUrl, 0);
	char *save_path = (*env)->GetStringUTFChars(env, fileSavePath, 0);
	saving_network_media(streaming_url, save_path, p_array, finishedSize);
	(*env)->DeleteLocalRef(env, streaming_url);
	(*env)->DeleteLocalRef(env, save_path);
	if (ptsArray)
		(*env)->ReleaseLongArrayElements(env, ptsArray, p_array, 0);
}

void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativePauseDownload(
		JNIEnv* env, jobject thiz) {
	if (get_download_flag() == FLAG_DOWNLOAD_START) {
		set_download_flag(FLAG_DOWNLOAD_PAUSE);
	}

}

void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeResumeDownload(
		JNIEnv* env, jobject thiz) {
	if (get_download_flag() == FLAG_DOWNLOAD_PAUSE) {
		set_download_flag(FLAG_DOWNLOAD_START);
	}
}
void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeStopDownload(
		JNIEnv* env, jobject thiz) {
	set_download_flag(FLAG_DOWNLOAD_STOP);
	des_mutex_cond();
//	java_object = NULL;
//	jvm = NULL;
}

jint Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeGetDuration(
		JNIEnv* env, jobject thiz) {
	return get_duration();
}

jint Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeGetCurrentTimeDownloadedTo(
		JNIEnv* env, jobject thiz) {
	return get_current_time_downloaded_to();
}

jint Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeGetStartDownloadedTime(
		JNIEnv* env, jobject thiz) {
	return get_start_downloaded_time();
}

//void onDownloadProgressChanged(JNIEnv* env, jobject thiz, jlong finished_size) {
//	jclass class = (*env)->GetObjectClass(env, thiz);
//	jmethodID onDownloadProgressChanged = (*env)->GetMethodID(env, class,
//			"onDownloadProgressChanged", "(IJ)V");
//	return (*env)->CallVoidMethod(env, thiz, onDownloadProgressChanged,
//			download_id, finished_size);
//}
//
//void onDownloadFinished(JNIEnv* env, jobject thiz) {
//	jclass class = (*env)->GetObjectClass(env, thiz);
//	jmethodID onDownloadFinished = (*env)->GetMethodID(env, class,
//			"onDownloadFinished", "(I)V");
//	return (*env)->CallVoidMethod(env, thiz, onDownloadFinished, download_id);
//}
//
//void onDownloadFailed(JNIEnv* env, jobject thiz) {
//	jclass class = (*env)->GetObjectClass(env, thiz);
//	jmethodID onDownloadFailed = (*env)->GetMethodID(env, class,
//			"onDownloadFailed", "(I)V");
//	return (*env)->CallVoidMethod(env, thiz, onDownloadFailed, download_id);
//}
