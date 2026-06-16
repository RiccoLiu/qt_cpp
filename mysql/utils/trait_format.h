

#ifndef UTILS_TRAIT_FORMAT_H
#define UTILS_TRAIT_FORMAT_H

// #include <utils/TypeHelpers.h>
#include "pixel_type.h"

namespace utils {

#define MAKE_FORMAT(c1,c2,c3,c4,c5,c6,c7,c8) ( (((uint64_t)(c1))<<56)   | (((uint64_t)(c2))<<48) \
                                               | (((uint64_t)(c3))<<40) | (((uint64_t)(c4))<<32) \
                                               | (((uint64_t)(c5))<<24) | (((uint64_t)(c6))<<16) \
                                               | (((uint64_t)(c7))<<8) | (((uint64_t)(c8))) )

#define MAKE_FORMAT7(c1,c2,c3,c4,c5,c6,c7) MAKE_FORMAT(c1,c2,c3,c4,c5,c6,c7,0)
#define MAKE_FORMAT6(c1,c2,c3,c4,c5,c6) MAKE_FORMAT(c1,c2,c3,c4,c5,c6,0,0)
#define MAKE_FORMAT5(c1,c2,c3,c4,c5) MAKE_FORMAT(c1,c2,c3,c4,c5,0,0,0)
#define MAKE_FORMAT4(c1,c2,c3,c4) MAKE_FORMAT(c1,c2,c3,c4,0,0,0,0)


//------------------------------------
template<typename T>
struct traits_name;

template<>
struct traits_name<uint32_t>
{
    static constexpr char* value = (char*)"uint32_t";
};

template<>
struct traits_name<int32_t>
{
    static constexpr char* value = (char*)"int32_t";
};

//------------------------------------
template<typename T>
struct traits_accquire;

//------------------------------------
template<typename T>
struct traits_format;

template<>
struct traits_format<int8_t>
{
    static const uint64_t value = MAKE_FORMAT4('8','S','C','1');
};

template<>
struct traits_format<uint8_t>
{
    static const uint64_t value = MAKE_FORMAT4('8','U','C','1');
};

template<>
struct traits_format<float> {
    static const uint64_t value = MAKE_FORMAT5('3','2','F','C','1');
};

template<>
struct traits_format<double> {
    static const uint64_t value = MAKE_FORMAT5('3','2','F','C','1');
};

template<>
struct traits_format<int16_t> {
    static const uint64_t value = MAKE_FORMAT5('1','6','S','C','1');
};
template<>
struct traits_format<uint16_t> {
    static const uint64_t value = MAKE_FORMAT5('1','6','U','C','1');
};

template<>
struct traits_format<int32_t> {
    static const uint64_t value = MAKE_FORMAT5('3','2','S','C','1');
};
template<>
struct traits_format<uint32_t> {
    static const uint64_t value = MAKE_FORMAT5('3','2','U','C','1');
};

template<>
struct traits_format<int64_t> {
    static const uint64_t value = MAKE_FORMAT5('6','4','S','C','1');
};
template<>
struct traits_format<uint64_t> {
    static const uint64_t value = MAKE_FORMAT5('6','4','U','C','1');
};

template<>
struct traits_format<RGBColor> {
    static const uint64_t value = MAKE_FORMAT4('8','U','C','3');
};

template<>
struct traits_format<RGBAColor> {
    static const uint64_t value = MAKE_FORMAT4('8','U','C','4');
};

} // namespace utils

#endif // UTILS_TRAIT_FORMAT_H

