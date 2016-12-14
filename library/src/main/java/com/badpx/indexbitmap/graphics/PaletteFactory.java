package com.badpx.indexbitmap.graphics;

import android.graphics.Color;

/**
 * Created by dk on 15-8-6.
 */
public class PaletteFactory {

    public static Palette getRGB332Palette() {
        int[] colorTable = new int[Palette.INDEX8_COLOR_TOTAL];

        int r, g, b, i = 0;
        for(r = 0; r < 256; r += 36) {
            for(g = 0; g < 256; g += 36) {
                for(b = 0; b < 256; b += 84) {
                    colorTable[i] = Color.rgb(r, g, b);
                    i++;
                }
            }
        }
        return new Palette(colorTable);
    }

    public static Palette getARGB2222Palette() {
        int[] colorTable = new int[Palette.INDEX8_COLOR_TOTAL];

        int r, g, b, a, i = 0;
        for (a = 0; a < 256; a += 84) {
            for (r = 0; r < 256; r += 84) {
                for (g = 0; g < 256; g += 84) {
                    for (b = 0; b < 256; b += 84) {
                        colorTable[i] = Color.argb(a, r, g, b);
                        i++;
                    }
                }
            }
        }
        return new Palette(colorTable);
    }

    public static Palette getARGB1232Palette() {
        int[] colorTable = new int[Palette.INDEX8_COLOR_TOTAL];

        int r, g, b, a, i = 0;
        for (a = 0; a < 256; a += 255) {
            for (r = 0; r < 256; r += 84) {
                for (g = 0; g < 256; g += 36) {
                    for (b = 0; b < 256; b += 84) {
                        colorTable[i] = Color.argb(a, r, g, b);
                        i++;
                    }
                }
            }
        }
        return new Palette(colorTable);
    }

    public static Palette getGrayPalette() {
        int[] colorTable = new int[Palette.INDEX8_COLOR_TOTAL];
        for (int i = 0; i < 256; ++i) {
            colorTable[i] = Color.rgb(i, i, i);
        }
        return new Palette(colorTable);
    }
}
