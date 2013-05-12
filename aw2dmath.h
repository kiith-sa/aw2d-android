#ifndef AW2DMATH_H
#define AW2DMATH_H

/// Is the given number a power of two?
bool isPowerOfTwo(const unsigned int x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

#endif // AW2DMATH_H
