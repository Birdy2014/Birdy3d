#include "utils/Unicode.hpp"

namespace Birdy3d {

    const uint8_t UTF8_ONE_BYTE_MASK = 0b10000000;
    const uint8_t UTF8_ONE_BYTE_COUNT = 0;

    const uint8_t UTF8_TWO_BYTE_MASK = 0b11100000;
    const uint8_t UTF8_TWO_BYTE_COUNT = 0b11000000;

    const uint8_t UTF8_THREE_BYTE_MASK = 0b11110000;
    const uint8_t UTF8_THREE_BYTE_COUNT = 0b11100000;

    const uint8_t UTF8_FOUR_BYTE_MASK = 0b11111000;

    const uint8_t UTF8_OTHER_MASK = 0b00111111;

    size_t utf8_codepoint_size(uint8_t text) {
        if ((text & UTF8_ONE_BYTE_MASK) == UTF8_ONE_BYTE_COUNT) {
            return 1;
        }

        if ((text & UTF8_TWO_BYTE_MASK) == UTF8_TWO_BYTE_COUNT) {
            return 2;
        }

        if ((text & UTF8_THREE_BYTE_MASK) == UTF8_THREE_BYTE_COUNT) {
            return 3;
        }

        return 4;
    }

    size_t utf8_strlen(std::string text) {
        size_t i = 0;
        size_t num_chars = 0;

        while (i < text.size()) {
            num_chars++;

            i += utf8_codepoint_size(text[i]);
        }

        return num_chars;
    }

    std::u32string Unicode::utf8_to_utf32(const std::string& input) {
        std::u32string output;
        size_t num_chars = utf8_strlen(input);
        size_t i = 0;

        for (size_t n = 0; n < num_chars; n++) {
            size_t byte_count = utf8_codepoint_size(input[i]);

            switch (byte_count) {
            case 1:
                output.append(1, input[i] & ~UTF8_ONE_BYTE_MASK);
                break;
            case 2:
                output.append(1, (input[i] & ~UTF8_TWO_BYTE_MASK) << 6 | (input[i + 1] & UTF8_OTHER_MASK));
                break;
            case 3:
                output.append(1, (input[i] & ~UTF8_THREE_BYTE_MASK) << 12 | (input[i + 1] & UTF8_OTHER_MASK) << 6 | (input[i + 2] & UTF8_OTHER_MASK));
                break;
            case 4:
                output.append(1, (input[i] & ~UTF8_FOUR_BYTE_MASK) << 18 | (input[i + 1] & UTF8_OTHER_MASK) << 12 | (input[i + 2] & UTF8_OTHER_MASK) << 6 | (input[i + 3] & UTF8_OTHER_MASK));
                break;
            }

            i += byte_count;
        }

        return output;
    }

    std::string Unicode::utf32_to_utf8(const std::u32string& input) {
        std::string output;
        for (const char32_t input_char : input) {
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
