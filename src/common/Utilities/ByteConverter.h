
#ifndef TRINITY_BYTECONVERTER_H
#define TRINITY_BYTECONVERTER_H

/** ByteConverter reverse your byte order.  This is use
    for cross platform where they have different endians.
 */

#include "Define.h"
#include <algorithm>
#include <boost/endian/conversion.hpp>

template<typename T>
inline void EndianConvert(T& val){ boost::endian::native_to_little_inplace<T>(val);}
template<typename T>
inline void EndianConvertReverse(T& val) { boost::endian::native_to_big_inplace<T>(val); }

template<typename T>
void EndianConvert(T*); // will generate link error
template<typename T>
void EndianConvertReverse(T*); // will generate link error

namespace ByteConverter {
template<size_t T>
inline void convert(char* val)
{
    std::swap(*val, *(val + T - 1));
    convert<T - 2>(val + 1);
}

template<>
inline void convert<0>(char*) {}
template<>
inline void convert<1>(char*) {} // ignore central byte

template<typename T>
inline void apply(T* val)
{
    convert<sizeof(T)>((char*)(val));
}
}

// boost::endian does not allow conversions of floats/doubles with boost version < 1.74, and with version >= 1.74 does basicaly the following
inline void EndianConvert(float& val)
{
    static_assert(sizeof(float) == sizeof(uint32), "Endian Convert not implemented for floats");
    uint32 v2;
    memcpy(&v2,&val,sizeof(val));
    v2 = boost::endian::native_to_little<uint32>(v2);
    memcpy(&val,&v2,sizeof(val));
}

inline void EndianConvert(double& val)
{
    static_assert(sizeof(double) == sizeof(uint64), "Endian Convert not implemented for doubles");
    uint64 v2;
    memcpy(&v2,&val,sizeof(val));
    v2 = boost::endian::native_to_little<uint64>(v2);
    memcpy(&val,&v2,sizeof(val));
}
inline void EndianConvert(uint8&) {}
inline void EndianConvert(int8&) {}
inline void EndianConvertReverse(uint8&) {}
inline void EndianConvertReverse(int8&) {}

#endif
