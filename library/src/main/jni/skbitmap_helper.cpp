#include <cstdio>
#include <cstring>
#include <android/bitmap.h>
#include "baseutils.h"
#include "SkTypeDef.h"
#include "skbitmap_helper.h"
#include "color_table.h"
#include "SkBitmapOperator.h"
#include "SkBitmapOperatorFactory.h"

SkBitmapOperator* gSkBitmapOperator;

int computeBytesPerPixel(uint32_t config);

bool setupLibrary(JNIEnv* env) {
    LOGI("sizeof(void*)=%d", sizeof(void*));
    gSkBitmapOperator = createSkBitmapOperator(env);
    if (NULL != gSkBitmapOperator) {
        return gSkBitmapOperator->setup(env);
    }
    return false;
}

jboolean JNICALL Init(JNIEnv* env, jobject, jobject index8bitmap, jintArray colorTable) {
    if (NULL != index8bitmap) {
//        gIndex8ConfigValue = GetConfig(env, NULL, index8bitmap);
//        int location = locateColorTable(env, index8bitmap);
        return gSkBitmapOperator->detectMemoryLayout(env, index8bitmap, colorTable);
    }
    return JNI_FALSE;
}

jint JNICALL GetIndex8Config (JNIEnv* env) {
    return (jint)gSkBitmapOperator->getIndex8ConfigValue();
}

jint JNICALL GetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);
        return computeBytesPerPixel(bmpInfo.format);
    }
    return 0;
}

jint JNICALL GetPalette(JNIEnv* env, jobject, jobject javaBitmap, jintArray output) {
    if (NULL != output) {
        uint8_t colorCount = 0;
        PMColor* colorTable = gSkBitmapOperator->getPalette(env, javaBitmap, &colorCount);
        if (NULL != colorTable) {
            int count = env->GetArrayLength(output);
            int* array = env->GetIntArrayElements(output, NULL);

            memset(array, 0, count * sizeof(jint));

            count = count < colorCount ? count : colorCount;
            memcpy(array, colorTable, count * sizeof(PMColor));

            env->ReleaseIntArrayElements(output, array, 0);
            return colorCount;
        }
    }
    return 0;
}

jint JNICALL ChangePalette(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette) {
    if (NULL != javaBitmap && NULL != palette) {
        int count = env->GetArrayLength(palette);
        int* array = env->GetIntArrayElements(palette, NULL);
        count = gSkBitmapOperator->setPalette(env, javaBitmap, (PMColor*)array, count);
        env->ReleaseIntArrayElements(palette, array, JNI_ABORT);

        return count;
    }

    return 0;
}

jboolean JNICALL Index8FakeToAlpha8(JNIEnv* env, jobject, jobject javaBitmap, jboolean fake) {
    if (!gSkBitmapOperator->hasColorTable(env, javaBitmap)) {
        LOGW("Bitmap(%p) hasn't color table!", javaBitmap);
        return JNI_FALSE;
    }

    if (getApiLevel(env) < 20) {
        const uint8_t index8Config = gSkBitmapOperator->getIndex8ConfigValue();
        if (index8Config > 0) {
            const uint8_t alpha8Config = index8Config - 1;
            const int config = gSkBitmapOperator->getConfig(env, javaBitmap);
            if (fake) {
                // INDEX8 fake to ALPHA8
                if (index8Config == config) {
                    gSkBitmapOperator->setConfig(env, javaBitmap, index8Config - 1);
                    return true;
                } else {
                    LOGW("Bitmap(%p) is not INDEX8 color format!", javaBitmap);
                    return JNI_FALSE;
                }
            } else {
                // restore to INDEX8
                if (alpha8Config == config) {
                    gSkBitmapOperator->setConfig(env, javaBitmap, index8Config);
                } else {
                    LOGW("Bitmap(%p) is not a faked ALPHA8 format bitmap!", javaBitmap);
                    return JNI_FALSE;
                }
            }
        }
    } else if (getApiLevel(env) > 20) {
        int colorType = gSkBitmapOperator->getColorType(env, javaBitmap);
        if (fake) {
            // INDEX8 fake to ALPHA8
            if (kIndex_8_SkColorType == colorType) {
                gSkBitmapOperator->setColorType(env, javaBitmap, kAlpha_8_SkColorType);
            } else {
                LOGW("Bitmap(%p) is not INDEX8 color format!", javaBitmap);
                return JNI_FALSE;
            }
        } else {
            // restore to INDEX8
            if (kAlpha_8_SkColorType == colorType) {
                gSkBitmapOperator->setColorType(env, javaBitmap, kIndex_8_SkColorType);
            } else {
                LOGW("Bitmap(%p) is not a faked ALPHA8 format bitmap!", javaBitmap);
                return JNI_FALSE;
            }
        }
    }
    return JNI_FALSE;
}

jint JNICALL GetConfig(JNIEnv* env, jobject, jobject javaBitmap) {
    return gSkBitmapOperator->getConfig(env, javaBitmap);
}

jint JNICALL SetConfig(JNIEnv* env, jobject, jobject javaBitmap, jint config) {
    return gSkBitmapOperator->setConfig(env, javaBitmap, config);
}

int computeBytesPerPixel(uint32_t config) {
    int bpp;
    switch (config) {
        case ANDROID_BITMAP_FORMAT_NONE:
            bpp = 0;   // not applicable
            break;
        case ANDROID_BITMAP_FORMAT_A_8:
            bpp = 1;
            break;
        case ANDROID_BITMAP_FORMAT_RGB_565:
        case ANDROID_BITMAP_FORMAT_RGBA_4444:
            bpp = 2;
            break;
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
            bpp = 4;
            break;
        default:
            bpp = 0;   // error
            break;
    }
    return bpp;
}

