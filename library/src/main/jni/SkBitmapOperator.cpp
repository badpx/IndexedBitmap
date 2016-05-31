
#include <cstring>
#include <android/log.h>
#include <android/bitmap.h>
#include "SkBitmapOperator.h"
#include "baseutils.h"

SkBitmapOperator::SkBitmapOperator(int* skBitmapFieldOffset, size_t skFieldNum,
    JavaFieldInfo* bitmapFields, size_t fieldNum,
    JavaMethodInfo* bitmapMethods, size_t methodNum) :
    mSkBitmapFieldBase(0),
    mSkBitmapFieldOffset(NULL), mBitmapFieldInfo(NULL),
    mBitmapMethodInfo(NULL)
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

        jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
        if (NULL == bitmap_class) {
            LOGE("Can't find java Bitmap class!");
            return false;
        }

        if (NULL != mBitmapFieldInfo) {
            for (int i = 0; i < FIELDS_NUM; ++i) {
                JavaFieldInfo* fieldInfo = mBitmapFieldInfo + i;
                if (NULL != fieldInfo && NULL != fieldInfo->name && NULL != fieldInfo->signature) {
                    LOGD("Field name = %s, signature=%s", fieldInfo->name, fieldInfo->signature);
                    fieldInfo->fieldID = env->GetFieldID(bitmap_class, fieldInfo->name, fieldInfo->signature);
                    if (NULL == fieldInfo->fieldID) return false;
                }
            }
        }

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

bool SkBitmapOperator::travelForNativeFields(JNIEnv* env, jobject javaBitmap) {
    if (NULL != javaBitmap && NULL != mSkBitmapFieldOffset) {
        char* bitmap = (char*)getNativeBitmap(env, javaBitmap);
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);

        int widthOffset = mSkBitmapFieldOffset[SK_WIDTH];
        int heightOffset = mSkBitmapFieldOffset[SK_HEIGHT];
        int rowBytesOffset = mSkBitmapFieldOffset[SK_ROW_BYTES];

        if (NULL != bitmap && 0 == mSkBitmapFieldBase) {
            for (int i = 0; i < TRAVERSAL_TIMES; ++i, ++bitmap) {
                if (0 != ((int)bitmap & 1)) {
                    LOGD("Address not align by 2, ignored...");
                    continue;
                }
                if ( *((int*)(bitmap + widthOffset)) == bmpInfo.width
                    && *((int*)(bitmap + heightOffset)) == bmpInfo.height
                    && *((uint32_t*)(bitmap + rowBytesOffset)) == bmpInfo.stride) {
                    mSkBitmapFieldBase = i;
                    LOGD("Traversal for field base success(base=%d)", i);
                    return true;
                }
            }
            mSkBitmapFieldBase = -1;
            LOGD("traversal for field offset failed!");
        }
    }
    return false;
}

void* SkBitmapOperator::getNativeBitmap(JNIEnv* env, jobject javaBitmap) const {
    if (NULL != javaBitmap) {
        return (void*)env->GetIntField(javaBitmap, mBitmapFieldInfo[NATIVE_BITMAP].fieldID);
    }
    return NULL;
}

int SkBitmapOperator::getConfig(void* bitmap) {
}
int SkBitmapOperator::setConfig(void* bitmap, int config) {
}
int SkBitmapOperator::getRowBytes(void* bitmap) {
}
int SkBitmapOperator::setRowBytes(void* bitmap, int rowBytes) {
}
int SkBitmapOperator::getWidth(void* bitmap) {
}
int SkBitmapOperator::setWidth(void* bitmap, int width) {
}
int SkBitmapOperator::getHeight(void* bitmap) {
}
int SkBitmapOperator::setHeight(void* bitmap, int height) {
}

PMColor* SkBitmapOperator::getColorTable(void* bitmap, size_t* count) {
}
void SkBitmapOperator::setColorTable(void* bitmap, PMColor* colors, size_t count) {
}

int SkBitmapOperator::locateColorTable(void* bitmap) {
}
int SkBitmapOperator::locateRowBytes(void* bitmap) {
}
int SkBitmapOperator::locateSize(void* bitmap) {
}
int SkBitmapOperator::locateConfig(void* bitmap) {
}
