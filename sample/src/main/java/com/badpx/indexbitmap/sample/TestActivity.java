package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.badpx.indexbitmap.BitmapHelper;
import com.badpx.indexbitmap.IndexBitmapFactory;
import com.badpx.indexbitmap.PaletteHelper;

import java.util.Arrays;


public class TestActivity extends Activity {

    public static final String TAG = "IndexBitmap";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

    }

    @Override
    protected void onResume() {
        super.onResume();

        byte[] pixels = new byte[720 * 1280];
        for (int i = 0; i < pixels.length; ++i) {
            pixels[i] = (byte) (i % 256);
        }
        int[] colorTable = PaletteHelper.getRGB332Palette();
        Bitmap bmp = IndexBitmapFactory.createBitmap(pixels, colorTable, 0, 0, 720, 1280, true);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inBitmap = bmp;
        options.inSampleSize = 1;
        Bitmap coffee = null;
//        try {
//            coffee = BitmapFactory.decodeResource(getResources(), R.drawable.coffee, options);
//            Log.d(TAG, String.format("inBitmap reused %s", coffee == bmp ? "success" : "failed"));
//        } catch (Exception e) {
//            e.printStackTrace();
//        }

        Log.d(TAG, String.format("result bmp w = %d, h = %d, rowBytes = %d, Config = %s",
                bmp.getWidth(), bmp.getHeight(), bmp.getRowBytes(), bmp.getConfig()));
        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap(null != coffee ? coffee : bmp);
        int[] palette = new int[256];
        int ret = BitmapHelper.getColorTable(bmp, palette);
        Log.d(TAG, "get palette ret = " + ret);
    }

}
