package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.widget.ImageView;
import android.widget.Toast;
import com.badpx.indexbitmap.graphics.*;


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
            pixels[i] = (byte) 255;
//            pixels[i] = (byte) (i % 256);
        }
        Palette palette = PaletteFactory.getGrayPalette();
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
//        palette = PaletteFactory.getARGB1232Palette();
//        palette[0] = Color.RED;
//        int ret = BitmapHelper.setPalette(bmp, palette);
//        Log.d(TAG, "palette count = " + ret);

        Bitmap coffe = BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher);
        BitmapHelper.index8FakeToAlpha8(bmp, true);
        Canvas canvas = new Canvas();
        canvas.setBitmap(bmp);
//        bmp.eraseColor(Color.argb(128, 0, 0, 0));
        IndexPaint paint = new IndexPaint(Paint.ANTI_ALIAS_FLAG);
        canvas.drawBitmap(coffe, 0, 0, paint);
        paint.setColorIndex(0x1);
        paint.setTextSize(38);
        canvas.drawText("Hello World!", 100, 100, paint);
        paint.setColorIndex(127);
        canvas.drawCircle(200, 200, 50, paint);
        paint.setColorIndex(127);
        canvas.drawRect(mScreenWidth / 2, mScreenHeight / 2, mScreenWidth, mScreenHeight, paint);
        canvas.setBitmap(null);
        int c = bmp.getPixel(1, 1);
        Toast.makeText(getApplicationContext(), "Pixel = 0x" + Integer.toHexString(c), Toast.LENGTH_LONG).show();
        BitmapHelper.index8FakeToAlpha8(bmp, false);

        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap( bmp);
//        ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION);
    }

}
