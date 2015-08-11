
#include <android/bitmap.h>
#include "skbitmap_helper.h"

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

jint JNICALL nativeGetBytesPerPixel(JNIEnv* env, jobject, jobject javaBitmap) {
    if (NULL != javaBitmap) {
        AndroidBitmapInfo bmpInfo;
        AndroidBitmap_getInfo(env, javaBitmap, &bmpInfo);
        return computeBytesPerPixel(bmpInfo.format);
    }
    return 0;
}
