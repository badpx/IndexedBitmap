
#include <jni.h>
#include "SkBitmapOperatorFactory.h"
#include "SkBitmapOperator.h"
#include "baseutils.h"

SkBitmapOperator* createSkBitmapOperator(JNIEnv* env) {
    int apiLevel = getApiLevel(env);
    if (apiLevel == 21) {
        int skBitmapFieldOffset[] = {
            /*SK_WIDTH*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/,
            /*SK_HEIGHT*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int)/*fWidth*/,
            /*SK_ROW_BYTES*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 4/*fInfo*/,
            /*SK_CONFIG*/ SkBitmapOperator::INVALID_OFFSET,
            /*SK_COLOR_TABLE*/0,
            /*SK_COLOR_TYPE*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 2/*fWidth,fHeight*/,
            /*SK_ALPHA_TYPE*/ sizeof(void*)/*fColorTable* */ + sizeof(int32_t) * 2/*fPixelRefOrigin*/ + sizeof(int) * 3/*fWidth,fHeight,fColorType*/,
        };

        JavaFieldInfo bitmapFiledInfo[] = {
            {"mNativeBitmap", "J", NULL},
        };

        JavaMethodInfo bitmapMethodInfo[] = {
            {"isMutable", "()Z", NULL},
        };

        return new SkBitmapOperator(
            skBitmapFieldOffset, NUM_ARRAY_ELEMENTS(skBitmapFieldOffset),
            bitmapFiledInfo, NUM_ARRAY_ELEMENTS(bitmapFiledInfo),
            bitmapMethodInfo, NUM_ARRAY_ELEMENTS(bitmapMethodInfo)
        );
    } else if (apiLevel == 19){
    }
    return NULL;
}
