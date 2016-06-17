package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.*;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.widget.ImageView;

import com.badpx.indexbitmap.graphics.BitmapHelper;
import com.badpx.indexbitmap.graphics.IndexBitmapFactory;
import com.badpx.indexbitmap.graphics.Palette;
import com.badpx.indexbitmap.graphics.PaletteFactory;
import com.badpx.indexbitmap.graphics.IndexPaint;


public class TestActivity extends Activity {

    public static final String TAG = "IndexBitmap";
    private int mScreenWidth;
    private int mScreenHeight;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

        Display display = getWindowManager().getDefaultDisplay();
        mScreenWidth = display.getWidth();  // deprecated
        mScreenHeight = display.getHeight();
    }

    @Override
    protected void onResume() {
        super.onResume();

        byte[] pixels = new byte[mScreenWidth * mScreenHeight];
        for (int i = 0; i < pixels.length; ++i) {
            pixels[i] = (byte) (i % 256);
        }
        Palette palette = PaletteFactory.getRGB332Palette();
        Bitmap bmp = IndexBitmapFactory.createBitmap(
                pixels, palette, 0, 0, mScreenWidth, mScreenHeight, true);

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
        palette = PaletteFactory.getARGB1232Palette();
//        palette[0] = Color.RED;
        int ret = BitmapHelper.setPalette(bmp, palette);
//        Log.d(TAG, "palette count = " + ret);

        Bitmap coffe = BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher);
        BitmapHelper.index8FakeToAlpha8(bmp, true);
        Canvas canvas = new Canvas();
        canvas.setBitmap(bmp);
        bmp.eraseColor(Color.argb(128, 0, 0, 0));
        IndexPaint paint = new IndexPaint(Paint.ANTI_ALIAS_FLAG);
        canvas.drawBitmap(coffe, 0, 0, paint);
        paint.setColorIndex(1);
        paint.setTextSize(48);
        canvas.drawText("Hello World!", 50, 50, paint);
        paint.setColorIndex(127);
        canvas.drawCircle(200, 200, 50, paint);
        paint.setColorIndex(200);
        canvas.drawRect(350, 350, 400, 400, paint);
        canvas.setBitmap(null);
        BitmapHelper.index8FakeToAlpha8(bmp, false);

        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap( bmp);
    }

}
