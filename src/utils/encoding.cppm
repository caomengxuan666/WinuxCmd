/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: encoding.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
module;
#include <cstdint>
export module utils:encoding;

import std;

export namespace encoding {

// ===== Base64 =====

namespace base64_detail {
// Base64 encoding table
constexpr char ENCODE_TABLE[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// Base64 decoding table (compile-time initialized)
constexpr int8_t DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1,
    -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1};
}  // namespace base64_detail

/**
 * @brief Encode data to base64
 * @param data Input data to encode
 * @param wrap Line wrap width (0 for no wrapping)
 * @return Base64 encoded string
 */
inline size_t wrapped_encoded_size(size_t encoded_size, int wrap) {
  if (encoded_size == 0 || wrap <= 0) return encoded_size;
  return encoded_size + ((encoded_size - 1) / static_cast<size_t>(wrap));
}

/**
 * @brief Encode data to base64 with a caller-provided alphabet
 * @param data Input data to encode
 * @param alphabet 64-character encoding alphabet
 * @param wrap Line wrap width (0 for no wrapping)
 * @return Base64 encoded string
 */
inline std::string base64_encode(std::span<const uint8_t> data,
                                 std::string_view alphabet, int wrap = 0) {
  const size_t encoded_size = ((data.size() + 2) / 3) * 4;
  const size_t wrap_width = wrap > 0 ? static_cast<size_t>(wrap) : 0;
  const char* alpha = alphabet.data();
  std::string result(wrapped_encoded_size(encoded_size, wrap), '\0');
  size_t out = 0;
  size_t column = 0;

  const size_t full_size = (data.size() / 3) * 3;
  if (wrap_width == 0) {
    for (size_t i = 0; i < full_size; i += 3) {
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      result[out++] = alpha[b0 >> 2];
      result[out++] = alpha[((b0 & 0x03) << 4) | (b1 >> 4)];
      result[out++] = alpha[((b1 & 0x0f) << 2) | (b2 >> 6)];
      result[out++] = alpha[b2 & 0x3f];
    }
  } else if ((wrap_width % 4) == 0) {
    for (size_t i = 0; i < full_size; i += 3) {
      if (column == wrap_width) {
        result[out++] = '\n';
        column = 0;
      }
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      result[out++] = alpha[b0 >> 2];
      result[out++] = alpha[((b0 & 0x03) << 4) | (b1 >> 4)];
      result[out++] = alpha[((b1 & 0x0f) << 2) | (b2 >> 6)];
      result[out++] = alpha[b2 & 0x3f];
      column += 4;
    }
  } else {
    for (size_t i = 0; i < full_size; i += 3) {
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      char quartet[4] = {
          alpha[b0 >> 2],
          alpha[((b0 & 0x03) << 4) | (b1 >> 4)],
          alpha[((b1 & 0x0f) << 2) | (b2 >> 6)],
          alpha[b2 & 0x3f],
      };
      for (char c : quartet) {
        if (column == wrap_width) {
          result[out++] = '\n';
          column = 0;
        }
        result[out++] = c;
        ++column;
      }
    }
  }

  const size_t remaining = data.size() - full_size;
  if (remaining > 0) {
    if (wrap_width > 0 && column == wrap_width) {
      result[out++] = '\n';
      column = 0;
    }
    const uint8_t b0 = data[full_size];
    const uint8_t b1 = remaining == 2 ? data[full_size + 1] : 0;
    result[out++] = alpha[b0 >> 2];
    result[out++] = alpha[((b0 & 0x03) << 4) | (b1 >> 4)];
    result[out++] = remaining == 2 ? alpha[(b1 & 0x0f) << 2] : '=';
    result[out++] = '=';
  }

  result.resize(out);
  return result;
}
inline std::string base64_encode(std::span<const uint8_t> data, int wrap = 0) {
  return base64_encode(data, std::string_view(base64_detail::ENCODE_TABLE, 64),
                       wrap);
}

/**
 * @brief Decode base64 to data
 * @param encoded Base64 encoded string
 * @param ignore_garbage Ignore non-base64 characters
 * @return Decoded data, or empty vector on error
 */
inline std::vector<uint8_t> base64_decode(std::string_view encoded,
                                          bool ignore_garbage = false) {
  std::vector<uint8_t> result;
  result.reserve((encoded.size() / 4) * 3);

  uint32_t triple = 0;
  int bits = 0;

  for (char c : encoded) {
    if (c == '=') break;
    if (c == '\n' || c == '\r') continue;

    int8_t value = base64_detail::DECODE_TABLE[static_cast<uint8_t>(c)];
    if (value < 0) {
      if (!ignore_garbage) {
        return {};  // Return empty vector on error
      }
      continue;
    }

    triple = (triple << 6) | static_cast<uint32_t>(value);
    bits += 6;

    if (bits >= 8) {
      bits -= 8;
      result.push_back(static_cast<uint8_t>((triple >> bits) & 0xFF));
    }
  }

  return result;
}

// ===== Base32 =====

namespace base32_detail {
// Base32 alphabet (RFC 4648)
constexpr char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Base32 decode table
constexpr signed char DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1};
}  // namespace base32_detail

/**
 * @brief Encode data to base32 (RFC 4648)
 * @param data Input data to encode
 * @param wrap Line wrap width (0 for no wrapping)
 * @return Base32 encoded string
 */
inline std::string base32_encode(std::span<const uint8_t> data,
                                 std::string_view alphabet, int wrap = 0) {
  const size_t encoded_size = ((data.size() + 4) / 5) * 8;
  const size_t wrap_width = wrap > 0 ? static_cast<size_t>(wrap) : 0;
  const char* alpha = alphabet.data();
  const char pad = char{61};
  const char nl = char{10};
  std::string result(wrapped_encoded_size(encoded_size, wrap), char{0});
  size_t out = 0;
  size_t column = 0;

  const size_t full_size = (data.size() / 5) * 5;
  if (wrap_width == 0) {
    for (size_t i = 0; i < full_size; i += 5) {
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      const uint8_t b3 = data[i + 3];
      const uint8_t b4 = data[i + 4];
      result[out++] = alpha[(b0 >> 3) & 0x1f];
      result[out++] = alpha[((b0 & 0x07) << 2) | (b1 >> 6)];
      result[out++] = alpha[(b1 >> 1) & 0x1f];
      result[out++] = alpha[((b1 & 0x01) << 4) | (b2 >> 4)];
      result[out++] = alpha[((b2 & 0x0f) << 1) | (b3 >> 7)];
      result[out++] = alpha[(b3 >> 2) & 0x1f];
      result[out++] = alpha[((b3 & 0x03) << 3) | (b4 >> 5)];
      result[out++] = alpha[b4 & 0x1f];
    }
  } else if ((wrap_width % 8) == 0) {
    for (size_t i = 0; i < full_size; i += 5) {
      if (column == wrap_width) {
        result[out++] = nl;
        column = 0;
      }
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      const uint8_t b3 = data[i + 3];
      const uint8_t b4 = data[i + 4];
      result[out++] = alpha[(b0 >> 3) & 0x1f];
      result[out++] = alpha[((b0 & 0x07) << 2) | (b1 >> 6)];
      result[out++] = alpha[(b1 >> 1) & 0x1f];
      result[out++] = alpha[((b1 & 0x01) << 4) | (b2 >> 4)];
      result[out++] = alpha[((b2 & 0x0f) << 1) | (b3 >> 7)];
      result[out++] = alpha[(b3 >> 2) & 0x1f];
      result[out++] = alpha[((b3 & 0x03) << 3) | (b4 >> 5)];
      result[out++] = alpha[b4 & 0x1f];
      column += 8;
    }
  } else {
    for (size_t i = 0; i < full_size; i += 5) {
      const uint8_t b0 = data[i];
      const uint8_t b1 = data[i + 1];
      const uint8_t b2 = data[i + 2];
      const uint8_t b3 = data[i + 3];
      const uint8_t b4 = data[i + 4];
      char octet[8] = {
          alpha[(b0 >> 3) & 0x1f],
          alpha[((b0 & 0x07) << 2) | (b1 >> 6)],
          alpha[(b1 >> 1) & 0x1f],
          alpha[((b1 & 0x01) << 4) | (b2 >> 4)],
          alpha[((b2 & 0x0f) << 1) | (b3 >> 7)],
          alpha[(b3 >> 2) & 0x1f],
          alpha[((b3 & 0x03) << 3) | (b4 >> 5)],
          alpha[b4 & 0x1f],
      };
      for (char c : octet) {
        if (column == wrap_width) {
          result[out++] = nl;
          column = 0;
        }
        result[out++] = c;
        ++column;
      }
    }
  }

  const size_t remaining = data.size() - full_size;
  if (remaining > 0) {
    if (wrap_width > 0 && column == wrap_width) {
      result[out++] = nl;
      column = 0;
    }
    const uint8_t b0 = data[full_size];
    const uint8_t b1 = remaining > 1 ? data[full_size + 1] : 0;
    const uint8_t b2 = remaining > 2 ? data[full_size + 2] : 0;
    const uint8_t b3 = remaining > 3 ? data[full_size + 3] : 0;
    char octet[8] = {
        alpha[(b0 >> 3) & 0x1f],
        alpha[((b0 & 0x07) << 2) | (b1 >> 6)],
        remaining > 1 ? alpha[(b1 >> 1) & 0x1f] : pad,
        remaining > 1 ? alpha[((b1 & 0x01) << 4) | (b2 >> 4)] : pad,
        remaining > 2 ? alpha[((b2 & 0x0f) << 1) | (b3 >> 7)] : pad,
        remaining > 3 ? alpha[(b3 >> 2) & 0x1f] : pad,
        remaining > 3 ? alpha[(b3 & 0x03) << 3] : pad,
        pad,
    };
    if (wrap_width == 0 || (wrap_width % 8) == 0) {
      for (char c : octet) result[out++] = c;
    } else {
      for (char c : octet) {
        if (column == wrap_width) {
          result[out++] = nl;
          column = 0;
        }
        result[out++] = c;
        ++column;
      }
    }
  }

  result.resize(out);
  return result;
}

inline std::string base32_encode(std::span<const uint8_t> data, int wrap = 0) {
  return base32_encode(data, std::string_view(base32_detail::ALPHABET, 32),
                       wrap);
}
/**
 * @brief Decode base32 to data
 * @param encoded Base32 encoded string
 * @return Decoded data, or empty vector on error
 */
inline std::vector<uint8_t> base32_decode(std::string_view encoded) {
  std::vector<uint8_t> result;
  uint8_t buffer[8] = {0};
  size_t buffer_pos = 0;
  size_t padding_count = 0;

  for (char c : encoded) {
    if (c == '=') {
      padding_count++;
      if (padding_count > 6) break;
      buffer[buffer_pos++] = 0;
    } else if (std::isalnum(static_cast<unsigned char>(c))) {
      signed char value = base32_detail::DECODE_TABLE[static_cast<uint8_t>(
          std::toupper(static_cast<unsigned char>(c)))];
      if (value < 0) {
        return {};  // Invalid character
      }
      buffer[buffer_pos++] = static_cast<uint8_t>(value);
    } else if (c != '\n' && c != '\r' && c != ' ') {
      return {};  // Invalid character
    }

    if (buffer_pos == 8) {
      result.push_back((buffer[0] << 3) | (buffer[1] >> 2));
      result.push_back(((buffer[1] & 0x03) << 6) | (buffer[2] << 1) |
                       (buffer[3] >> 4));
      result.push_back(((buffer[3] & 0x0F) << 4) | (buffer[4] >> 1));
      result.push_back(((buffer[4] & 0x01) << 7) | (buffer[5] << 2) |
                       (buffer[6] >> 3));
      result.push_back(((buffer[6] & 0x07) << 5) | buffer[7]);
      buffer_pos = 0;
    }
  }

  // Handle partial buffer
  if (buffer_pos > 0 && buffer_pos >= 2) {
    result.push_back((buffer[0] << 3) | (buffer[1] >> 2));
  }
  if (buffer_pos > 0 && buffer_pos >= 4) {
    result.push_back(((buffer[1] & 0x03) << 6) | (buffer[2] << 1) |
                     (buffer[3] >> 4));
  }
  if (buffer_pos > 0 && buffer_pos >= 5) {
    result.push_back(((buffer[3] & 0x0F) << 4) | (buffer[4] >> 1));
  }
  if (buffer_pos > 0 && buffer_pos >= 7) {
    result.push_back(((buffer[4] & 0x01) << 7) | (buffer[5] << 2) |
                     (buffer[6] >> 3));
  }

  return result;
}

// ===== Base16 (Hex) =====

namespace base16_detail {
constexpr char ALPHABET_LOWER[] = "0123456789abcdef";
constexpr char ALPHABET_UPPER[] = "0123456789ABCDEF";
}  // namespace base16_detail

/**
 * @brief Encode data to base16 (hexadecimal)
 * @param data Input data to encode
 * @param uppercase Use uppercase letters
 * @return Hex encoded string
 */
inline std::string base16_encode(std::span<const uint8_t> data,
                                 bool uppercase = false) {
  std::string result;
  result.reserve(data.size() * 2);

  const char* alphabet =
      uppercase ? base16_detail::ALPHABET_UPPER : base16_detail::ALPHABET_LOWER;

  for (uint8_t byte : data) {
    result += alphabet[byte >> 4];
    result += alphabet[byte & 0x0F];
  }

  return result;
}

/**
 * @brief Decode base16 (hexadecimal) to data
 * @param encoded Hex encoded string
 * @return Decoded data, or empty vector on error
 */
inline std::vector<uint8_t> base16_decode(std::string_view encoded) {
  std::vector<uint8_t> result;
  result.reserve(encoded.size() / 2);

  uint8_t high_nibble = 0;
  bool has_high_nibble = false;

  for (char c : encoded) {
    uint8_t value = 0;

    if (c >= '0' && c <= '9') {
      value = c - '0';
    } else if (c >= 'A' && c <= 'F') {
      value = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
      value = c - 'a' + 10;
    } else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
      continue;  // Skip whitespace
    } else {
      return {};  // Invalid character
    }

    if (!has_high_nibble) {
      high_nibble = value;
      has_high_nibble = true;
    } else {
      result.push_back((high_nibble << 4) | value);
      has_high_nibble = false;
    }
  }

  return result;
}

}  // namespace encoding
