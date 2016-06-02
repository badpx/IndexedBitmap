package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.*;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.badpx.indexbitmap.BitmapHelper;
import com.badpx.indexbitmap.IndexBitmapFactory;
import com.badpx.indexbitmap.PaletteHelper;


public class TestActivity extends Activity {

    public static final String TAG = "IndexBitmap";

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

    }

    @Override
    protected void onResume() {
        super.onResume();

        byte[] pixels = new byte[400 * 400];
        for (int i = 0; i < pixels.length; ++i) {
            pixels[i] = (byte) (i % 256);
        }
        int[] colorTable = PaletteHelper.getRGB332Palette();
        Bitmap bmp = IndexBitmapFactory.createBitmap(pixels, colorTable, 0, 0, 400, 400, true);

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
//        palette[0] = Color.RED;
        int ret = BitmapHelper.changePalette(bmp, palette);
//        Log.d(TAG, "palette count = " + ret);

        Bitmap coffe = BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher);
        BitmapHelper.index8FakeToAlpha8(bmp, true);
        Canvas canvas = new Canvas();
        canvas.setBitmap(bmp);
        bmp.eraseColor(0);
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        canvas.drawBitmap(coffe, 0, 0, paint);
        paint.setAlpha(1);
        paint.setTextSize(48);
        canvas.drawText("Hello World!", 50, 50, paint);
        paint.setAlpha(127);
        canvas.drawCircle(200, 200, 50, paint);
        paint.setAlpha(200);
        canvas.drawRect(350, 350, 400, 400, paint);
        canvas.setBitmap(null);
        BitmapHelper.index8FakeToAlpha8(bmp, false);

        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap( bmp);
    }

}
