
#include <cassert>
#include <cstdio>
#include <cstring>
#include <android/log.h>
#include "SkBitmapOperator.h"
#include "baseutils.h"
#include "color_table.h"

#define BITMAP_CLASS_NAME "android/graphics/Bitmap"

SkBitmapOperator::SkBitmapOperator(int* skBitmapFieldOffset, size_t skFieldNum,
    JavaFieldInfo* bitmapFields, size_t fieldNum,
    JavaMethodInfo* bitmapMethods, size_t methodNum) :
    mSkBitmapFieldOffset(NULL), mBitmapFieldInfo(NULL), mBitmapMethodInfo(NULL),
    mIndex8ConfigRealValue(0), mIsAllFieldsLocated(false)
{
    if (NULL != skBitmapFieldOffset && skFieldNum == SK_FIELDS_NUM) {
        mSkBitmapFieldOffset = new int[SK_FIELDS_NUM];
        memcpy(mSkBitmapFieldOffset, skBitmapFieldOffset, skFieldNum * sizeof(int));
    }

    if (NULL != bitmapFields && fieldNum == FIELDS_NUM) {
        mBitmapFieldInfo = new JavaFieldInfo[FIELDS_NUM];
        memcpy(mBitmapFieldInfo, bitmapFields, fieldNum * sizeof(JavaFieldInfo));
    }

    if (NULL != bitmapMethods && methodNum == METHODS_NUM) {
        mBitmapMethodInfo = new JavaMethodInfo[METHODS_NUM];
        memcpy(mBitmapMethodInfo, bitmapMethods, methodNum * sizeof(JavaMethodInfo));
    }
}

SkBitmapOperator::~SkBitmapOperator() {
    delete[] mSkBitmapFieldOffset;
    delete[] mBitmapFieldInfo;
    delete[] mBitmapMethodInfo;
}

bool SkBitmapOperator::setup(JNIEnv* env) {
    static int sInitFlag;

    if (0 == sInitFlag) {
        int apiLevel = getApiLevel(env);

        LOGD("NativeBitmapHelper initialize started");
        sInitFlag = -1;

        jclass bitmap_class = env->FindClass(BITMAP_CLASS_NAME);
        if (NULL == bitmap_class) {
            LOGE("Can't find java Bitmap class!");
            return false;
        }

        LOGD("Start get fields for Bitmap by reflection");
        if (NULL != mBitmapFieldInfo) {
            for (int i = 0; i < FIELDS_NUM; ++i) {
                JavaFieldInfo* fieldInfo = mBitmapFieldInfo + i;
                if (NULL != fieldInfo && NULL != fieldInfo->name && NULL != fieldInfo->signature) {
                    LOGD("Field name = %s, signature=%s", fieldInfo->name, fieldInfo->signature);
                    jclass clazz = bitmap_class;
                    if (NULL != fieldInfo->innerClass) {
                        char className[64];
                        // TODO: change to std::string.append()
                        sprintf(className, "%s$%s", BITMAP_CLASS_NAME, fieldInfo->innerClass);
                        clazz = env->FindClass(className);
                        LOGD("Find %s class=%p", className, clazz);
                    }
                    if (NULL != clazz) {
                        fieldInfo->fieldID = env->GetFieldID(clazz, fieldInfo->name, fieldInfo->signature);
                    }
                    if (NULL == fieldInfo->fieldID) {
                        LOGE("Can't find such field in class(%p)!", clazz);
                        return false;
                    } else {
                        LOGD("Field was found in class(%p)", clazz);
                    }
                }
            }
        }

        LOGD("Start get methods for Bitmap by reflection");
        if (NULL != mBitmapMethodInfo) {
            for (int i = 0; i < METHODS_NUM; ++i) {
                JavaMethodInfo* methodInfo = mBitmapMethodInfo + i;
                if (NULL != methodInfo && NULL != methodInfo->name && NULL != methodInfo->signature) {
                    LOGD("Method name = %s, signature=%s", methodInfo->name, methodInfo->signature);
                    methodInfo->methodID = env->GetMethodID(bitmap_class, methodInfo->name, methodInfo->signature);
                    if (NULL == methodInfo->methodID) return false;
                }
            }
        }

        sInitFlag = 1;  // Initialized success
        LOGD("NativeBitmapHelper initialize finished");
    } else if (-1 == sInitFlag) {
        // Initialized failed, may be can't take some java fields or methods by reflection.
        LOGD("NativeBitmapHelper initialize failed!");
        return false;
    }
    return true;
}

bool SkBitmapOperator::detectMemoryLayout(JNIEnv* env, jobject index8Bitmap, jintArray palette) {
    // TODO: find all fields when travel failed
    bool allFieldsLocated = travelForNativeFields(env, index8Bitmap, palette);
    if (allFieldsLocated) setAllFieldsLocated();
    return allFieldsLocated;
}

bool SkBitmapOperator::travelForNativeFields(JNIEnv* env, jobject index8Bitmap, jintArray palette) {
    if (NULL != mSkBitmapFieldOffset) {
        char* bitmap = (char*)getNativeBitmap(env, index8Bitmap);
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, index8Bitmap, &bmpInfo);

        if (NULL != bitmap && 0 == bmpInfo.format/* is index8 */) {
            int widthOffset = mSkBitmapFieldOffset[SK_WIDTH];
            int heightOffset = mSkBitmapFieldOffset[SK_HEIGHT];
            int rowBytesOffset = mSkBitmapFieldOffset[SK_ROW_BYTES];
            int configOffset = mSkBitmapFieldOffset[SK_CONFIG];
            int colorTableOffset = mSkBitmapFieldOffset[SK_COLOR_TABLE];
            int colorTypeOffset = mSkBitmapFieldOffset[SK_COLOR_TYPE];
            int alphaTypeOffset = mSkBitmapFieldOffset[SK_ALPHA_TYPE];
            if (INVALID_OFFSET == widthOffset || INVALID_OFFSET == heightOffset
                || INVALID_OFFSET == rowBytesOffset || INVALID_OFFSET == colorTableOffset
                || (INVALID_OFFSET == configOffset
                    && (INVALID_OFFSET == colorTypeOffset || INVALID_OFFSET == alphaTypeOffset)) ) {
                LOGE("necessary information missing!");
                return false;
            }
        }

        LOGD("Start traversal for native fields...");
        for (int i = 0; i < TRAVERSAL_TIMES; ++i, ++bitmap) {
            //TODO: just for ARM?
            if (0 != ((int)bitmap & 3)) {
                // Address of bitmap not align to 4
                continue;
            }
            LOGD("Traversal times = %d in %p", i, *((int*)bitmap));

            do {
                if (!locateSize(bitmap, bmpInfo.width, bmpInfo.height)) {
                    break;
                }
                LOGD("fWidth/fHeight located");

                if (!locateRowBytes(bitmap, bmpInfo.stride)) {
                    break;
                }
                LOGD("fRowBytes located");

                uint8_t config = locateConfig(bitmap);
                if (locateColorType(bitmap)) {
                    LOGD("fColorType located");
                } else if (config > 0) {
                    setIndex8ConfigValue(config);
                    LOGD("fConfig located");
                } else {
                    LOGW("Neither fConfig nor fColorType can located!");
                    break;
                }

                if (!locateColorTable(env, bitmap, palette)) {
                    break;
                }
                LOGD("fColorTable located");

                if (locateAlphaType(bitmap)) {
                    LOGD("fAlphaType located");
                }

                LOGD("Traversal for field base success(base=%d)", i);
                for (int n = 0; n < SK_FIELDS_NUM; ++n) {
                    int offset = mSkBitmapFieldOffset[n];
                    if (INVALID_OFFSET != offset) {
                        // Adjust relative offset to absolute value.
                        mSkBitmapFieldOffset[n] = offset + i;
                    }
                }

                return true;
            } while (false);
        }
    }
    LOGD("traversal for fields failed!");
    return false;
}


void* SkBitmapOperator::getNativeBitmap(JNIEnv* env, jobject javaBitmap) const {
    if (NULL != javaBitmap && NULL != mBitmapFieldInfo[NATIVE_BITMAP].fieldID) {
        return (void*)env->GetLongField(javaBitmap, mBitmapFieldInfo[NATIVE_BITMAP].fieldID);
    }
    return NULL;
}

bool SkBitmapOperator::locateColorTable(JNIEnv* env, char* bitmap, jintArray expectPalette) const {
    bool located = false;
    int colorTableOffset = mSkBitmapFieldOffset[SK_COLOR_TABLE];
    if (INVALID_OFFSET != colorTableOffset) {
        // Compare for locate native color table
        ColorTable* colorTable = *(ColorTable**)(bitmap + colorTableOffset);
        if (NULL != colorTable) {
            assert(NULL != expectPalette);
            int count = env->GetArrayLength(expectPalette);
            int* array = env->GetIntArrayElements(expectPalette, NULL);
            LOGD("Native colorTable count = %d. Expect palette count = %d", colorTable->fCount, count);

            if (colorTable->fCount == count && memcmp(array, colorTable->fColors, count * sizeof(PMColor)) == 0) {
                located = true;
                LOGD("Color table is located");
            } else {
                LOGW("Color table verify failed!");
            }
            env->ReleaseIntArrayElements(expectPalette, array, 0);
        }
    }
    return located;
}

int SkBitmapOperator::locateConfig(char* bitmap) const {
    int configOffset = mSkBitmapFieldOffset[SK_CONFIG];
    if (INVALID_OFFSET != configOffset) {
        uint8_t config = *((uint8_t*)(bitmap + configOffset));
        if (INDEX8_CONFIG_FOR_4_4_3 == config
            || INDEX8_CONFIG == config) {
            LOGD("kIndex8Config=%d", config);
            return config;
        }
        LOGW("Unexpected config value!");
    }
    return 0;
}

bool SkBitmapOperator::locateColorType(char* bitmap) const {
    int colorTypeOffset = mSkBitmapFieldOffset[SK_COLOR_TYPE];
    if (INVALID_OFFSET != colorTypeOffset) {
        int colorType = *((int*)(bitmap + colorTypeOffset));
        return INDEX_8_SK_COLOR_TYPE == colorType;
    }
    return false;
}

bool SkBitmapOperator::locateAlphaType(char* bitmap) const {
    int alphaTypeOffset = mSkBitmapFieldOffset[SK_ALPHA_TYPE];
    if (INVALID_OFFSET != alphaTypeOffset) {
        int alphaType = *((uint8_t*)(bitmap + alphaTypeOffset));
        if (alphaType >= kIgnore_SkAlphaType
            && alphaType <= kLastEnum_SkAlphaType) {
            return true;
        }
    }
    return false;
}

void* SkBitmapOperator::getColorTable(JNIEnv* env, jobject javaBitmap) const {
    int colorTableOffset = mSkBitmapFieldOffset[SK_COLOR_TABLE];
    if (isAllFieldsLocated() && INVALID_OFFSET != colorTableOffset) {
        char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
        if (NULL != bitmap) {
            ColorTable* colorTable =
                *(ColorTable**)(bitmap + colorTableOffset);
            return colorTable;
        }
    }
    return NULL;
}

PMColor* SkBitmapOperator::getPalette(JNIEnv* env, jobject javaBitmap, uint8_t* count) const {
    ColorTable* colorTable = (ColorTable*)getColorTable(env, javaBitmap);
    if (NULL != colorTable) {
        if (NULL != count) {
            *count = colorTable->fCount;
        }
        return colorTable->fColors;
    }

    // The bitmap hasn't color table
    if (NULL != count) {
        *count = 0;
    }
    return NULL;
}

int SkBitmapOperator::setPalette(JNIEnv* env, jobject javaBitmap, PMColor* palette, uint8_t count) {
    if (isAllFieldsLocated() && NULL != palette && count > 0) {
        uint8_t colorCount;
        PMColor* colors = getPalette(env, javaBitmap, &colorCount);
        if (NULL != colors && colorCount > 0) {
            count = count < colorCount ? count : colorCount;
            memset(colors, 0, colorCount * sizeof(PMColor));
            if (getAlphaType(env, javaBitmap) == kPremul_SkAlphaType) {
                LOGD("Bitmap(%p) need premultiply...", javaBitmap);
                for (int i = 0; i < count; ++i) {
                    int color = palette[i];
                    int a = SkColorGetA(color);
                    int r = SkColorGetR(color);
                    int g = SkColorGetG(color);
                    int b = SkColorGetB(color);

                    float alphaFactor = a / 255.0f;
                    colors[i] = PackABGR32(a,
                            int(b * alphaFactor),
                            int(g * alphaFactor),
                            int(r * alphaFactor));
                }
            } else {
                memcpy(colors, palette, count * sizeof(PMColor));
            }

            return count; // real access color entity count
        }
    }
    return 0;
}

uint8_t SkBitmapOperator::getConfig(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int configOffset = mSkBitmapFieldOffset[SK_CONFIG];
        if (INVALID_OFFSET != configOffset) {
            uint8_t* bitmap = (uint8_t*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *(bitmap + configOffset);
            }
        }
    }
    return 0;
}

uint8_t SkBitmapOperator::setConfig(JNIEnv* env, jobject javaBitmap, uint8_t config) {
    if (isAllFieldsLocated()) {
        int configOffset = mSkBitmapFieldOffset[SK_CONFIG];
        if (INVALID_OFFSET != configOffset) {
            uint8_t* bitmap = (uint8_t*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint8_t oldConfig = *(bitmap + configOffset);
                *(bitmap + configOffset) = config;
                return oldConfig;
            }
        }
    }
    return -1;
}

uint32_t SkBitmapOperator::getRowBytes(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int rowBytesOffset = mSkBitmapFieldOffset[SK_ROW_BYTES];
        if (INVALID_OFFSET != rowBytesOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *((uint32_t*)(bitmap + rowBytesOffset));
            }
        }
    }
    return 0;
}

uint32_t SkBitmapOperator::setRowBytes(JNIEnv* env, jobject javaBitmap, uint32_t rowBytes) {
    if (isAllFieldsLocated()) {
        int rowBytesOffset = mSkBitmapFieldOffset[SK_ROW_BYTES];
        if (INVALID_OFFSET != rowBytesOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint32_t oldRowBytes = *((uint32_t*)(bitmap + rowBytesOffset));
                *((uint32_t*)(bitmap + rowBytesOffset)) = rowBytes;
                return oldRowBytes;
            }
        }
    }
    return -1;
}

uint32_t SkBitmapOperator::getWidth(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int widthOffset = mSkBitmapFieldOffset[SK_WIDTH];
        if (INVALID_OFFSET != widthOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *((uint32_t*)(bitmap + widthOffset));
            }
        }
    }
    return 0;
}

uint32_t SkBitmapOperator::setWidth(JNIEnv* env, jobject javaBitmap, uint32_t width) {
    if (isAllFieldsLocated()) {
        int widthOffset = mSkBitmapFieldOffset[SK_WIDTH];
        if (INVALID_OFFSET != widthOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint32_t oldWidth = *((uint32_t*)(bitmap + widthOffset));
                *((uint32_t*)(bitmap + widthOffset)) = width;
                return oldWidth;
            }
        }
    }
    return -1;
}

uint32_t SkBitmapOperator::getHeight(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int heightOffset = mSkBitmapFieldOffset[SK_WIDTH];
        if (INVALID_OFFSET != heightOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *((uint32_t*)(bitmap + heightOffset));
            }
        }
    }
    return 0;
}

uint32_t SkBitmapOperator::setHeight(JNIEnv* env, jobject javaBitmap, uint32_t height) {
    if (isAllFieldsLocated()) {
        int heightOffset = mSkBitmapFieldOffset[SK_HEIGHT];
        if (INVALID_OFFSET != heightOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint32_t oldHeight = *((uint32_t*)(bitmap + heightOffset));
                *((uint32_t*)(bitmap + heightOffset)) = height;
                return oldHeight;
            }
        }
    }
    return -1;
}

int SkBitmapOperator::getAlphaType(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int alphaTypeOffset = mSkBitmapFieldOffset[SK_ALPHA_TYPE];
        if (INVALID_OFFSET != alphaTypeOffset) {
            uint8_t* bitmap = (uint8_t*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *(bitmap + alphaTypeOffset);
            }
        }
    }
    return 0;
}

int SkBitmapOperator::setAlphatype(JNIEnv* env, jobject javaBitmap, int alphaType) {
    if (isAllFieldsLocated()) {
        int alphaTypeOffset = mSkBitmapFieldOffset[SK_ALPHA_TYPE];
        if (INVALID_OFFSET != alphaTypeOffset) {
            uint8_t* bitmap = (uint8_t*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint8_t oldAlphaType = *(bitmap + alphaTypeOffset);
                *(bitmap + alphaTypeOffset) = alphaType;
                return oldAlphaType;
            }
        }
    }
    return -1;
}

int SkBitmapOperator::getColorType(JNIEnv* env, jobject javaBitmap) const {
    if (isAllFieldsLocated()) {
        int colorTypeOffset = mSkBitmapFieldOffset[SK_COLOR_TYPE];
        if (INVALID_OFFSET != colorTypeOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                return *((uint32_t*)(bitmap + colorTypeOffset));
            }
        }
    }
    return 0;
}

int SkBitmapOperator::setColorType(JNIEnv* env, jobject javaBitmap, int colorType) {
    if (isAllFieldsLocated()) {
        int colorTypeOffset = mSkBitmapFieldOffset[SK_COLOR_TYPE];
        if (INVALID_OFFSET != colorTypeOffset) {
            char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
            if (NULL != bitmap) {
                uint32_t oldColorType = *((uint32_t*)(bitmap + colorTypeOffset));
                *((uint32_t*)(bitmap + colorTypeOffset)) = colorType;
                return oldColorType;
            }
        }
    }
    return -1;
}
