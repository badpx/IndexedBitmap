package com.badpx.indexbitmap;

import android.graphics.Bitmap;

/**
 * Created by dk on 15-8-11.
 */
public class BitmapHelper {
    static {
        System.loadLibrary("skbitmap_helper");
    }
    public static native int getBytesPerPixel(Bitmap bitmap);
}
