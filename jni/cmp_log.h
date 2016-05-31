/*
 * cmp_log.h
 *
 *  Created on: 2010-5-5
 *      Author: water.e@gmail.com
 */

#ifndef CMP_LOG_H_
#define CMP_LOG_H_

#ifdef NEED_ANDROID_LOG

#include <android/log.h>

#define  LOG_TAG    "NDKTest"

/*
	#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
	#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


	#define  LOGV(...)  __android_log_print(ANDROID_LOG_INFO,"player",__VA_ARGS__)
	#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"player",__VA_ARGS__)
//	#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"player",__VA_ARGS__)
	#define  LOGD1(...) __android_log_print(ANDROID_LOG_DEBUG,"player",__VA_ARGS__)
	#define  LOGD2(...) __android_log_print(ANDROID_LOG_DEBUG,"OOOplayer",__VA_ARGS__)
	#define  LOG LOGD

*/
	#define LOGD
	#define LOGV
	#define LOGE
	#define LOGV1
	#define LOGV2
	#define LOG LOGD
	#define LOGD1 LOGD
	#define LOGD2 LOGD
#else

#define LOGD
#define LOGV
#define LOGV1
#define LOGV2
#define LOG LOGD
#define LOGD1 LOGD
#define LOGD2 LOGD

#endif


#endif /* CMP_LOG_H_ */
