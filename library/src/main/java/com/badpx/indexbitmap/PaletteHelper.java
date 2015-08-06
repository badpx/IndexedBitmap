package com.badpx.indexbitmap;

import android.graphics.Color;

/**
 * Created by dk on 15-8-6.
 */
public class PaletteHelper {
    public final static int MAX_COLORS = 256;

    private static int[] sFakeTrueColorTable;

    public static int[] getRGB332Palette() {
        if (null == sFakeTrueColorTable) {
            int[] palette = new int[MAX_COLORS];

            int r, g, b, i = 0;
            for(r = 0; r < 256; r += 36) {
                for(g = 0; g < 256; g += 36) {
                    for(b = 0; b < 256; b += 84) {
                        palette[i] = Color.rgb(b, g, r);
                        i++;
                    }
                }
            }

            sFakeTrueColorTable = palette;
        }
        return sFakeTrueColorTable;
    }

    public static int[] getARGB2222Palette() {
        if (null == sFakeTrueColorTable) {
            int[] palette = new int[MAX_COLORS];

            int r, g, b, a, i = 0;
            for (a = 0; a < 256; a += 84) {
                for (r = 0; r < 256; r += 84) {
                    for (g = 0; g < 256; g += 84) {
                        for (b = 0; b < 256; b += 84) {
                            palette[i] = Color.argb(a, b, g, r);
                            i++;
                        }
                    }
                }
            }

            sFakeTrueColorTable = palette;
        }
        return sFakeTrueColorTable;
    }

    public static int[] getARGB1232Palette() {
        if (null == sFakeTrueColorTable) {
            int[] palette = new int[MAX_COLORS];

            int r, g, b, a, i = 0;
            for (a = 0; a < 256; a += 255) {
                for (r = 0; r < 256; r += 84) {
                    for (g = 0; g < 256; g += 36) {
                        for (b = 0; b < 256; b += 84) {
                            palette[i] = Color.argb(a, b, g, r);
                            i++;
                        }
                    }
                }
            }

            sFakeTrueColorTable = palette;
        }
        return sFakeTrueColorTable;
    }
}
