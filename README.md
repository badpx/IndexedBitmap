# IndexBitmap

**Android support library for use 8BPP Indexed Bitmap.**

Indexed color is a technique to manage bitmap colors in a limited fashion, in order to save RAM and video memory buffer space, file storage space, telecom bandwidth. Instead of storing and managing every primary color component of every pixel, the most representative colors, are grouped into a limited size palette: an array of color elements, in which each element (a color) is indexed by its position. This way, the pixels contain not the full color components but merely their index into the palette.
This library can be used to create 8BPP(8-bits per pixel) indexed color bitmap, so it takes up storage space is a quarter of the same dimension 32-bit bitmap(like ARGB8888).

## Integration
You can either add the library to your application as a library project or add the following dependency to your build.gradle:

```
dependencies {
    compile 'com.badpx.indexbitmap:library:(1.0.0)'
}
```

## Usage

### Create the indexed bitmap:

```java
public class IndexBitmapFactory {
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int width, int height);
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride, int width, int height);
                                      
    /**
     * Create a 8-bits indexed bitmap.
     * @param pixels Array of Color Index used to initialize the pixels.
     * @param colorTable Array of Colors (pre-multiplied 32-bit colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable(array length must be <= 256).
     * @param offset Number of values to skip before the first pixel in the array of pixels.
     * @param stride Number of pixels in the array between rows (must be >= width).
     * @param width The width of the bitmap.
     * @param height The height of the bitmap.
     * @param mutable True if the resulting bitmap should be mutable (i.e. its pixels can be modified)
     */
    public static Bitmap createBitmap(byte[] pixels, int[] colorTable, int offset, int stride, int width, int height, boolean mutable);
}
```

### Access Color Table of Bitmap

```java
public class BitmapHelper {
    /**
     * Get color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param output The array to receive the bitmap's color table. */
    public static int getColorTable(Bitmap bitmap, int[] output);

    /**
     * Change color table of the bitmap.
     * @param bitmap A indexed bitmap.
     * @param colorTable Array of Colors (pre-multiplied 32-bit colors) used by 8-bit bitmaps,
     *                   where the bitmap bytes are interpreted as indices into the colorTable.*/
    public static int changeColorTable(Bitmap bitmap, int[] colorTable);
}
```

## Limitation
A Indexed Bitmap should not be used as target for a Canvas, that means the below code doesn't work:

```java
Canvas canvas = new Canvas(aIndexedBitmap);
canvas.drawSomething(...);   // It can't draw anything!
```
