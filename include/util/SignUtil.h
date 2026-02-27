#pragma once

template<typename T>
constexpr T sign_extend(T value, unsigned bits)
{
    const T mask = T(1) << (bits - 1);   // sign bit
    return (value ^ mask) - mask;
}