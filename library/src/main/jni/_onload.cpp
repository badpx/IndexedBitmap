#include <android/log.h>
#include "baseutils.h"
#include "_onload.h"
#include "skbitmap_helper.h"

#define LOCAL_DEBUG 0

static JNINativeMethod methods[] = {
    { "nativeInit", "(Landroid/graphics/Bitmap;)Z", (void*)Init },
    { "nativeGetBytesPerPixel", "(Landroid/graphics/Bitmap;)I", (void*)GetBytesPerPixel },
    { "nativeGetColorTable", "(Landroid/graphics/Bitmap;[I)I", (void*)GetColorTable },
    { "nativeChangeColorTable", "(Landroid/graphics/Bitmap;[I)I", (void*)ChangeColorTable },
    { "nativeIndex8FakeToAlpha8", "(Landroid/graphics/Bitmap;Z)I", (void*)Index8FakeToAlpha8 },
    { "nativeGetConfig", "(Landroid/graphics/Bitmap;)I", (void*)GetConfig },
    { "nativeSetConfig", "(Landroid/graphics/Bitmap;I)I", (void*)SetConfig },
};

jint registerNativeMethods(JNIEnv* env, const char *class_name, JNINativeMethod *methods, int num_methods) {
	int result = 0;

	jclass clazz = env->FindClass(class_name);
	if (LIKELY(clazz)) {
		int result = env->RegisterNatives(clazz, methods, num_methods);
		if (UNLIKELY(result < 0)) {
			LOGE("registerNativeMethods failed(class=%s)", class_name);
		}
	} else {
		LOGE("registerNativeMethods: class'%s' not found", class_name);
	}
	return result;
}

static int register_native_methods(JNIEnv *env) {
	LOGV("register native method:");
	if (registerNativeMethods(env,
		"com/badpx/indexbitmap/BitmapHelper",
		methods, NUM_ARRAY_ELEMENTS(methods)) < 0) {
		return -1;
	}
    return 0;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
#if LOCAL_DEBUG
    LOGI("JNI_OnLoad");
#endif

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    if (!setupLibrary(env)) {
        return JNI_ERR;
    }

    // register native methods
    int result = register_native_methods(env);
	setVM(vm);

#if LOCAL_DEBUG
    LOGI("JNI_OnLoad:finshed:result=%d", result);
#endif

    return JNI_VERSION_1_6;
}
