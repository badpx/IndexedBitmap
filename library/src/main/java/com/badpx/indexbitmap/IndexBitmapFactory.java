package com.badpx.indexbitmap;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Build;
import android.os.Parcel;
import android.util.Log;

import java.util.Arrays;


/**
 * Created by dk on 15-8-6.
 */
public class IndexBitmapFactory {
    private final static String TAG = "IndexBitmapFactory";
    private static final int IN_PLACE_FLAG_BASE = 28;
    private static final int IN_PLACE_FLAG_BASE_ABOVE_KITKAT = 32;  // Add write alpha type to parcel

    /**
     * Create a 8-bits immutable indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     */
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int width, int height) {
        return createBitmap(pixels, colorTable, 0, 0, width, height);
    }

    /**
     * Create a 8-bits immutable indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param offset Number of values to skip before the first pixel in the array of pixels.
     * @param stride Number of pixels in the array between rows (must be >= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     */
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride,
                                      int width, int height) {
        return createBitmap(pixels, colorTable, offset, stride, width, height, false);
    }

    /**
     * Create a 8-bits indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.This array must be at least as large as width * height.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param offset Number of values to skip before the first pixel in the array of pixels.
     * @param stride Number of pixels in the array between rows (must be >= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @param mutable True if the resulting bitmap should be mutable (i.e. its pixels can be modified)
     */
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride,
                                      int width, int height, boolean mutable) {
        Parcel parcel = createIndexBitmapParcel(pixels, colorTable, offset, stride, width, height, mutable);
        try {
            return Bitmap.CREATOR.createFromParcel(parcel);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            parcel.recycle();
        }
        return null;
    }

    /**
     * Create a 8-bits indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param offset Number of values to skip before the first pixel in the array of pixels.
     * @param stride Number of pixels in the array between rows (must be >= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @param mutable True if the resulting bitmap should be mutable (i.e. its pixels can be modified)
     */
    private static Parcel createIndexBitmapParcel(byte[] pixels, int[] colorTable, int offset, int stride, int width, int height, boolean mutable) {
        int inPlaceFlagBase = IN_PLACE_FLAG_BASE;
        Parcel parcel = Parcel.obtain();

        parcel.writeInt(mutable ? 1 : 0); // mutable
        if (Build.VERSION.SDK_INT > 20) { // For Android 5.0 and above
            parcel.writeInt(6); // SkColorType
            parcel.writeInt(2); // kPremul_SkAlphaType
            inPlaceFlagBase = IN_PLACE_FLAG_BASE_ABOVE_KITKAT;
        } else if (Build.VERSION.SDK_INT < 20) {
            // Value of SkBitmap::Config::kIndex8_Config
            parcel.writeInt(BitmapHelper.getNativeIndex8Config());
        }
        parcel.writeInt(width);    // width
        parcel.writeInt(height);   // height
        parcel.writeInt(0 != stride ? stride : width); // row bytes
        parcel.writeInt(0);  // density

        int colorCount = 0;
        if (null != colorTable) {
            colorCount = colorTable.length;
            parcel.writeInt(colorCount);
            for (int i = 0; i < colorCount; ++i) {
                int color = colorTable[i];
                int a = Color.alpha(color);
                int r = Color.red(color);
                int g = Color.green(color);
                int b = Color.blue(color);
                // Alpha pre-multiplied:
                float alphaFactor = a / 255f;
                r *= alphaFactor;
                g *= alphaFactor;
                b *= alphaFactor;
                // Turn ARGB to ABGR which was used in Skia internal.
                parcel.writeInt(Color.argb(a,
                        (int) (b * alphaFactor),
                        (int) (g * alphaFactor),
                        (int) (r * alphaFactor)));
            }
        } else {
            // it's no color table
            parcel.writeInt(0);
        }

        parcel.writeByteArray(pixels, offset, pixels.length - offset);

        byte[] rawBuf = parcel.marshall();
        Log.d("IndexBitmap", "Raw buffer length = " + rawBuf.length);
        int inPlaceFlagOffset = inPlaceFlagBase + colorCount * 4;
        Arrays.fill(rawBuf, inPlaceFlagOffset, inPlaceFlagOffset + 4, (byte)0);
        parcel.unmarshall(rawBuf, 0, rawBuf.length);

        // Reset the parcel
        parcel.setDataPosition(0);

        return parcel;
    }
}
