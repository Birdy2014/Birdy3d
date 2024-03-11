#include "utils/Unicode.hpp"

#include <cstdint>

namespace Birdy3d::utils {

    const uint8_t ut_f8_one_byte_mask = 0b10000000;
    const uint8_t ut_f8_one_byte_count = 0;

    const uint8_t ut_f8_two_byte_mask = 0b11100000;
    const uint8_t ut_f8_two_byte_count = 0b11000000;

    const uint8_t ut_f8_three_byte_mask = 0b11110000;
    const uint8_t ut_f8_three_byte_count = 0b11100000;

    const uint8_t ut_f8_four_byte_mask = 0b11111000;

    const uint8_t ut_f8_other_mask = 0b00111111;

    size_t utf8_codepoint_size(uint8_t text)
    {
        if ((text & ut_f8_one_byte_mask) == ut_f8_one_byte_count) {
            return 1;
        }

        if ((text & ut_f8_two_byte_mask) == ut_f8_two_byte_count) {
            return 2;
        }

        if ((text & ut_f8_three_byte_mask) == ut_f8_three_byte_count) {
            return 3;
        }

        return 4;
    }

    size_t utf8_strlen(std::string text)
    {
        size_t i = 0;
        size_t num_chars = 0;

        while (i < text.size()) {
            num_chars++;

            i += utf8_codepoint_size(text[i]);
        }

        return num_chars;
    }

    std::u32string Unicode::utf8_to_utf32(std::string const& input)
    {
        std::u32string output;
        size_t num_chars = utf8_strlen(input);
        size_t i = 0;

        for (size_t n = 0; n < num_chars; n++) {
            size_t byte_count = utf8_codepoint_size(input[i]);

            switch (byte_count) {
            case 1:
                output.append(1, input[i] & ~ut_f8_one_byte_mask);
                break;
            case 2:
                output.append(1, (input[i] & ~ut_f8_two_byte_mask) << 6 | (input[i + 1] & ut_f8_other_mask));
                break;
            case 3:
                output.append(1, (input[i] & ~ut_f8_three_byte_mask) << 12 | (input[i + 1] & ut_f8_other_mask) << 6 | (input[i + 2] & ut_f8_other_mask));
                break;
            case 4:
                output.append(1, (input[i] & ~ut_f8_four_byte_mask) << 18 | (input[i + 1] & ut_f8_other_mask) << 12 | (input[i + 2] & ut_f8_other_mask) << 6 | (input[i + 3] & ut_f8_other_mask));
                break;
            }

            i += byte_count;
        }

        return output;
    }

    std::string Unicode::utf32_to_utf8(std::u32string const& input)
    {
        std::string output;
        for (char32_t const input_char : input) {
            if (input_char <= 0x7F) {
                output.append(1, input_char);
            } else if (input_char <= 0x7FF) {
                output.append(1, 0xC0 | (input_char >> 6)); /* 110xxxxx */
                output.append(1, 0x80 | (input_char & 0x3F)); /* 10xxxxxx */
            } else if (input_char <= 0xFFFF) {
                output.append(1, 0xE0 | (input_char >> 12)); /* 1110xxxx */
                output.append(1, 0x80 | ((input_char >> 6) & 0x3F)); /* 10xxxxxx */
                output.append(1, 0x80 | (input_char & 0x3F)); /* 10xxxxxx */
            } else if (input_char <= 0x10FFFF) {
                output.append(1, 0xF0 | (input_char >> 18)); /* 11110xxx */
                output.append(1, 0x80 | ((input_char >> 12) & 0x3F)); /* 10xxxxxx */
                output.append(1, 0x80 | ((input_char >> 6) & 0x3F)); /* 10xxxxxx */
                output.append(1, 0x80 | (input_char & 0x3F)); /* 10xxxxxx */
            }
        }
        return output;
    }

}
