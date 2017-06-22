
#ifndef __SK_BITMAP_OPERATOR_H__
#define __SK_BITMAP_OPERATOR_H__

#include <cstdint>
#include <android/bitmap.h>
#include <jni.h>
#include "SkTypeDef.h"
#include "color_table.h"

typedef struct {
    const char* innerClass;
    const char* name;
    const char* signature;
    jfieldID  fieldID;
} JavaFieldInfo;

typedef struct {
    const char* innerClass;
    const char* name;
    const char* signature;
    jmethodID methodID;
} JavaMethodInfo;

class SkBitmapOperator {
public:
    const static int INVALID_OFFSET = -1;

    const static uint8_t INDEX8_CONFIG = 3;
    // kIndex8Config value change to 2 in android 4.4.3 and above.
    const static uint8_t INDEX8_CONFIG_FOR_4_4_3 = 2;

    const static int INDEX_8_SK_COLOR_TYPE = 6;

    enum SkBitmapFieldIndex {
        SK_WIDTH,
        SK_HEIGHT,
        SK_ROW_BYTES,
        SK_CONFIG,
        SK_COLOR_TABLE,
        SK_COLOR_TYPE,
        SK_ALPHA_TYPE,
        SK_FIELDS_NUM,
    };

    enum BitmapFieldIndex {
        NATIVE_BITMAP,
        FIELDS_NUM,
    };

    enum BitmapMethodIndex {
        IS_MUTABLE,
        METHODS_NUM,
    };

    SkBitmapOperator(int* skBitmapFieldOffset, size_t skFieldNum,
    JavaFieldInfo* bitmapFields, size_t fieldNum,
    JavaMethodInfo* bitmapMethods, size_t methodNum);

    virtual ~SkBitmapOperator();

    virtual bool setup(JNIEnv* env);
    virtual bool detectMemoryLayout(JNIEnv* env, jobject index8Bitmap, jintArray palette);

    virtual void* getNativeBitmap(JNIEnv* env, jobject javaBitmap) const;
    uint8_t getConfig(JNIEnv* env, jobject javaBitmap) const;
    uint8_t setConfig(JNIEnv* env, jobject javaBitmap, uint8_t config);
    uint32_t getRowBytes(JNIEnv* env, jobject javaBitmap) const;
    uint32_t setRowBytes(JNIEnv* env, jobject javaBitmap, uint32_t rowBytes);
    uint32_t getWidth(JNIEnv* env, jobject javaBitmap) const;
    uint32_t setWidth(JNIEnv* env, jobject javaBitmap, uint32_t width);
    uint32_t getHeight(JNIEnv* env, jobject javaBitmap) const;
    uint32_t setHeight(JNIEnv* env, jobject javaBitmap, uint32_t height);
    virtual int32_t getAlphaType(JNIEnv *env, jobject javaBitmap) const;
    virtual int32_t setAlphaType(JNIEnv *env, jobject javaBitmap, int32_t alphaType);
    int getColorType(JNIEnv* env, jobject javaBitmap) const;
    int setColorType(JNIEnv *env, jobject javaBitmap, int32_t colorType);

    bool hasColorTable(JNIEnv* env, jobject javaBitmap) const {
        return NULL != getColorTable(env, javaBitmap);
    }
    virtual PMColor* getPalette(JNIEnv *env, jobject javaBitmap, /*Output*/
                                uint16_t *count) const;
    virtual int setPalette(JNIEnv *env, jobject javaBitmap, PMColor *palette,
                           uint16_t count);

    uint8_t getIndex8ConfigValue() const {
        return mIndex8ConfigRealValue;
    }

protected:
    const static int TRAVERSAL_TIMES = 64;

    virtual bool locateColorTable(JNIEnv* env, char* bitmap, jintArray expectPalette) const;
    virtual uint8_t locateConfig(char *bitmap) const;
    virtual bool locateRowBytes(char* bitmap, uint32_t expectRowBytes) const {
        return (INVALID_OFFSET != mSkBitmapFieldOffset[SK_ROW_BYTES]
                && *((uint32_t*)(bitmap + mSkBitmapFieldOffset[SK_ROW_BYTES])) == expectRowBytes);
    }

    virtual bool locateSize(char* bitmap, uint32_t expectWidth, uint32_t expectHeight) const {
        return (INVALID_OFFSET != mSkBitmapFieldOffset[SK_WIDTH]
                && INVALID_OFFSET != mSkBitmapFieldOffset[SK_HEIGHT]
                &&*((uint32_t*)(bitmap + mSkBitmapFieldOffset[SK_WIDTH])) == expectWidth
                && *((uint32_t*)(bitmap + mSkBitmapFieldOffset[SK_HEIGHT])) == expectHeight);
    }

    virtual bool locateColorType(char* bitmap) const;
    virtual bool locateAlphaType(char* bitmap) const;

    virtual void* getColorTable(JNIEnv* env, jobject javaBitmap) const;
    virtual bool travelForNativeFields(JNIEnv* env, jobject index8Bitmap, jintArray palette);

    void setIndex8ConfigValue(uint8_t value) {
        mIndex8ConfigRealValue = value;
    }

    void setAllFieldsLocated() {
        mIsAllFieldsLocated = true;
    }

    bool isAllFieldsLocated() const {
        return mIsAllFieldsLocated;
    }

    // TODO: change to std::map
    int* mSkBitmapFieldOffset;
    JavaFieldInfo* mBitmapFieldInfo;
    JavaMethodInfo* mBitmapMethodInfo;

private:
    uint8_t mIndex8ConfigRealValue;
    bool mIsAllFieldsLocated;
};

#endif
