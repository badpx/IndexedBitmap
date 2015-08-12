package com.badpx.indexbitmap;

import android.graphics.Bitmap;

/**
 * Created by dk on 15-8-11.
 */
public class BitmapHelper {
    private static final boolean IS_COLOR_TABLE_LOCATED;
    static {
        System.loadLibrary("skbitmap_helper");

        IS_COLOR_TABLE_LOCATED = locateBitmapColorTable() > 0;
    }

    private static int locateBitmapColorTable() {
        byte[] sPixels = new byte[1];
        int[] sPalette = new int[8];
        for (int i = 0; i < 8; ++i) {
            sPalette[i] = i * i + i;
        }
        Bitmap sSample = IndexBitmapFactory.createBitmap(sPixels, sPalette, 1, 1);
        return nativeLocateColorTable(sSample, sPalette);
    }

    public static int getBytePerPixel(Bitmap bitmap) {
        if (IS_COLOR_TABLE_LOCATED) {
            return nativeGetBytesPerPixel(bitmap);
        }
        return -127;
    }

    /**
     * Get color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param output The array to receive the bitmap's color table. */
    public static int getColorTable(Bitmap bitmap, int[] output) {
        if (IS_COLOR_TABLE_LOCATED) {
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
        if (IS_COLOR_TABLE_LOCATED) {
            return nativeChangeColorTable(bitmap, colorTable);
        }
        return -127;
    }

    private static native int nativeGetBytesPerPixel(Bitmap bitmap);
    private static native int nativeGetColorTable(Bitmap bitmap, int[] output);
    private static native int nativeChangeColorTable(Bitmap bitmap, int[] colorTable);

    private static native int nativeLocateColorTable(Bitmap bitmap, int[] colorTable);
}
