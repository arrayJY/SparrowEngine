//
// Created by arrayJY on 2023/01/16.
//

#ifndef SPARROWENGINE_FIXED_STRING_H
#define SPARROWENGINE_FIXED_STRING_H

#include <algorithm>

namespace Sparrow {

template <typename CharT, std::size_t N>
struct FixedString {
  explicit constexpr FixedString() = default;
  explicit constexpr FixedString(const CharT (&s)[N]) {
    std::copy(s, s + N, _data);
  }

  constexpr auto operator<=>(const FixedString&) const = default;

  template <std::size_t M>
  constexpr auto operator+(const FixedString<CharT, M>& s) const {
    auto newString = FixedString<CharT, N + M - 1>();
    std::copy(_data, _data + N, newString._data);
    std::copy(s._data, s._data + M, newString._data + N - 1);
    return newString;
  }

  template <std::size_t start, std::size_t end = N>
  [[nodiscard]] constexpr auto substr() const {
    constexpr auto size = end - start;
    auto newString = FixedString<CharT, size>();
    std::copy(_data + start, _data + end, newString._data);
    newString._data[size - 1] = '\0';
    return newString;
  }

  static constexpr auto size() { return N; }
  static constexpr auto len() { return size() - 1; }

  CharT _data[N];
};

template <typename T>
  requires std::is_integral_v<T>
constexpr std::size_t integralLength(T val) {
  std::size_t r{1U};
  while (T{} != (val /= T{10}))
    r++;
  return r;
}

template <typename T, T val>
  requires std::is_integral_v<T>
constexpr auto parseIntToFixedString() {
  constexpr auto N = integralLength(val);
  auto s = FixedString<char, N + 1>();
  auto v = val;
  for (auto i = 0; i < N; i++) {
    auto n = v % T{10};
    s._data[N - 1 - i] = '0' + n;
    v /= T{10};
  }
  s._data[N] = '\0';
  return s;
}

template <typename... Strings>
constexpr auto concatStrings(Strings... strings) {
  return (strings + ...);
}
}  // namespace Sparrow

#endif
