#ifndef ONLOAD_H_
#define ONLOAD_H_

static const char *const PACKAGE_NAME = "com/badpx/indexbitmap/graphics/BitmapHelper";
#pragma interface

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM *vm, void *reserved);

#ifdef __cplusplus
}
#endif

#endif /* ONLOAD_H_ */
