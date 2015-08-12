#include <cstdio>
#include <cstring>
#include <android/bitmap.h>
#include "baseutils.h"
#include "skbitmap_helper.h"
#include "color_table.h"

#define TRAVERSAL_TIMES     (16)

jfieldID gBitmap_nativeBitmapFieldID;
jfieldID gBitmap_widthFieldID;
jfieldID gBitmap_heightFieldID;
jmethodID  gBitmap_isMutableMethodID;

static int gBmpInfoFieldsBase;

int getApiLevel(JNIEnv* env);
int computeBytesPerPixel(uint32_t config);
int locateColorTable(JNIEnv* env, jobject javaBitmap, jintArray colorTable = NULL);
ColorTable* getColorTable(JNIEnv* env, jobject javaBitmap);
int locateBitmapInfoFieldsBaseBelowAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo);
int locateBitmapInfoFieldsBaseAboveAPI20(void* bitmap, const AndroidBitmapInfo& bmpInfo);
int locateColorTableRelateInfoFieldsBase(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, int offset, jintArray palette);
int locateColorTableBelowAPI18(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);
int locateColorTableBelowAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);
int locateColorTableAboveAPI21(JNIEnv* env, void* bitmap, const AndroidBitmapInfo& bmpInfo, jintArray colorTable);

bool setupLibrary(JNIEnv* env) {
    static int sInitFlag;

    if (0 == sInitFlag) {
        int apiLevel = getApiLevel(env);

        LOGD("NativeBitmapHelper initialize started");
        sInitFlag = -1;

        jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
        if (NULL == bitmap_class) return false;
        if (apiLevel < 21) {
            gBitmap_nativeBitmapFieldID = env->GetFieldID(bitmap_class, "mNativeBitmap", "I");
        } else {
            gBitmap_nativeBitmapFieldID = env->GetFieldID(bitmap_class, "mNativeBitmap", "J");
        }
        if (NULL == gBitmap_nativeBitmapFieldID) return false;
        gBitmap_widthFieldID = env->GetFieldID(bitmap_class, "mWidth", "I");
        if (NULL == gBitmap_widthFieldID) return false;
        gBitmap_heightFieldID = env->GetFieldID(bitmap_class, "mHeight", "I");
        if (NULL == gBitmap_heightFieldID) return false;
        gBitmap_isMutableMethodID = env->GetMethodID(bitmap_class, "isMutable", "()Z");
        if (NULL == gBitmap_isMutableMethodID) return false;

        sInitFlag = 1;  // Initialize success
        LOGD("NativeBitmapHelper initialize finished");
    } else if (-1 == sInitFlag) {
        // Initialize failed, may be can't take some java fields or methods by reflection.
        LOGD("NativeBitmapHelper initialize failed!");
        return false;
    }
    return true;
}

jint JNICALL nativeGetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);
        return computeBytesPerPixel(bmpInfo.format);
    }
    return 0;
}

jint JNICALL nativeLocateColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray colorTable) {
    return locateColorTable(env, javaBitmap, colorTable);
}

jint JNICALL nativeGetColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray output) {
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

jint JNICALL nativeChangeColorTable(JNIEnv* env, jobject, jobject javaBitmap, jintArray palette) {
    if (NULL != palette) {
        ColorTable* colorTable = getColorTable(env, javaBitmap);
        if (NULL != colorTable) {
            int count = env->GetArrayLength(palette);
            if (colorTable->fCount >= count) {
                int* array = env->GetIntArrayElements(palette, NULL);
                memcpy(colorTable->fColors, array, count * sizeof(PMColor));
                env->ReleaseIntArrayElements(palette, array, JNI_ABORT);
                return count;
            } else {
                return 0;
            }
        }
    }

    return -1;
}

int getApiLevel(JNIEnv* env) {
    static int sApiLevel = 0;

    while (NULL != env && 0 == sApiLevel) {
        sApiLevel = -1;

        jclass versionClass = env->FindClass("android/os/Build$VERSION");
        if (NULL == versionClass) {
            LOGD("Can't find Build.VERSION");
            break;
        }

        jfieldID sdkIntFieldID = env->GetStaticFieldID(versionClass, "SDK_INT", "I");
        if (NULL == sdkIntFieldID) {
            LOGD("Can't find Build.VERSION.SDK_INT");
            break;
        }

        sApiLevel = env->GetStaticIntField(versionClass, sdkIntFieldID);
        LOGD("SDK_INT = %d", sApiLevel);
        break;
    }

    return sApiLevel;
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

    if (NULL != javaBitmap && NULL != colorTable && 0 == sColorTableOffset) {
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
