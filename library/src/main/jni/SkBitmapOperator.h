
#ifndef __SK_BITMAP_OPERATOR_H__
#define __SK_BITMAP_OPERATOR_H__

#include <cstdint>
#include <jni.h>
#include "color_table.h"

typedef struct {
    const char* name;
    const char* signature;
    jfieldID  fieldID;
} JavaFieldInfo;

typedef struct {
    const char* name;
    const char* signature;
    jmethodID methodID;
} JavaMethodInfo;

class SkBitmapOperator {
public:
    const static int INVALID_OFFSET = -1;

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
        WIDTH,
        HEIGHT,
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
    virtual bool travelForNativeFields(JNIEnv* env, jobject javaBitmap);
    void* getNativeBitmap(JNIEnv* env, jobject javaBitmap) const;
    int getConfig(void* bitmap);
    int setConfig(void* bitmap, int config);
    int getRowBytes(void* bitmap);
    int setRowBytes(void* bitmap, int rowBytes);
    int getWidth(void* bitmap);
    int setWidth(void* bitmap, int width);
    int getHeight(void* bitmap);
    int setHeight(void* bitmap, int height);

    PMColor* getColorTable(void* bitmap, size_t* count);
    void setColorTable(void* bitmap, PMColor* colors, size_t count);

protected:
    const static int TRAVERSAL_TIMES = 32;
    virtual int locateColorTable(void* bitmap);
    virtual int locateRowBytes(void* bitmap);
    virtual int locateSize(void* bitmap);
    virtual int locateConfig(void* bitmap);

    int mSkBitmapFieldBase;
    int* mSkBitmapFieldOffset;
    JavaFieldInfo* mBitmapFieldInfo;
    JavaMethodInfo* mBitmapMethodInfo;
};

#endif
