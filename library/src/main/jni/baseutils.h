
#ifndef __BASEUTILS_H__
#define __BASEUTILS_H__

#include <jni.h>
#include <android/log.h>
#include <libgen.h>

#ifndef LOG_TAG
#define LOG_TAG "SkBitmapHelper"
#endif

#define		NUM_ARRAY_ELEMENTS(p)		((int) sizeof(p) / sizeof(p[0]))

#define LOGV(FMT, ...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define LOGD(FMT, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define LOGI(FMT, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define LOGW(FMT, ...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define LOGE(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define LOGF(FMT, ...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "[%s:%d:%s]:"FMT,	\
        basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)

#if defined(__GNUC__)
// the macro for branch prediction optimaization for gcc(-O2/-O3 required)
#define		CONDITION(cond)				(__builtin_expect((cond)!=0, 0))
#define		LIKELY(x)					(__builtin_expect(!!(x), 1))	// x is likely true
#define		UNLIKELY(x)					(__builtin_expect(!!(x), 0))	// x is likely false
#else
#define		CONDITION(cond)				(cond)
#define		LIKELY(x)					(x)
#define		UNLIKELY(x)					(x)
#endif

void setVM(JavaVM *);
JavaVM *getVM();
JNIEnv *getEnv();
int getApiLevel(JNIEnv* env);

#define BAD_READ_PTR     (-1)
#define UNKNOWN_READ_PTR (0)
#define GOOD_READ_PTR    (1)
int checkBadReadPtr(void* ptr, size_t size);

#endif
