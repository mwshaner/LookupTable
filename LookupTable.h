#pragma once
#include <iostream>
#include <array>
#include <functional>
#include <stdexcept>
#include <cmath>

/*
      File: LookupTable.h
      Author: Mason Shaner
      Date: 3/30/2025
      Description: The LookupTable template class implements a 2d lookup table
      that uses linear interpolation to find the dependant value. Traditional 
      lookup tables use an iterative search to find the indices used in the 
      interpolation step, an O(N) operation. This implementation uses a branchless 
      binary search to find the lower bound of the independant value, thus offering
      a time complexity of O(logN). 
*/

template <typename T, size_t TABLE_SIZE>
class LookupTable
{
public:
    LookupTable(const std::array<T, TABLE_SIZE>& x, const std::array<T, TABLE_SIZE>& f)
        : m_x{ x },
          m_f{ f }
    {

    }

    T get(const T& t)
    {
        // Arrays must be equal in size and non-empty
        if (m_x.size() == 0 || m_f.size() != m_x.size())
        {
            return 0;
        }

        // Early exit if first or last
        if (t <= m_x.front())
        {
            return m_f.front();
        }
        if (t >= m_x.back())
        {
            return m_f.back();
        }

        size_t low = 0;
        size_t high = TABLE_SIZE;


        // Binary search to locate the interval [x[low], x[high]] that contains t.
        while (high - low > 1)
        {
            // Right shift by 1 is equal to dividing by 2
            size_t mid = low + ((high - low) >> 1);

            // Compute cmp: 1 if target > m_x[mid], 0 otherwise.
            size_t cmp = (t > m_x[mid]);

            // Create a mask from cmp: If cmp == 1 then mask becomes all 1s, else 0.
            size_t mask = -static_cast<long>(cmp);

            /*
                Update boundaries using bitwise operations.
                If mask is all ones, then low becomes mid, high remains unchanged
                If mask is 0, then high becomes mid, low remains unchanged.

                For mid = 0011, low = 0000, high = 1001, t = 1000, then mask = 1111

                low  = (0011 & 1111) | (0000 & 0000) = 0011 | 0000 =   0011
                high = (0011 & 0000) | (1001 & 1111) = 0000 | 1001 =   1001

                Thus low becomes mid and the search continues

                For mid = 0011, low = 0000, high = 1001, t = 0010, then mask = 0000

                low  = (0011 & 0000) | (0000 & 1111) = 0000 | 0000 =   0000
                high = (0011 & 1111) | (1001 & 0000) = 0011 | 0000 =   0011

                Thus high becomes mid and the search continues
            */
            low = (mid & mask) | (low & ~mask);
            high = (mid & ~mask) | (high & mask);
        }


        // Linear interpolation.
        double denom = (m_x[high] - m_x[low]);
        if (denom == 0)
        {
            return 0;
        }

        return ((m_f[high] - m_f[low]) / denom * t + (m_x[high] * m_f[low] - m_x[low] * m_f[high]) / denom);
    }

private:
    std::array<T, TABLE_SIZE> m_x;
    std::array<T, TABLE_SIZE> m_f;
};


