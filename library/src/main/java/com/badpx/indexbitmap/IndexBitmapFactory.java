package com.badpx.indexbitmap;

import android.graphics.Bitmap;
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
    private static final int IN_PLACE_FLAG_BASE_ABOVE_KITKAT = 32;

    private static int NATIVE_INDEX_CONFIG = 1;

    static {
        NATIVE_INDEX_CONFIG = testNativeIndexConfigEnum();
    }

    // Decide the value of native index bitmap config.
    private static int testNativeIndexConfigEnum() {
        int nativeConfig = 2;   // For Android 4.4.3 and above
        if (Build.VERSION.SDK_INT > 20) { // For Android 5.0 and above
            // Since Android 5.0 the SkBitmap::Config was deprecated.
            return nativeConfig;
        }

        Parcel parcel = createIndexBitmapParcel(new byte[1], null, 0, 0, 1, 1, false);

        try {
            Bitmap.CREATOR.createFromParcel(parcel).recycle();
        } catch (Exception e) {
            Log.d(TAG, "System version <= 4.4.2");
            nativeConfig = 3;   // For Android 4.4.2 and below
        } finally {
            parcel.recycle();
        }

        return nativeConfig;
    }

    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int width, int height) {
        return createBitmap(pixels, colorTable, 0, 0, width, height);
    }

    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride,
                                      int width, int height) {
        return createBitmap(pixels, colorTable, offset, stride, width, height, false);
    }

    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride,
                                      int width, int height, boolean mutable) {
        Parcel parcel = createIndexBitmapParcel(pixels, colorTable, offset, stride, width, height, mutable);
        try {
            Bitmap bitmap = Bitmap.CREATOR.createFromParcel(parcel);
            parcel.recycle();
            return bitmap;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    private static Parcel createIndexBitmapParcel(byte[] pixels, int[] colorTable, int offset, int stride, int width, int height, boolean mutable) {
        int inPlaceFlagBase = IN_PLACE_FLAG_BASE;
        Parcel parcel = Parcel.obtain();

        parcel.writeInt(mutable ? 1 : 0); // mutable
        if (Build.VERSION.SDK_INT > 20) { // For Android 5.0 and above
            parcel.writeInt(6); // SkColorType
            parcel.writeInt(2); // kPremul_SkAlphaType
            inPlaceFlagBase = IN_PLACE_FLAG_BASE_ABOVE_KITKAT;
        } else {
            parcel.writeInt(NATIVE_INDEX_CONFIG); // SkBitmap::Config
        }
        parcel.writeInt(width);    // width
        parcel.writeInt(height);   // height
        parcel.writeInt(0 != stride ? stride : width); // row bytes
        parcel.writeInt(0);  // density

        int colorCount = 0;
        if (null != colorTable) {
            colorCount = colorTable.length;
            parcel.writeIntArray(colorTable);
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
