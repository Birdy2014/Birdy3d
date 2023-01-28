#include "common.hpp"

#include "ui/Text.hpp"

using namespace Birdy3d::ui;

TEST_CASE("set")
{
    SUBCASE("append single line")
    {
        TextDescription text{};
        text.append_u32(U"Hello\n");
        CHECK_EQ(text.lines().size(), 1);
        CHECK_EQ(text.lines()[0], U"Hello");
    }

    SUBCASE("append multiple lines")
    {
        TextDescription text{};
        text.append_u32(U"Hello\nWorld\n");
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Hello");
        CHECK_EQ(text.lines()[1], U"World");
    }

    SUBCASE("append multiple partial lines")
    {
        TextDescription text{};
        text.append_u32(U"Hello");
        CHECK_EQ(text.lines().size(), 1);
        CHECK_EQ(text.lines()[0], U"Hello");
        text.append_u32(U"World");
        CHECK_EQ(text.lines().size(), 1);
        CHECK_EQ(text.lines()[0], U"HelloWorld");
    }

    SUBCASE("setter")
    {
        TextDescription text{U"Hello\nWorld"};
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Hello");
        CHECK_EQ(text.lines()[1], U"World");
    }

    SUBCASE("insert at end of line")
    {
        TextDescription text{U"Hello\nWorld"};
        text.insert_u32({.row = 0, .column = 5}, U"abc");
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Helloabc");
        CHECK_EQ(text.lines()[1], U"World");
    }

    SUBCASE("insert in new line")
    {
        TextDescription text{U"Hello\nWorld\n"};
        text.insert_u32({.row = 2, .column = 0}, U"abc");
        CHECK_EQ(text.lines().size(), 3);
        CHECK_EQ(text.lines()[0], U"Hello");
        CHECK_EQ(text.lines()[1], U"World");
        CHECK_EQ(text.lines()[2], U"abc");
    }

    SUBCASE("insert in line")
    {
        TextDescription text{U"Hello\nWorld\n"};
        text.insert_u32({.row = 0, .column = 1}, U"abc");
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Habcello");
        CHECK_EQ(text.lines()[1], U"World");
    }
}

TEST_CASE("erase")
{
    SUBCASE("erase_char")
    {
        TextDescription text{U"Hello\nWorld"};
        text.erase_char(TextDescription::Position{.row = 0, .column = 1});
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Hllo");
        CHECK_EQ(text.lines()[1], U"World");
    }

    SUBCASE("erase_char middle newline")
    {
        TextDescription text{U"Hello\nWorld"};
        text.erase_char(TextDescription::Position{.row = 0, .column = 5});
        CHECK_EQ(text.lines().size(), 1);
        CHECK_EQ(text.lines()[0], U"HelloWorld");
    }

    SUBCASE("erase in single line")
    {
        TextDescription text{U"Hello\nWorld\nabc"};
        text.erase(TextDescription::Position{.row = 0, .column = 1}, TextDescription::Position{.row = 0, .column = 3});
        CHECK_EQ(text.lines().size(), 3);
        CHECK_EQ(text.lines()[0], U"Ho");
        CHECK_EQ(text.lines()[1], U"World");
        CHECK_EQ(text.lines()[2], U"abc");
    }

    SUBCASE("erase in multiple lines")
    {
        TextDescription text{U"Hello\nWorld\nabc"};
        text.erase(TextDescription::Position{.row = 0, .column = 1}, TextDescription::Position{.row = 1, .column = 1});
        CHECK_EQ(text.lines().size(), 2);
        CHECK_EQ(text.lines()[0], U"Hrld");
        CHECK_EQ(text.lines()[1], U"abc");
    }

    SUBCASE("erase all lines without newline")
    {
        TextDescription text{U"Hallo Welt\nHallo Welt\naaaaaaaa\nbbbbbbb\ncccccc\ndddddd"};
        text.erase(TextDescription::Position{.row = 0, .column = 0}, TextDescription::Position{.row = 5, .column = 5});
        CHECK_EQ(text.lines().size(), 0);
    }

    SUBCASE("erase all lines with newline")
    {
        TextDescription text{U"Hallo Welt\nHallo Welt\naaaaaaaa\nbbbbbbb\ncccccc\ndddddd\n"};
        text.erase(TextDescription::Position{.row = 0, .column = 0}, TextDescription::Position{.row = 5, .column = 6});
        CHECK_EQ(text.lines().size(), 0);
    }
}
