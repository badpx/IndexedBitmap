
#ifndef __SKBITMAP_HELPER_H__
#define __SKBITMAP_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

bool setupLibrary(JNIEnv* env);

//jint JNICALL nativeReconfigure(JNIEnv* env, jobject, jobject javaBitmap, jint width, jint height);

jboolean JNICALL Init(JNIEnv* env, jobject, jobject index8bitmap, jintArray colorTable);
jint JNICALL GetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap);
jint JNICALL GetPalette(JNIEnv* env, jobject, jobject javaBitmap, jintArray output);
jint JNICALL ChangePalette(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette);
jint JNICALL GetConfig(JNIEnv* env, jobject, jobject javaBitmap);
jint JNICALL SetConfig(JNIEnv* env, jobject, jobject javaBitmap, jint config);
jint JNICALL GetIndex8Config (JNIEnv* env);
jboolean JNICALL Index8FakeToAlpha8(JNIEnv* env, jobject, jobject javaBitmap, jboolean fake);

#ifdef __cplusplus
}

#endif
#endif
