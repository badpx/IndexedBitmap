
#ifndef __SKBITMAP_HELPER_H__
#define __SKBITMAP_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

//jint JNICALL nativeReconfigure(JNIEnv* env, jobject, jobject javaBitmap, jint width, jint height);

jint JNICALL nativeGetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap);

#ifdef __cplusplus
}

#endif
#endif
