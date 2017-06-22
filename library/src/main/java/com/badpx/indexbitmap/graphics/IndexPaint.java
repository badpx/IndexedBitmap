package com.badpx.indexbitmap.graphics;

import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;

/**
 * Created with IntelliJ IDEA.
 * User: dk
 * Date: 16/6/17
 */

public class IndexPaint extends Paint {
    public IndexPaint(Paint paint) {
        super(paint);
        // Use PorterDuff.SRC because indexed color is a type of indirect color
        // presentation, so can't be blended correctly
        setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
    }

    public IndexPaint(int flags) {
        super(flags);
        setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
    }

    @Override
    public int getColor() {
        return super.getAlpha();
    }

    @Override
    public void setColor(int color) {
        // TODO: find closet color from color table
        super.setAlpha(color & 0xFF);
    }

    /**
     * Set the paint's color index, which used to find real RGBA value from color table*/
    public void setColorIndex(int color) {
        super.setAlpha(color & 0xFF);
    }

    public int getColorIndex() {
        return super.getAlpha();
    }
}
