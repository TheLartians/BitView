#pragma once

#include <climits>
#include <cstddef>

namespace bit_view {

const size_t BITS_IN_BYTE = CHAR_BIT;

template <class C> class Container {
protected:
  C &container;

public:
  using Word = typename C::value_type;
  static const size_t WORD_SIZE = sizeof(Word) * BITS_IN_BYTE;

  Container(C &c) : container(c) {}

  /**
   * returns the number of bits in the container
   */
  size_t size() { return container.size() * WORD_SIZE; }

  /**
   * resizes the container to hold at least `N` bits.
   */
  void resizeToHold(size_t N, bool v = false) {
    Word offset = N % WORD_SIZE == 0 ? 0 : 1;
    container.resize(N / WORD_SIZE + offset, v);
  }

  /**
   * returns the value of the `i`th bit.
   */
  bool get(size_t i) const {
    Word offset = i % WORD_SIZE;
    return (container[i / WORD_SIZE] >> offset) & Word(1);
  }

  /**
   * same as `get(i)`.
   */
  bool operator[](size_t i) const { return get(i); }

  /**
   * sets the value of the `i`th bit
   */
  void set(size_t i, bool v) {
    Word offset = i % WORD_SIZE;
    if (v) {
      container[i / WORD_SIZE] |= Word(1) << offset;
    } else {
      container[i / WORD_SIZE] &= ~(Word(1) << offset);
    }
  }
};

} // namespace bit_view