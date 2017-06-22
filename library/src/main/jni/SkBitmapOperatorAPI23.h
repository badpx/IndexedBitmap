
#ifndef __SK_BITMAP_OPERATOR_API23_H__
#define __SK_BITMAP_OPERATOR_API23_H__

#include "SkBitmapOperator.h"

class SkBitmapOperatorAPI23 : public SkBitmapOperator {
public:
    SkBitmapOperatorAPI23(int* skBitmapFieldOffset, size_t skFieldNum,
    JavaFieldInfo* bitmapFields, size_t fieldNum,
    JavaMethodInfo* bitmapMethods, size_t methodNum)
    : SkBitmapOperator(skBitmapFieldOffset, skFieldNum,
                        bitmapFields, fieldNum,
                        bitmapMethods, methodNum) {}
    virtual void* getNativeBitmap(JNIEnv* env, jobject javaBitmap) const;
};
#endif