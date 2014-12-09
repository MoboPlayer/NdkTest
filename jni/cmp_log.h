/*
 * cmp_log.h
 *
 *  Created on: 2010-5-5
 *      Author: water.e@gmail.com
 */

#ifndef CMP_LOG_H_
#define CMP_LOG_H_



#ifdef ANDROID

#ifdef NEED_ANDROID_LOG


#include <cutils/log.h>
#include <android/log.h>

	#define  LOGV(...)  __android_log_print(ANDROID_LOG_INFO,"player",__VA_ARGS__)
	#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"player",__VA_ARGS__)
	#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"player",__VA_ARGS__)
	#define  LOGD1(...) __android_log_print(ANDROID_LOG_DEBUG,"player",__VA_ARGS__)
	#define  LOGD2(...) __android_log_print(ANDROID_LOG_DEBUG,"OOOplayer",__VA_ARGS__)
	#define  LOG LOGD

/*  
	#define LOGD
	#define LOGV
	#define LOGE
	#define LOGV1
	#define LOGV2
	#define LOG LOGD
	#define LOGD1 LOGD
	#define LOGD2 LOGD
*/
#else

#define LOGD
#define LOGV
#define LOGV1
#define LOGV2
#define LOG LOGD
#define LOGD1 LOGD
#define LOGD2 LOGD

#endif





#else
static void LOG(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	if(FILE* f = fopen("c:\\ffmpeg.txt", "at"))
	{
		fseek(f, 0, 2);
		fprintf(f, fmt, args);
		fclose(f);
	}
	va_end(args);
}
#endif

#endif /* CMP_LOG_H_ */
