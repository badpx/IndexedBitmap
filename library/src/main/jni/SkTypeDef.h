
#ifndef __SK_TYPE_DEF_H__
#define __SK_TYPE_DEF_H__

/**
 *  Describes how to interpret the alpha compoent of a pixel.
 */
enum SkAlphaType {
    /**
     *  All pixels should be treated as opaque, regardless of the value stored
     *  in their alpha field. Used for legacy images that wrote 0 or garbarge
     *  in their alpha field, but intended the RGB to be treated as opaque.
     */
    kIgnore_SkAlphaType,
    /**
     *  All pixels are stored as opaque. This differs slightly from kIgnore in
     *  that kOpaque has correct "opaque" values stored in the pixels, while
     *  kIgnore may not, but in both cases the caller should treat the pixels
     *  as opaque.
     */
    kOpaque_SkAlphaType,
    /**
     *  All pixels have their alpha premultiplied in their color components.
     *  This is the natural format for the rendering target pixels.
     */
    kPremul_SkAlphaType,
    /**
     *  All pixels have their color components stored without any regard to the
     *  alpha. e.g. this is the default configuration for PNG images.
     *
     *  This alpha-type is ONLY supported for input images. Rendering cannot
     *  generate this on output.
     */
    kUnpremul_SkAlphaType,
    kLastEnum_SkAlphaType = kUnpremul_SkAlphaType
};

/**
 *  Describes how to interpret the components of a pixel.
 *
 *  kN32_SkColorType is an alias for whichever 32bit ARGB format is the "native"
 *  form for skia's blitters. Use this if you don't have a swizzle preference
 *  for 32bit pixels.
 */
enum SkColorType {
    kUnknown_SkColorType,
    kAlpha_8_SkColorType,
    kRGB_565_SkColorType,
    kARGB_4444_SkColorType,
    kRGBA_8888_SkColorType,
    kBGRA_8888_SkColorType,
    kIndex_8_SkColorType,
    kGray_8_SkColorType,
    kLastEnum_SkColorType = kGray_8_SkColorType,
//#if SK_PMCOLOR_BYTE_ORDER(B,G,R,A)
//    kN32_SkColorType = kBGRA_8888_SkColorType,
//#elif SK_PMCOLOR_BYTE_ORDER(R,G,B,A)
//    kN32_SkColorType = kRGBA_8888_SkColorType,
//#else
//    #error "SK_*32_SHFIT values must correspond to BGRA or RGBA byte order"
//#endif
};

#endif