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


public class TestActivity extends Activity {

    public static final String TAG = "IndexBitmap";
    boolean is = BitmapHelper.IS_COLOR_TABLE_LOCATED;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

    }

    @Override
    protected void onResume() {
        super.onResume();

        byte[] pixels = new byte[100 * 100];
        for (int i = 0; i < pixels.length; ++i) {
            pixels[i] = (byte) (i % 256);
        }
        int[] colorTable = PaletteHelper.getRGB332Palette();
        Bitmap bmp = IndexBitmapFactory.createBitmap(pixels, colorTable, 0, 0, 100, 100, true);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inBitmap = bmp;
        options.inSampleSize = 1;
        Bitmap coffee = null;
/*        try {
            coffee = BitmapFactory.decodeResource(getResources(), R.drawable.coffee, options);
        } catch (Exception e) {
            e.printStackTrace();
        }
        Log.d(TAG, String.format("inBitmap reused %s", coffee == bmp ? "success" : "failed"));*/

        Log.d(TAG, String.format("result bmp w = %d, h = %d, rowBytes = %d, Config = %s",
                bmp.getWidth(), bmp.getHeight(), bmp.getRowBytes(), bmp.getConfig()));
        int[] palette = PaletteHelper.getARGB1232Palette();
        palette[0] = Color.WHITE;
        int ret = BitmapHelper.changeColorTable(bmp, palette);
        Log.d(TAG, "palette count = " + ret);

        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap(null != coffee ? coffee : bmp);
    }

}
