package com.badpx.indexbitmap.graphics;

import java.util.Arrays;

/**
 * Created with IntelliJ IDEA.
 * User: lokii
 * Date: 16/6/17
 */

public class Palette {
    public static final int INDEX8_COLOR_TOTAL = 256;

    private int[] mColorTable = new int[INDEX8_COLOR_TOTAL];

    public Palette(int[] colorTable) {
        int count = colorTable.length < INDEX8_COLOR_TOTAL ? colorTable.length : INDEX8_COLOR_TOTAL;
        System.arraycopy(colorTable, 0, mColorTable, 0, count);
    }

    public Palette(Palette copyFrom) {
        setPalette(copyFrom);
    }

    public int getColor(int index) {
        return (index >= 0 && index < INDEX8_COLOR_TOTAL) ? mColorTable[index] : 0;
    }

    public void setColor(int index, int color) {
        if (index >= 0 && index < INDEX8_COLOR_TOTAL) mColorTable[index] = color;
    }

    public void fillColor(int color) {
        Arrays.fill(mColorTable, color);
    }

    public void fillColor(int start, int count, int color) {
        if (start >= 0 && start + count < INDEX8_COLOR_TOTAL) {
            Arrays.fill(mColorTable, start, start + count, color);
        }
    }

    public void setPalette(Palette palette) {
        if (this != palette) {
            int[] colors = palette.getColorTable();
            System.arraycopy(colors, 0, mColorTable, 0, INDEX8_COLOR_TOTAL);
        }
    }

    public int findColorIndex(int color) {
        return 0;
    }

    int[] getColorTable() {
        return mColorTable;
    }
}

