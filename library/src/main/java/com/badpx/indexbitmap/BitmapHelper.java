package com.badpx.indexbitmap;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Build;

/**
 * Created by dk on 15-8-11.
 */
public class BitmapHelper {
    private static Boolean sIsInitialized;

    static {
        try {
            System.loadLibrary("skbitmap_helper");
            sIsInitialized = initialize();
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

    private static Boolean initialize() {
        if (null == sIsInitialized) {
            synchronized (BitmapHelper.class) {
                if (null == sIsInitialized) {
                    byte[] sPixels = new byte[2];
                    int[] palette = new int[8];
                    for (int i = 0; i < 8; ++i) {
                        palette[i] = Color.rgb(0xAA, i * i + i, 0xAA);
                    }
                    // IMPORTANT: Create a tiny indexed bitmap to detecting native memory model.
                    Bitmap detector = IndexBitmapFactory.createBitmap(sPixels, palette, 1, 2);
                    sIsInitialized = nativeInit(detector, palette);
                    detector.recycle();
                }
            }
        }
        return null != sIsInitialized ? sIsInitialized : false;
    }

    public static boolean isInitialized() {
        return sIsInitialized;
    }

    public static int getBytePerPixel(Bitmap bitmap) {
        return nativeGetBytesPerPixel(bitmap);
    }

    /**
     * Get color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param output The array to receive the bitmap's color table. */
    public static int getPalette(Bitmap bitmap, int[] output) {
        return nativeGetPalette(bitmap, output);
    }

    /**
     * Change color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param palette Array of Colors (pre-multiplied 32-bit colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the palette.*/
    public static int changePalette(Bitmap bitmap, int[] palette) {
        return nativeChangePalette(bitmap, palette);
    }

    public static int index8FakeToAlpha8(Bitmap bitmap, boolean faked) {
        return nativeIndex8FakeToAlpha8(bitmap, faked);
    }

    public static int getNativeIndex8Config() {
        if (Build.VERSION.SDK_INT < 20) { // Android 5.0 below
            return nativeGetIndex8Config();
        }
        return 0;
    }

    private static native boolean nativeInit(Bitmap index8Bitmap, int[] palette);
    private static native int nativeGetBytesPerPixel(Bitmap bitmap);
    private static native int nativeGetPalette(Bitmap bitmap, int[] output);
    private static native int nativeChangePalette(Bitmap bitmap, int[] palette);

    private static native int nativeIndex8FakeToAlpha8(Bitmap bitmap, boolean faked);
    private static native int nativeGetConfig(Bitmap bitmap);
    private static native int nativeSetConfig(Bitmap bitmap, int config);
    private static native int nativeGetIndex8Config();
}
