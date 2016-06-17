package com.badpx.indexbitmap.graphics;

import android.graphics.Paint;

/**
 * Created with IntelliJ IDEA.
 * User: dk
 * Date: 16/6/17
 */

public class IndexPaint extends Paint {
    public IndexPaint(Paint paint) {
        super(paint);
    }

    public IndexPaint(int flags) {
        super(flags);
    }

    @Override
    public int getColor() {
        return super.getAlpha();
    }

    @Override
    public void setColor(int color) {
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
