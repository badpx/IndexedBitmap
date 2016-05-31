package com.badpx.indexbitmap;

import android.graphics.Bitmap;
import android.graphics.Color;

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

    public static Boolean initialize() {
        if (null == sIsInitialized) {
            synchronized (BitmapHelper.class) {
                if (null == sIsInitialized) {
                    byte[] sPixels = new byte[1];
                    int[] sPalette = new int[8];
                    for (int i = 0; i < 8; ++i) {
                        sPalette[i] = Color.rgb(0, i * i + i, 0);
                    }
                    // IMPORTANT: Create a tiny indexed bitmap to detecting native memory model.
                    Bitmap detector = IndexBitmapFactory.createBitmap(sPixels, sPalette, 1, 1);
                    sIsInitialized = nativeInit(detector);
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
        if (sIsInitialized) {
            return nativeGetBytesPerPixel(bitmap);
        }
        return -127;
    }

    /**
     * Get color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param output The array to receive the bitmap's color table. */
    public static int getColorTable(Bitmap bitmap, int[] output) {
        if (sIsInitialized) {
            return nativeGetColorTable(bitmap, output);
        }
        return -127;
    }

    /**
     * Change color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param colorTable Array of Colors (pre-multiplied 32-bit colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.*/
    public static int changeColorTable(Bitmap bitmap, int[] colorTable) {
        if (sIsInitialized) {
            return nativeChangeColorTable(bitmap, colorTable);
        }
        return -127;
    }

    public static int index8FakeToAlpha8(Bitmap bitmap, boolean faked) {
        return nativeIndex8FakeToAlpha8(bitmap, faked);
    }

    private static native boolean nativeInit(Bitmap index8Bitmap);
    private static native int nativeGetBytesPerPixel(Bitmap bitmap);
    private static native int nativeGetColorTable(Bitmap bitmap, int[] output);
    private static native int nativeChangeColorTable(Bitmap bitmap, int[] colorTable);

    private static native int nativeIndex8FakeToAlpha8(Bitmap bitmap, boolean faked);
    private static native int nativeGetConfig(Bitmap bitmap);
    private static native int nativeSetConfig(Bitmap bitmap, int config);
}
