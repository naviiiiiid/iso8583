#pragma once

#include <string>
#include <sstream>
#include <cassert>

#include <iostream>

namespace isolib
{
  enum class PaddingType : char { Left, Right, None };

  int fromHex(char c);
  std::string readFixedField(std::istringstream& iss, size_t length);
  std::string readVarField(std::istringstream& iss, size_t headerLength);
  size_t getNumberOfDigits(size_t number);
  void validateMessageType(const std::string& mt);
  std::string sendAndRecieve(const std::string& messgae , const std::string& ip ,  int port);
  std::string readFromFile(const std::string& pathFile);

  std::string pad_right(std::string const &str, size_t s, char ch);
  std::string pad_left(std::string const &str, size_t s, char ch);

  namespace detail
  {
    const char hexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    template <typename T, bool U>
    struct SetImpl {};

    template <typename T>
    struct SetImpl<T, true>
    {
      static T set(size_t i, T target)
      {
        // TODO make the proper type checkings
        constexpr auto sizeInBits = sizeof(uint64_t) * 8;
        uint32_t mask = 1 << ((sizeInBits - i) % 32);
        if (i > 32)
        {
            return (target | static_cast<uint64_t>(mask));
        }
        else
        {
          uint32_t upperHalf = (target >> 32) | mask;
          uint32_t lowerHalf = target & 0xFFFFFFFF;
          return (static_cast<uint64_t>(upperHalf) << 32) | static_cast<uint64_t>(lowerHalf);
        }
      }
    };

    template <typename T>
    struct SetImpl<T, false>
    {
      static T set(size_t i, T target)
      {
        // TODO make the proper type checkings
        constexpr auto sizeInBits = sizeof(T) * 8;
        std::cout << "sizeInBits "<<sizeInBits <<"\n" << (target | (1ULL << (sizeInBits - i)));
        return (target | (1ULL << (sizeInBits - i)));
      }
    };
  }

  template <typename T>
  T set(size_t i, T target)
  {
    using UIntType = typename std::make_unsigned<T>::type;
    constexpr bool is64 = std::is_same<UIntType, uint64_t>::value;
    return detail::SetImpl<T, is64>::set(i, target);
  }

  template <typename T>
  T clear(size_t i, T target)
  {
    constexpr auto sizeInBits = sizeof(T) * 8;

    static_assert(std::is_integral<T>::value, "The target has to be integral");
    assert(i <= sizeInBits && "The index cannot be bigger than the size in bits of the target");
    assert(i > 0 && "The minimum allowed value for the index is 1");

    uint64_t mask = ~(1ULL << (sizeInBits - i));
    return target & static_cast<T>(mask);
  }

  template <typename T>
  bool get(size_t i, T target)
  {
    constexpr auto sizeInBits = sizeof(T) * 8;
    //constexpr auto sizeInBits = sizeof(T) * 16;

    static_assert(std::is_integral<T>::value, "The target has to be integral");
    //assert(i <= sizeInBits && "The index cannot be bigger than the size in bits of the target");
    //assert(i > 0 && "The minimum allowed value for the index is 1");

    //std::cout << " ["<<i<<"]"<< (target >> (sizeInBits - i)) << "   "  <<1UL<<"\n";

    return (target >> (sizeInBits - i)) & 1UL;
  }

  template <typename T>
  std::string toHex(T target)
  {
    static_assert(std::is_integral<T>::value, "The target has to be integral");

    constexpr const size_t numChars = sizeof(T) * 2;
    unsigned char result[numChars];
    size_t currentChar = numChars - 1;

    for (size_t i = 0; i < numChars; i++)
    {
      result[currentChar--] = detail::hexChars[target & 0xF];
      target >>= 4;
    }

    return std::string(reinterpret_cast<const char*>(result), numChars);
  }

  template <typename T>
  std::string toBinary(T target)
  {
    static_assert(std::is_unsigned<T>::value, "The target has to be unsigned integral");
    constexpr const size_t numChars = sizeof(T);
    unsigned char result[numChars];
    size_t currentChar = numChars - 1;

    for (size_t i = 0; i < numChars; i++)
    {
      result[currentChar--] = target & 0xFF;

      // To avoid compiler warning in case the target type is 1 byte
      if (numChars == 1) break;

      target >>= 8;
    }

    return std::string(reinterpret_cast<const char*>(result), numChars);
  }

  template <typename T>
  T fromHex(const std::string& in)
  {
    static_assert(std::is_unsigned<T>::value, "The return value has to be unsigned integral");
    if (sizeof(T) < in.size() / 2)
      throw std::invalid_argument("Number of hex chars on the input cannot be larger than the size in bytes of the return type");
    if (in.size() % 2)
      throw std::invalid_argument("Hex string has to have an even length");
    if(in.find_first_not_of("1234567890ABCDEFabcdef") != std::string::npos)
      throw std::invalid_argument("Input contains invalid characters");

    T ret{0};
    const char* input = in.data();
    for (size_t i = 0; i < in.size(); i++)
    {
      ret = ret << 4;
      ret = ret | fromHex(input[i]);
    }
    return ret;
  }

  template <typename T>
  T fromBinary(const std::string& in)
  {
    static_assert(std::is_unsigned<T>::value, "The return value has to be unsigned integral");

    if (sizeof(T) < in.size())
      throw std::invalid_argument("Number of chars on the input cannot be larger than the size in bytes of the return type");

    T ret{0};
    const char* input = in.data();
    for (size_t i = 0; i < in.size(); i++)
    {
      ret = ret << 8;
      ret = ret | static_cast<uint8_t>(input[i]);
    }

    return ret;
  }
}
