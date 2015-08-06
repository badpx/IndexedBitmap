package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.badpx.indexbitmap.IndexBitmapFactory;

import java.util.Arrays;


public class TestActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

    }

    @Override
    protected void onResume() {
        super.onResume();

        byte[] pixels = new byte[100 * 100];
        Arrays.fill(pixels, (byte)1);
        int[] colorTable = new int[256];
        for (int i = 0; i < 256; ++i) {
            colorTable[i] = Color.argb(128, 128, 0, 0);
        }
        Bitmap bmp = IndexBitmapFactory.createBitmap(pixels, colorTable, 100, 100);

        Log.d("IndexBitmap", String.format("result bmp w = %d, h = %d, rowBytes = %d, Config = %s",
                bmp.getWidth(), bmp.getHeight(), bmp.getRowBytes(), bmp.getConfig()));
        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap(bmp);
    }

}
