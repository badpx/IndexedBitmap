
#ifndef __COLOR_TABLE_H__
#define __COLOR_TABLE_H__

/** return the alpha byte from a SkColor value */
#define SkColorGetA(color)      (((color) >> 24) & 0xFF)
/** return the red byte from a SkColor value */
#define SkColorGetR(color)      (((color) >> 16) & 0xFF)
/** return the green byte from a SkColor value */
#define SkColorGetG(color)      (((color) >>  8) & 0xFF)
/** return the blue byte from a SkColor value */
#define SkColorGetB(color)      (((color) >>  0) & 0xFF)

/**
 *  Pack the components into a ABGR 32-bit color, that the components are 0..255, 
 *  and are already premultiplied (i.e. alpha >= color)
 */
#define PackABGR32(a, b, g, r) ((a << 24) | (b << 16) | (g << 8) | (r << 0))

typedef uint32_t PMColor;

typedef struct ColorTable {
    private:
        void*   fVirtualPointer;
        mutable int32_t fRefCnt;

    public:
        PMColor*  fColors;
        uint16_t* f16BitCache;
        uint16_t  fCount;
        uint8_t   fFlags;
} ColorTable;

/*typedef struct ColorTableNew {
    private:
        void*   fVirtualPointer;
        mutable int32_t fRefCnt;

    public:
    SkPMColor*                          fColors;
    SkLazyPtr<uint16_t, Free16BitCache> f16BitCache; // equals to mutable T*
    int                                 fCount;
} ColorTableNew;*/

#endif
