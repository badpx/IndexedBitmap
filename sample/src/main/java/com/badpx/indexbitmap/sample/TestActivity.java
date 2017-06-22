package com.badpx.indexbitmap.sample;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
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
        Palette palette = PaletteFactory.getARGB1232Palette();
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

        Bitmap icon = BitmapFactory.decodeResource(getResources(), R.drawable.coffee);
        BitmapHelper.index8FakeToAlpha8(bmp, true);
        Canvas canvas = new Canvas(bmp);
        IndexPaint paint = new IndexPaint(Paint.ANTI_ALIAS_FLAG);
//        canvas.drawBitmap(icon, 0, 0, paint);
        paint.setColorIndex(palette.lookupColorIndex(Color.RED)[0]);
        paint.setTextSize(38);
        canvas.drawText("Hello World!", 400, 100, paint);
        paint.setColorIndex(palette.lookupColorIndex(Color.GREEN)[0]);
        canvas.drawCircle(200, 400, 50, paint);
        paint.setColorIndex(palette.lookupColorIndex(Color.BLUE)[0]);
        canvas.drawRect(mScreenWidth / 2, mScreenHeight / 2, mScreenWidth, mScreenHeight, paint);

        for (int y = 0; y < icon.getHeight(); ++y) {
            for (int x = 0; x < icon.getWidth(); ++x) {
                paint.setColorIndex(palette.lookupColorIndex(icon.getPixel(x, y))[0]);
                canvas.drawPoint(x, y, paint);
            }
        }

        int c = bmp.getPixel(1, 1);
        Toast.makeText(getApplicationContext(), "Pixel = 0x" + Integer.toHexString(c), Toast.LENGTH_LONG).show();
        BitmapHelper.index8FakeToAlpha8(bmp, false);

        ImageView imageView = (ImageView) findViewById(R.id.imageview);
        imageView.setImageBitmap( bmp);
//        ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION);
    }

}
