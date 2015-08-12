
#ifndef __COLOR_TABLE_H__
#define __COLOR_TABLE_H__

typedef uint32_t PMColor;

typedef struct ColorTable {
    private:
        void*   fVirtualTable;
        mutable int32_t fRefCnt;

    public:
        PMColor*  fColors;
        uint16_t* f16BitCache;
        uint16_t  fCount;
        uint8_t   fFlags;
} ColorTable;

#endif
