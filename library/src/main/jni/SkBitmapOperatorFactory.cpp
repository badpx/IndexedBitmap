
#include <jni.h>
#include "baseutils.h"
#include "SkBitmapOperatorFactory.h"
#include "SkBitmapOperator.h"
#include "SkBitmapOperatorAPI23.h"

SkBitmapOperator* createSkBitmapOperator(JNIEnv* env) {
    int apiLevel = getApiLevel(env);
    if (apiLevel >= _M_) {
        int skBitmapFieldOffset[] = {
            /*SK_WIDTH:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/,
            /*SK_HEIGHT:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/ + sizeof(int)/*fWidth*/,
            /*SK_ROW_BYTES:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/ + sizeof(int) * 5/*fInfo*/ + sizeof(int&)/*mBitmap*/ + sizeof(void*)/*mStorage*/,
            /*SK_CONFIG:*/ SkBitmapOperator::INVALID_OFFSET,
            /*SK_COLOR_TABLE:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/ + sizeof(int) * 5/*fInfo*/ + sizeof(int&)/*mBitmap*/,
            /*SK_COLOR_TYPE:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/ + sizeof(int) * 2/*fWidth+fHeight*/,
            /*SK_ALPHA_TYPE:*/ sizeof(void*)/*vptr*/ + sizeof(int32_t)/*fRefCnt*/ + sizeof(void*)/*fMutex*/ + sizeof(int) * 3/*fWidth+fHeight+fColorType*/,
        };

        JavaFieldInfo bitmapFiledInfo[] = {
            {NULL, "mNativePtr", "J", NULL},
//            {NULL, "mFinalizer", "Landroid/graphics/Bitmap$BitmapFinalizer;", NULL},
        };

        JavaMethodInfo bitmapMethodInfo[] = {
            {NULL, "isMutable", "()Z", NULL},
        };

        return new SkBitmapOperatorAPI23(
            skBitmapFieldOffset, NUM_ARRAY_ELEMENTS(skBitmapFieldOffset),
            bitmapFiledInfo, NUM_ARRAY_ELEMENTS(bitmapFiledInfo),
            bitmapMethodInfo, NUM_ARRAY_ELEMENTS(bitmapMethodInfo)
        );
    } else if (_LOLLIPOP_ == apiLevel || _LOLLIPOP_MR1_ == apiLevel) {
        int skBitmapFieldOffset[] = {
            /*SK_WIDTH:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/,
            /*SK_HEIGHT:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int)/*fWidth*/,
            /*SK_ROW_BYTES:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 4/*fInfo*/,
            /*SK_CONFIG:*/ SkBitmapOperator::INVALID_OFFSET,
            /*SK_COLOR_TABLE:*/0,
            /*SK_COLOR_TYPE:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 2/*fWidth,fHeight*/,
            /*SK_ALPHA_TYPE:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 3/*fWidth,fHeight,fColorType*/,
        };

        JavaFieldInfo bitmapFiledInfo[] = {
            {NULL, "mNativeBitmap", "J", NULL},
        };

        JavaMethodInfo bitmapMethodInfo[] = {
            {NULL, "isMutable", "()Z", NULL},
        };

        return new SkBitmapOperator(
            skBitmapFieldOffset, NUM_ARRAY_ELEMENTS(skBitmapFieldOffset),
            bitmapFiledInfo, NUM_ARRAY_ELEMENTS(bitmapFiledInfo),
            bitmapMethodInfo, NUM_ARRAY_ELEMENTS(bitmapMethodInfo)
        );
    } else if (_KITKAT_ == apiLevel){
        int skBitmapFieldOffset[] = {
            /*SK_WIDTH:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t)/*fRowBytes*/,
            /*SK_HEIGHT:*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fRowBytes+fWidth*/,
            /*SK_ROW_BYTES:*/ sizeof(void*)/*fColorTable* */,
            /*SK_CONFIG:*/  sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 3/*fRowBytes+fWidth+fHeight*/,
            /*SK_COLOR_TABLE:*/0,
            /*SK_COLOR_TYPE:*/ SkBitmapOperator::INVALID_OFFSET,
            /*SK_ALPHA_TYPE:*/ SkBitmapOperator::INVALID_OFFSET,
        };

        JavaFieldInfo bitmapFiledInfo[] = {
            {NULL, "mNativeBitmap", "I", NULL},
        };

        JavaMethodInfo bitmapMethodInfo[] = {
            {NULL, "isMutable", "()Z", NULL},
        };

        return new SkBitmapOperator(
            skBitmapFieldOffset, NUM_ARRAY_ELEMENTS(skBitmapFieldOffset),
            bitmapFiledInfo, NUM_ARRAY_ELEMENTS(bitmapFiledInfo),
            bitmapMethodInfo, NUM_ARRAY_ELEMENTS(bitmapMethodInfo)
        );
    }
    return NULL;
}
