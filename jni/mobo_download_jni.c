#include <jni.h>
#include <string.h>
#include "cmp_log.h"
#include "mobo_download_remuxing.h"

jint download_id;

JavaVM* jvm = NULL;
jobject java_object = NULL;

void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeStartDownload(
		JNIEnv* env, jobject thiz, jstring streamingUrl, jstring fileSavePath,
		jint id, jlongArray ptsArray) {
	java_object = thiz;
	(*env)->GetJavaVM(env, &jvm);
	download_id = id;
	set_download_flag(flag_download_start);
	char *streaming_url = (*env)->GetStringUTFChars(env, streamingUrl, 0);
	char *save_path = (*env)->GetStringUTFChars(env, fileSavePath, 0);
	saving_network_media(streaming_url, save_path, ptsArray);
}
void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativePauseDownload(
		JNIEnv* env, jobject thiz, jint id) {
	if (download_id == id && get_download_flag() == flag_download_start) {
		download_id = id;
		set_download_flag(flag_download_pause);
	}

}

void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeResumeDownload(
		JNIEnv* env, jobject thiz, jint id) {
	if (download_id == id && get_download_flag() == flag_download_pause) {
		download_id = id;
		set_download_flag(flag_download_pause);
	}

}
void Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeStopDownload(
		JNIEnv* env, jobject thiz, jint id) {
	if (download_id == id) {
		download_id = id;
		set_download_flag(flag_download_stop);
	}

}

jint Java_com_clov4r_moboplayer_android_nil_codec_StreamingDownloadLib_nativeGetDuration(
		JNIEnv* env, jobject thiz, jint id) {
	if (download_id == id && get_download_flag() != flag_download_stop) {
		return get_duration();
	}
	return 0;
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
