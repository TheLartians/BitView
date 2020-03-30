#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <bit_lens.h>
#include <bitset>
#include <doctest/doctest.h>
#include <type_traits>
#include <vector>

// an irregular test sequence
auto pattern = [](auto i) { return i % ((i / 3) % 7 + 1) == 0; };

TEST_CASE_TEMPLATE("bit_lens", T, unsigned char, unsigned short, unsigned int,
                   size_t) {
  std::vector<T> container;
  bit_lens::Lens bits(container);
  static_assert(std::is_same<typename decltype(bits)::Word, T>::value);
  static_assert(bit_lens::BITS_IN_BYTE >= 8);
  const auto wordSize = sizeof(T) * bit_lens::BITS_IN_BYTE;

  SUBCASE("read bits") {
    container.resize(2);
    CHECK(bits.size() == container.size() * wordSize);

    std::bitset<wordSize> bitset;
    bitset[1] = 1;
    bitset[3] = 1;
    bitset[7] = 1;

    container[0] = static_cast<T>(bitset.to_ulong());
    for (size_t i = 0; i < wordSize; ++i) {
      CAPTURE(i);
      CHECK(bits[i] == bitset[i]);
    }

    bitset[2] = 1;
    bitset[3] = 0;
    bitset[6] = 1;
    // bitset[wordSize - 1] = 1;

    // we cant set high bits in the bitset, due to a strange limitation in
    // microsoft's STL that that throws an exception. high bits will be tested
    // in the "set bits" test later.

    container[1] = static_cast<T>(bitset.to_ulong());
    for (size_t i = 0; i < wordSize; ++i) {
      CAPTURE(i);
      CHECK(bits[wordSize + i] == bitset[i]);
    }
  }

  SUBCASE("resize") {
    bool fill;
    SUBCASE("fill with 0") { fill = false; }
    SUBCASE("fill with 1") { fill = true; }
    CAPTURE(fill);
    bits.resizeToHold(5 * wordSize - 3, fill);
    CHECK(bits.size() == 5 * wordSize);
    CHECK(container.size() == 5);
    bits.forEach([&](auto v, auto i) {
      CAPTURE(i);
      CHECK(v == fill);
    });
  }

  SUBCASE("set bits") {
    bits.resizeToHold(5 * wordSize);
    for (size_t i = 0; i < bits.size(); ++i) {
      CAPTURE(i);
      CHECK(bits[i] == 0);
    }

    for (size_t i = 0; i < bits.size(); ++i) {
      CAPTURE(i);
      bits.set(i, pattern(i));
    }
    for (size_t i = 0; i < bits.size(); ++i) {
      CAPTURE(i);
      CHECK(bits[i] == pattern(i));
    }
  }

  SUBCASE("iteration") {
    bits.resizeToHold(5 * wordSize);
    bits.forEach([&](auto, auto i) { bits.set(i, pattern(i)); });
    bits.forEach([&](auto v, auto i) {
      CAPTURE(i);
      CHECK(v == pattern(i));
    });
  }
}