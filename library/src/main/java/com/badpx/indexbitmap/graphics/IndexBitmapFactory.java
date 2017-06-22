package com.badpx.indexbitmap.graphics;

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

    private static int NATIVE_INDEX_CONFIG;

    static {
        testNativeIndexConfigEnum();
    }

    // Decide the value of native index bitmap config.
    private static void testNativeIndexConfigEnum() {
        // Since Android 5.0 the SkBitmap::Config was deprecated.
        if (Build.VERSION.SDK_INT < 20) {
            NATIVE_INDEX_CONFIG = 2;   // For Android 4.4.3 and above
            Parcel parcel = createIndexBitmapParcel(new byte[1], new int[1], 0, 0, 1, 1, false);
            try {
                Bitmap.CREATOR.createFromParcel(parcel).recycle();
            } catch (Exception e) {
                Log.d(TAG, "Android version <= 4.4.2");
                NATIVE_INDEX_CONFIG = 3;   // For Android 4.4.2 and below
            } finally {
                parcel.recycle();
            }
        }
    }

    public static Bitmap createBitmap(byte[] pixels, Palette palette, int width, int height) {
        return createBitmap(pixels, palette.getColorTable(), 0, 0, width, height);
    }

    /**
     * Create a 8-bits immutable indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @return Return a Bitmap instance or null
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
     * @param stride Number of pixels in the array between rows (must be &gt;= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @return Return a Bitmap instance or null
     */
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride,
                                      int width, int height) {
        return createBitmap(pixels, colorTable, offset, stride, width, height, true);
    }

    public static Bitmap createBitmap(byte[] pixels, Palette palette, int offset, int stride,
                                      int width, int height, boolean mutable) {
        return createBitmap(pixels, palette.getColorTable(), offset, stride, width, height, mutable);
    }

    /**
     * Create a 8-bits indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.This array must be at least as large as width * height.
     * @param colorTable Array of Colors (non-premultiplied 32-bit ARGB colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.
     * @param offset Number of values to skip before the first pixel in the array of pixels.
     * @param stride Number of pixels in the array between rows (must be &gt;= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @param mutable True if the resulting bitmap should be mutable (i.e. its pixels can be modified)
     * @return Return a Bitmap instance or null
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
            parcel.writeInt(NATIVE_INDEX_CONFIG);
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
