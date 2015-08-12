
#ifndef __SKBITMAP_HELPER_H__
#define __SKBITMAP_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

bool setupLibrary(JNIEnv* env);

//jint JNICALL nativeReconfigure(JNIEnv* env, jobject, jobject javaBitmap, jint width, jint height);

jint JNICALL nativeGetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap);
jint JNICALL nativeLocateColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette);
jint JNICALL nativeGetColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray output);
jint JNICALL nativeChangeColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette);


#ifdef __cplusplus
}

#endif
#endif
