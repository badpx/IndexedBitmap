#include <cstdio>
#include <cstring>
#include <android/bitmap.h>
#include "baseutils.h"
#include "skbitmap_helper.h"
#include "color_table.h"
#include "SkBitmapOperator.h"
#include "SkBitmapOperatorFactory.h"

#define TRAVERSAL_TIMES     16

jfieldID gBitmap_nativeBitmapFieldID;
jfieldID gBitmap_widthFieldID;
jfieldID gBitmap_heightFieldID;
jmethodID  gBitmap_isMutableMethodID;

static uint32_t gBmpInfoFieldsBase;
static uint8_t gIndex8ConfigValue;

int computeBytesPerPixel(uint32_t config);

int locateColorTable(JNIEnv* env, jobject javaBitmap, jintArray colorTable = NULL);
int locateColorTableRelateInfoFieldsBase(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, int offset, jintArray palette);
int locateColorTableBelowAPI18(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);
int locateColorTableBelowAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);
int locateColorTableAboveAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);

ColorTable* getColorTable(JNIEnv* env, jobject javaBitmap);

int locateBitmapInfoFieldsBaseBelowAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo);
int locateBitmapInfoFieldsBaseAboveAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo);
SkBitmapOperator* gSkBitmapOperator;

bool setupLibrary(JNIEnv* env) {
    gSkBitmapOperator = createSkBitmapOperator(env);
    if (NULL != gSkBitmapOperator) {
        return gSkBitmapOperator->setup(env);
    }
    return false;
}

jboolean JNICALL Init(JNIEnv* env, jobject, jobject index8bitmap) {
    if (NULL != index8bitmap) {
//        gIndex8ConfigValue = GetConfig(env, NULL, index8bitmap);
//        int location = locateColorTable(env, index8bitmap);
        return gSkBitmapOperator->travelForNativeFields(env, index8bitmap);
    }
    return false;
}

jint JNICALL GetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);
        return computeBytesPerPixel(bmpInfo.format);
    }
    return 0;
}

jint JNICALL GetColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray output) {
    if (NULL != output) {
        ColorTable* colorTable = getColorTable(env, javaBitmap);
        if (NULL != colorTable) {
            int count = env->GetArrayLength(output);
            if (count >= colorTable->fCount) {
                int* array = env->GetIntArrayElements(output, NULL);

                memset(array, 0, count * sizeof(jint));
                memcpy(array, colorTable->fColors, colorTable->fCount * sizeof(PMColor));

                env->ReleaseIntArrayElements(output, array, 0);
                return colorTable->fCount;
            } else {
                return 0;
            }
        }
    }
    return -1;
}

jint JNICALL ChangeColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette) {
    if (NULL != palette) {
        ColorTable* colorTable = getColorTable(env, javaBitmap);
        if (NULL != colorTable) {
            int count = env->GetArrayLength(palette);
            if (colorTable->fCount >= count) {
                int* array = env->GetIntArrayElements(palette, NULL);

                for (int i = 0; i < count; ++i) {
                    int color = array[i];
                    int a = SkColorGetA(color);
                    int r = SkColorGetR(color);
                    int g = SkColorGetG(color);
                    int b = SkColorGetB(color);

                    float alphaFactor = a / 255.0f;
                    colorTable->fColors[i] = PackABGR32(a, 
                            int(b * alphaFactor), 
                            int(g * alphaFactor),
                            int(r * alphaFactor));
                }

                env->ReleaseIntArrayElements(palette, array, JNI_ABORT);
                return count;
            } else {
                return 0;
            }
        }
    }

    return -1;
}

jint JNICALL Index8FakeToAlpha8(JNIEnv* env, jobject, jobject javaBitmap, jboolean fake) {
    if (NULL != javaBitmap) {
        if (NULL != getColorTable(env, javaBitmap)) {
            int infoFieldsBase = 0;
            int32_t* bitmap = (int32_t*)env->GetIntField(javaBitmap, gBitmap_nativeBitmapFieldID);
            AndroidBitmapInfo bmpInfo;
            AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);

            if (getApiLevel(env) < 21) {
                infoFieldsBase = locateBitmapInfoFieldsBaseBelowAPI20(bitmap, bmpInfo);
            } else {
                infoFieldsBase = locateBitmapInfoFieldsBaseAboveAPI20(bitmap, bmpInfo);
            }
            if (infoFieldsBase > 2) {
                uint8_t* pConfig = (uint8_t*)(bitmap + infoFieldsBase + 3);
                int oldConfig = *pConfig;
                int config = fake ? gIndex8ConfigValue - 1: gIndex8ConfigValue;
                *pConfig = config;

                LOGD("Set bitmap config from %d to %d", oldConfig, config);
                return 0;
            }
        }
    }
    return -1;
}

jint JNICALL GetConfig(JNIEnv* env, jobject, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        int infoFieldsBase = 0;
        int32_t* bitmap = (int32_t*)env->GetIntField(javaBitmap, gBitmap_nativeBitmapFieldID);
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);

        if (getApiLevel(env) < 21) {
            infoFieldsBase = locateBitmapInfoFieldsBaseBelowAPI20(bitmap, bmpInfo);
        } else {
            infoFieldsBase = locateBitmapInfoFieldsBaseAboveAPI20(bitmap, bmpInfo);
        }
        if (infoFieldsBase > 2) {
            uint8_t* pConfig = (uint8_t*)(bitmap + infoFieldsBase + 3);
            return (jint)*pConfig;
        }
    }
    return 0;
}

jint JNICALL SetConfig(JNIEnv* env, jobject, jobject javaBitmap, jint config) {
    if (NULL != javaBitmap) {
        int infoFieldsBase = 0;
        int32_t* bitmap = (int32_t*)env->GetIntField(javaBitmap, gBitmap_nativeBitmapFieldID);
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);

        if (getApiLevel(env) < 21) {
            infoFieldsBase = locateBitmapInfoFieldsBaseBelowAPI20(bitmap, bmpInfo);
        } else {
            infoFieldsBase = locateBitmapInfoFieldsBaseAboveAPI20(bitmap, bmpInfo);
        }
        if (infoFieldsBase > 2) {
            uint8_t* pConfig = (uint8_t*)(bitmap + infoFieldsBase + 3);
            int oldConfig = *pConfig;
            *pConfig = config;
            LOGD("Set bitmap config from %d to %d", oldConfig, config);
            return oldConfig;
        }
    }
    return 0;
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

int locateColorTable(JNIEnv* env, jobject javaBitmap, jintArray colorTable) {
    static int sColorTableOffset = 0;

    if (NULL != javaBitmap && (NULL != colorTable || 0 == sColorTableOffset)/*TODO: fix it*/) {
        int bitmap = env->GetIntField(javaBitmap, gBitmap_nativeBitmapFieldID);
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);

        int apiLevel = getApiLevel(env);

        if (apiLevel < 18) {
            sColorTableOffset = locateColorTableBelowAPI18(env, (void*)bitmap, bmpInfo, colorTable);
        } else if (apiLevel < 21) {
            sColorTableOffset = locateColorTableBelowAPI21(env, (void*)bitmap, bmpInfo, colorTable);
        } else {
            sColorTableOffset = locateColorTableAboveAPI21(env, (void*)bitmap, bmpInfo, colorTable);
        }
    }

    return sColorTableOffset;
}

ColorTable* getColorTable(JNIEnv* env, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        int offset = locateColorTable(env, javaBitmap);
        if (offset > 0) {
            int32_t* bitmap = (int32_t*)env->GetIntField(javaBitmap, gBitmap_nativeBitmapFieldID);
            return (ColorTable*)bitmap[offset];
        }
    }
    return NULL;
}

int locateColorTableBelowAPI18(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray palette) {
    return locateColorTableRelateInfoFieldsBase(env, bitmap, bmpInfo, -2, palette);
}

int locateColorTableBelowAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray palette) {
    return locateColorTableRelateInfoFieldsBase(env, bitmap, bmpInfo, -1, palette);
}

int locateColorTableAboveAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray palette) {
    return locateColorTableRelateInfoFieldsBase(env, bitmap, bmpInfo, -3, palette);
}

int locateColorTableRelateInfoFieldsBase(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, int offset, jintArray palette) {
    int result = 0;
    int infoFieldsBase = 0;
    if (getApiLevel(env) < 21) {
        infoFieldsBase = locateBitmapInfoFieldsBaseBelowAPI20(bitmap, bmpInfo);
    } else {
        infoFieldsBase = locateBitmapInfoFieldsBaseAboveAPI20(bitmap, bmpInfo);
    }

    if (infoFieldsBase > 2) {
        uint32_t* ptr = (uint32_t*)bitmap;
        result = infoFieldsBase + offset;
        ColorTable* colorTable = (ColorTable*)ptr[result];
        if (NULL != colorTable) {
            if (NULL != palette) {
                int count = env->GetArrayLength(palette);
                int* array = env->GetIntArrayElements(palette, NULL);
                LOGD("result = %d, colorTable count = %d, palette count = %d", result, colorTable->fCount, count);
                if (colorTable->fCount == count && memcmp(array, colorTable->fColors, count * sizeof(PMColor)) == 0) {
                    env->ReleaseIntArrayElements(palette, array, 0);
                    LOGD("Locate color table offset = %d", result);
                    return result;
                }
                env->ReleaseIntArrayElements(palette, array, 0);
            } else if (NULL != colorTable->fColors && colorTable->fCount <= 256) {
                LOGD("Guess offset of color table = %d", result);
                return result;
            }
        }
    }
    LOGD("Color table offset locate failed!");
    return -1;
}


int locateBitmapInfoFieldsBaseBelowAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo) {
    uint32_t* ptr = (uint32_t*)bitmap;

    if (NULL != ptr && 0 == gBmpInfoFieldsBase) {
        for (int i = 0; i < TRAVERSAL_TIMES; ++i) {
            // Assuming the rowBytes/width/height of SkBitmap are continuous in memory model
            if (ptr[i] == bmpInfo.stride && ptr[i + 1] == bmpInfo.width && ptr[i + 2] == bmpInfo.height) {
                gBmpInfoFieldsBase = i;
                LOGD("traversal for field offset success(offset=%d)", i);
                return gBmpInfoFieldsBase;
            }
        }
        gBmpInfoFieldsBase = -1;
        LOGD("traversal for field offset failed!");
    }
    return gBmpInfoFieldsBase;
}

int locateBitmapInfoFieldsBaseAboveAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo) {
    uint32_t* ptr = (uint32_t*)bitmap;

    if (NULL != ptr && 0 == gBmpInfoFieldsBase) {
        for (int i = 0; i < TRAVERSAL_TIMES; ++i) {
            // Assuming the rowBytes/width/height of SkBitmap are continuous in memory model
            if (ptr[i] == bmpInfo.width && ptr[i + 1] == bmpInfo.height && ptr[i + 4] == bmpInfo.stride) {
                gBmpInfoFieldsBase = i;
                LOGD("traversal for field offset success(offset=%d)", i);
                return gBmpInfoFieldsBase;
            }
        }
        gBmpInfoFieldsBase = -1;
        LOGD("traversal for field offset failed!");
    }
    return gBmpInfoFieldsBase;
}
