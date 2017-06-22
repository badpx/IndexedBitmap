
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

// Android API Level list:
#define _M_   23
#define _LOLLIPOP_MR1_    22
#define _LOLLIPOP_    21
#define _KITKAT_WATCH_    20
#define _KITKAT_  19
#define _JELLY_BEAN_MR2_  18
#define _JELLY_BEAN_MR1_  17
#define _JELLY_BEAN_  16
#define _ICE_CREAM_SANDWICH_MR1_  15
#define _ICE_CREAM_SANDWICH_  14
#define _HONEYCOMB_MR2_   13
#define _HONEYCOMB_MR1_   12
#define _HONEYCOMB_   11
#define _GINGERBREAD_MR1_ 10
#define _GINGERBREAD_ 9
#define _FROYO_   8
#define _ECLAIR_MR1_  7
#define _ECLAIR_0_1_  6
#define _ECLAIR_  5
#define _DONUT_   4
#define _CUPCAKE_ 3
#define _BASE_1_1_    2
#define _BASE_    1

void setVM(JavaVM *);
JavaVM *getVM();
JNIEnv *getEnv();
int getApiLevel(JNIEnv* env);

#define BAD_READ_PTR     (-1)
#define UNKNOWN_READ_PTR (0)
#define GOOD_READ_PTR    (1)
int checkBadReadPtr(void* ptr, size_t size);

#endif
