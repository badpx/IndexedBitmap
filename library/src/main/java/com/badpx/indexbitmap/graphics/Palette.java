package com.badpx.indexbitmap.graphics;

import android.graphics.Color;

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

    public int[] lookupColorIndex(int...colors) {
        int len = colors.length;
        int[] results = new int[len];
        for (int n = 0; n < len; ++n) {
            float minDistance = 255;
            int color = colors[n];
            int r = Color.red(color);
            int g = Color.green(color);
            int b = Color.blue(color);
            int a = Color.alpha(color);

            for (int i = 0; i < INDEX8_COLOR_TOTAL; ++i) {
                int c = mColorTable[i];
                float dis = Math.abs(r - Color.red(c))
                        + Math.abs(g - Color.green(c))
                        + Math.abs(b - Color.blue(c))
                        + Math.abs(a - Color.alpha(c));
                dis /= 4;
                if (dis < minDistance) {
                    results[n] = i;
                    if (0 == dis) break;
                    minDistance = dis;
                }
            }
        }
        return results;
    }

    int[] getColorTable() {
        return mColorTable;
    }
}

