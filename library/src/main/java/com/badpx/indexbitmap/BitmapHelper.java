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

    public static int locateBitmapColorTable() {
        byte[] sPixels = new byte[1];
        int[] sPalette = new int[8];
        for (int i = 0; i < 8; ++i) {
            sPalette[i] = i * i + i;
        }
        Bitmap sSample = IndexBitmapFactory.createBitmap(sPixels, sPalette, 1, 1);
        return locateColorTable(sSample, sPalette);
    }

    public static native int getBytesPerPixel(Bitmap bitmap);
    private static native int locateColorTable(Bitmap bitmap, int[] palette);
    public static native int getColorTable(Bitmap bitmap, int[] output);
    public static native int changeColorTable(Bitmap bitmap, int[] palette);
}
