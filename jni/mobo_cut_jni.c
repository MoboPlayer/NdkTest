#include <jni.h>
#include <string.h>
#include "cmp_log.h"

extern int ffmpeg(int argc, char **argv, void (*set_progress)(int progress));

static JavaVM* jvm = NULL;
static jobject obj_video_cut_lib = NULL;

static int JniThreadAttach(int *needDetach, JNIEnv **_env) {
	int getEnvStat;
	if (_env == NULL)
		return -1;

	*needDetach = 0;
	getEnvStat = (*jvm)->GetEnv(jvm, (void**)_env, JNI_VERSION_1_6);
	if (getEnvStat == JNI_EDETACHED) {
		if ((*jvm)->AttachCurrentThread(jvm, _env, NULL) == 0) {
			*needDetach = 1;
			return 0;
		} else {
			return -1;
		}
	}

	return 0;
}

static void JniThreadDetach(int needDetach) {
	if (needDetach)
		(*jvm)->DetachCurrentThread(jvm);
}

void set_progress(int currentTime){
	JNIEnv *env = NULL;
	jclass class;
	jmethodID mid;

	int needDetach = 0;
	int attachStatus = -1;

	attachStatus = JniThreadAttach(&needDetach, &env);
	if(attachStatus == -1)
		return -1;

	class = (*env)->GetObjectClass(env, obj_video_cut_lib);
	if(class == NULL)
		goto error;

	mid = (*env)->GetMethodId(env, class, "setProgress", "(I)V");
	if(mid == NULL)
		goto error;

	(*env)->CallIntMethod(env, obj_video_cut_lib, mid, currentTime);

error:
	JniThreadDetach(needDetach);
}

JNIEXPORT int Java_com_clov4r_moboplayer_android_videocut_VideoCutLib_cutVideo(
		JNIEnv* env, jobject thiz, jint command_num,jobjectArray command_array) {
	if (obj_video_cut_lib == NULL)
		obj_video_cut_lib = (*env)->NewGlobalRef(env, thiz);
	if(!jvm)
		(*env)->GetJavaVM(env, &jvm);

	char *commands[100];
	int i=0;
	for(i=0;i<command_num;i++){
	    jstring obja = (jstring)(*env)->GetObjectArrayElement(env,command_array,i);
	    commands[i] = (*env)->GetStringUTFChars(env,obja,NULL);
	}
	int ret = ffmpeg(command_num, commands, set_progress);
	return ret;
}

JNIEXPORT int Java_com_clov4r_moboplayer_android_videocut_VideoCutLib_stopCut(
		JNIEnv* env, jobject thiz) {
	sigterm_handler(1);

	(*env)->DeleteGlobalRef(env, obj_video_cut_lib);
    obj_video_cut_lib = NULL;
}
