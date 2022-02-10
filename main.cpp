#include <iostream>
#include <StringTools/Stringtools.h>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <random>

//! Generic uppercase character
constexpr char UPPERCASE = (1<<5);

//! Generic lowercase character
constexpr char LOWERCASE = 0;

//! Checks whether or not `c` is a vowel. You can define custom vowel characters.
bool IsVowel(const char c, const std::string& vowels = "euioay") {
    return std::any_of(
            vowels.cbegin(),
            vowels.cend(),
            [c](const char vowel) {
                    return c == vowel;
                }
        );
}

//! Checks whether or not `c` is an uppercase character
bool IsUpper(const char c) {
    return !(c & (1<<5));
}

//! Will return `c` as an uppercase character
char MakeUpper(char c) {
    if (IsUpper(c))
        return c;
    else
        return c & ~(1<<5);
}

//! Will return `c` as a lowercase character
char MakeLower(char c) {
    if (!IsUpper(c))
        return c;
    else
        return c | (1<<5);
}

//! Will return `c` with the same capitalization as `sign`.
char CopySign(char sign, char c) {
    if (IsUpper(sign))
        return MakeUpper(c);
    else
        return MakeLower(c);
}

//! Will replace all occurrences of a substring `find` in `str` with `sub`, but it will try to keep the characters signs.
//! Like (pay attention to the capitalization):.
//! ("Hello World", "hello", "hi") -> "Hi World".
//! ("hello World", "hello", "hi") -> "hi World".
//! ("HELLO World", "hello", "hi") -> "HI World".
//! You can also supply a callback to only perform a replacement if certain conditions apply.
std::string ReplaceButKeepSigns(
        const std::string& str,
        std::string find,
        const std::string& sub,
        const std::function<bool(const std::string&, const std::size_t)>& onlyIf = [](const std::string &,
                                                                                      const std::size_t) { return true; } // Default is: replace always
) {

    // Quick accepts-and rejects
    if (str.length() == 0)
        return "";
    else if (find.length() == 0)
        return str;

    std::stringstream ss;

    // Better safe than sorry
    find = StringTools::Lower(find);

    for (std::size_t i = 0; i < str.length(); i++)
    {
        const std::string foundInText = str.substr(i, find.length());
        const std::string foundInText_lower = StringTools::Lower(foundInText);
        if (foundInText_lower == find)
        {
            // Ask the callback if we should replace this one
            if (onlyIf(foundInText, i))
            {
                // Here we've found our occurrence...
                // We have three possible cases:
                // 1: len(find) == len(sub), in this case we want to sync capitalization by index.
                // 2: len(find) < len(sub), in this case we sync by index, BUT...
                // 3: len(find) > len(sub): sync capitalization by index

                // We want to sync capitalization by index
                // This accounts for both cases: 1 and 3
                if (foundInText.length() >= sub.length())
                {
                    for (std::size_t j = 0; j < sub.length(); j++)
                    {
                        const char cf = foundInText[j];
                        const char cs = sub[j];

                        ss << CopySign(cf, cs);
                    }
                }

                    // in this case we sync by index, BUT...
                else if (foundInText.length() < sub.length())
                {
                    char followingCharsSign = 0;
                    bool doHaveFollowingChar = false;
                    // Do we even have a following char?
                    if (str.length() >= i + foundInText.length() + 1)
                    {
                        const char followingChar = str[i + foundInText.length()];

                        // Is it a letter?
                        if ((followingChar >= 'a' && followingChar <= 'z') ||
                            (followingChar >= 'A' && followingChar <= 'Z'))
                        {
                            // Copy its sign
                            followingCharsSign = followingChar & (1 << 5);
                            doHaveFollowingChar = true;
                        }
                    }


                    {
                        char lastCharCharSign = 0;

                        for (std::size_t j = 0; j < sub.length(); j++)
                        {
                            const char cs = sub[j];

                            // Do we still have chars of 'find' left?
                            if (j < foundInText.length()) {
                                const char cf = foundInText[j];
                                lastCharCharSign = cf & (1 << 5);
                            }

                            const char charSignToUse = doHaveFollowingChar ? followingCharsSign : lastCharCharSign;
                            ss << CopySign(charSignToUse, cs);
                        }
                    }
                }
            }
            else
            {
                // We do not have an occurrence... just insert the char as is
                ss << str[i];
            }

            // Advance i accordingly
            i += foundInText.length()-1;
        }
        else
        {
            // We do not have an occurrence... just insert the char as is
            ss << str[i];
        }
    }

    return ss.str();
}

std::string MakeUwu(std::string boringString) {
    // Easy ones first
    // none, lol

    // Slightly more complex... Multichar replacements, but we have to keep capitalization...
    boringString = ReplaceButKeepSigns(boringString, "th", "tw");
    boringString = ReplaceButKeepSigns(boringString, "ove", "uv");
    boringString = ReplaceButKeepSigns(boringString, "have", "haf");
    boringString = ReplaceButKeepSigns(boringString, "tr", "tw");
    boringString = ReplaceButKeepSigns(boringString, "up", "uwp");

    // Replace N with Ny, but only if succeeded by a
    boringString = ReplaceButKeepSigns(
            boringString,
            "n",
            "ny",
            [boringString](const std::string &found, int index) {
                // Don't replace, if we are on the last char
                if (index == boringString.length() - 1)
                    return false;

                // Only replace if the next char is a vowel
                const char nextChar = MakeLower(boringString[index + found.length()]);

                // Is this a vowel?
                if (IsVowel(nextChar))
                    return true;

                // Else, don't replace
                return false;
            }
    );

    // Replace R with W, but only if not succeeded by a non-vowel
    boringString = ReplaceButKeepSigns(
            boringString,
            "r",
            "w",
            [boringString](const std::string &found, const std::size_t index) {
                // Don't replace, if we are on the last char
                if (index == boringString.length())
                    return false;

                // Only replace if the next char is a vowel
                const char nextChar = MakeLower(boringString[index + found.length()]);

                // Is this a non-vowel?
                if (!IsVowel(nextChar))
                    return false;

                // Else, replace
                return true;
            }
    );

    // Replace L with W, but only if not followed or preceded by another L
    boringString = ReplaceButKeepSigns(
            boringString,
            "l",
            "w",
            [boringString](const std::string &found, int index) {
                if (boringString.length() < found.length() + 2)
                    return false;

                const char lastChar = MakeLower(boringString[index - 1]);
                const char nextChar = MakeLower(boringString[index + found.length()]);

                return (lastChar != 'l') && (nextChar != 'l');
            }
    );

    // Replace LL with WW, but only if followed by a vowel
    boringString = ReplaceButKeepSigns(
            boringString,
            "ll",
            "ww",
            [boringString](const std::string &found, int index) {
                if (boringString.length() < found.length())
                    return false;

                const char nextChar = MakeLower(boringString[index + found.length()]);

                return IsVowel(nextChar);
            }
    );

    // Replace random punctuation with uwwwwu cute symbols
    // About evewy fifteenth symbol
    std::stringstream ss;
    std::mt19937 rng(69);
    for (const char c : boringString)
    {
        if ((c == '.') && (rng() % 15 == 0))
        {
            ss << " <3333 ^.^ ";
        }
        else if ((c == '!') && (rng() % 15 == 0))
        {
            ss << "!! Thadws impowtant! <3 ";
        }
        else if ((c == ',') && (rng() % 15 == 0))
        {
            ss << " <3 aaaaaand ";
        }
        else if ((c == '?') && (rng() % 15 == 0))
        {
            ss << "?? now tell me! >:( ";
        }
        else
            ss << c;
    }
    boringString = ss.str();

    // Also replace some ascii-"emojis'
    boringString = StringTools::Replace(boringString, ":)", "UwU :D");
    boringString = StringTools::Replace(boringString, ":-)", "UwwwU :D");
    boringString = StringTools::Replace(boringString, "lol", "XDD");
    boringString = StringTools::Replace(boringString, "^^", "^.^ UwU");
    boringString = StringTools::Replace(boringString, "thank you", "youwe twe best <3333 xoxo");
    boringString = StringTools::Replace(boringString, "thanks", "youwe twe best :D xoxo");
    boringString = StringTools::Replace(boringString, "thank's", "youwe twe best <3 uwu");
    boringString = StringTools::Replace(boringString, "hello", "Hiiiiiii");
    boringString = StringTools::Replace(boringString, "c++", "c++ (rust is hella cutewr btw ^^)");
    boringString = StringTools::Replace(boringString, "C++", "C++ (rust is hella cutewr btw ^^)");

    return boringString;
}

int main(int argc, char** argv) {

    // We have arguments. Uwwuifie these instead
    if (argc > 1)
    {
        for (std::size_t i = 1; i < argc; i++)
            std::cout << MakeUwu(argv[i]) + " ";

        std::cout << std::endl;
    }

    // Else, be prepared to get __piped__
    else
    {
        std::string buf;
        while (std::getline(std::cin, buf))
            std::cout << MakeUwu(buf) << std::endl;
    }

    return 0;
}
