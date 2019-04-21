#pragma once

#include <cstdint>

template<int Size>
class RingBuffer
{
  public:
    bool empty() const
    {
        return readIndex == writeIndex;
    }

    bool read(uint8_t* b);
    bool write(uint8_t b);

  private:
    void advance(int* cnt_p);

        uint8_t buffer[Size];
    int readIndex = 0;
    int writeIndex = 0;
};


template<int Size>
void
RingBuffer<Size>::advance(int* cnt_p)
{
    if (++(*cnt_p) == Size)
        *cnt_p = 0;
}

template<int Size>
bool
RingBuffer<Size>::read(uint8_t* b)
{
    if (empty())
        return false;
    *b = buffer[readIndex];
    advance(&readIndex);
    return true;
}

template<int Size>
bool
RingBuffer<Size>::write(uint8_t b)
{
    int next = writeIndex;
    advance(&next);
    if (next == readIndex)
        return false;
    buffer[writeIndex] = b;
    writeIndex = next;
    return true;
}

