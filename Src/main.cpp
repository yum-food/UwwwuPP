#include <iostream>
#include <StringTools.h>
#include <CharTools.h>
#include <string>
#include <sstream>
#include <functional>
#include <random>
#include "Util.h"

// This validator will only replace findings, if they are a complete word, and not just part of a word.
auto ValidatorFindingIsCompleteWord(const std::string& original, const std::string& finding, const std::size_t index) -> bool {
    // Quick-accept: Original-string length matches finding-string length
    if (original.length() == finding.length())
        return true;

    bool lastCharBreaksWord = true; // Default is true, because this value stays in case there is no last/next char.
    bool nextCharBreaksWord = true; // In this case, "no character" would imply the word to be broken.

    // Assign surrounding chars, if possible
    if (index > 0)
        lastCharBreaksWord = !CharTools::IsLetter(original[index - 1]);
    if (index + finding.length() < original.length())
        nextCharBreaksWord = !CharTools::IsLetter(original[index + finding.length()]);

    // If both the last and the next character are word-breaking, replace.
    if (lastCharBreaksWord && nextCharBreaksWord)
        return true;
    // Else: don't
    else
        return false;
}


//! Will make a boring string look sooper dooper kawaii and cute :3
std::string MakeUwu(std::string boringString) {
    // Easy ones first
    // none, lol

    // Slightly more complex... Multichar replacements, but we have to keep capitalization...
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "th", "tw");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "ove", "uv");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "have", "haf");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "tr", "tw");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "up", "uwp");

    // Let's do some language adjustments
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "twank you", "you're twe best <3333 xoxo", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "good", "sooper dooper", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "suwper", "sooper dooper", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "well", "sooper dooper", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "emacs", "vim", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "twanks", "you're twe best :33 xoxo", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "hello", "hiiiiiii", ValidatorFindingIsCompleteWord);
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "dear", "hiiiiiii", ValidatorFindingIsCompleteWord);

    // Let's extend some phonetics
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "hi", "hiiiiiii");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "ay", "aaay");
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "ey", "eeey");

    // Replace N with Ny, but only if succeeded by a vowel, and not (preceded by an o and succeeded by an "e{nonletter}"): "one" has such a niche pronunciation...
    boringString = Util::ConditionalReplaceButKeepSigns(
            boringString,
            "n",
            "ny",
            [](const std::string& original, const std::string& finding, const std::size_t index) {
                // Jävla this is one clustered piece of shitcode

                // Don't replace, if we are on the last char
                if (index + finding.length() == original.length())
                    return false;

                const char nextChar = CharTools::MakeLower(original[index + finding.length()]);
                const char lastChar = CharTools::MakeLower(original[index - 1]);

                // Apply the complex "one\b"-rule:
                // (don't replace if 'n' is preceded by 'o' and succeeded by 'e', which is succeeded by a word break)
                {
                    bool nextNextCharIsNotLetter = false;
                    char nextNextChar;

                    // How much length is left including `nextChar`?
                    const std::size_t sizeLeft = original.length() - (index + finding.length());

                    // We have room to pick the nextNext char...
                    if (sizeLeft > 1)
                    {
                        nextNextChar = CharTools::MakeLower(original[index + finding.length() + 1]);
                        nextNextCharIsNotLetter = !CharTools::IsLetter(nextNextChar);
                    }

                    const bool nextNextCharBreaksWord = (sizeLeft == 1) || (nextNextCharIsNotLetter);

                    // Don't replace if:
                    // (lastChar == o) && (nextChar == e) && (nextNextCharBreaksWord)
                    if ((lastChar == 'o') && (nextChar == 'e') && (nextNextCharBreaksWord))
                        return false;
                }

                // Is this a vowel?
                if (CharTools::IsVowel(nextChar))
                    return true;

                // Else, don't replace
                return false;
            }
    );

    // Replace R with W, but only if not succeeded by a non-vowel, and if it's not the first character of a word
    boringString = Util::ConditionalReplaceButKeepSigns(
            boringString,
            "r",
            "w",
            [](const std::string& original, const std::string& finding, const std::size_t index) {
                // Don't replace, if we are on the last char
                if (index + finding.length() == original.length())
                    return false;

                // Don't replace if we're at index 0
                if (index == 0)
                    return false;

                const char nextChar = CharTools::MakeLower(original[index + finding.length()]);
                const char lastChar = CharTools::MakeLower(original[index - 1]);

                // Is this a non-vowel?
                if (!CharTools::IsVowel(nextChar))
                    return false;

                // Don't replace if the last char is not a letter
                if (!CharTools::IsLetter(lastChar))
                    return false;

                // Else, replace
                return true;
            }
    );

    // Replace L with W, but only if not followed or preceded by another L, and if it's not the first character of a word
    boringString = Util::ConditionalReplaceButKeepSigns(
            boringString,
            "l",
            "w",
            [](const std::string& original, const std::string& finding, const std::size_t index) {
                // Our segment has to be at least two characters long
                if (original.length() < finding.length() + 2)
                    return false;

                // Don't replace if we're at index o
                if (index == 0)
                    return false;

                const char lastChar = CharTools::MakeLower(original[index - 1]);
                const char nextChar = CharTools::MakeLower(original[index + finding.length()]);

                // Don't replace if the last char is not a letter
                if (!CharTools::IsLetter(lastChar))
                    return false;

                return (lastChar != 'l') && (nextChar != 'l');
            }
    );

    // Replace LL with WW, but only if followed by a vowel
    boringString = Util::ConditionalReplaceButKeepSigns(
            boringString,
            "ll",
            "ww",
            [](const std::string& original, const std::string& finding, const std::size_t index) {
                // Don't replace, if we are on the last char
                if (index + finding.length() == original.length())
                    return false;

                const char nextChar = CharTools::MakeLower(original[index + finding.length()]);

                return CharTools::IsVowel(nextChar);
            }
    );

    // Replace ER with A, but only if it's the last two letters of a word
    boringString = Util::ConditionalReplaceButKeepSigns(
            boringString,
            "er",
            "a",
            [](const std::string& original, const std::string& finding, const std::size_t index) {
                // Replace if we're at the end of this line/segment
                if (index + finding.length() == original.length())
                    return true;

                // Fetch the next char
                const char nextChar = CharTools::MakeLower(original[index + finding.length()]);

                // Replace if the next char is not a letter
                return !CharTools::IsLetter(nextChar);
            }
    );

    // Replace random punctuation with uwwwwu cute symbols
    // About evewy fifteenth symbol
    std::stringstream ss;
    std::mt19937 rng(std::hash<std::string>()(boringString)); // Seed rng based on string
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
    boringString = StringTools::Replace(boringString, ":D", ":3");
    boringString = StringTools::Replace(boringString, ":-)", "UwwwU :3");
    boringString = StringTools::Replace(boringString, "^^", "^.^ UwU");

    // Some language replacement should happen after these more complex rules
    boringString = Util::ConditionalReplaceButKeepSigns(boringString, "c++", "c++ (rust is hella cutewr btw ^^)");


    return boringString;
}

int main(int argc, char** argv) {

    // We have arguments. Uwwuifie these instead
    if (argc > 1)
    {
        // We have to put the args together first, because some replace-rules cross word-borders
        std::stringstream  ss;
        for (std::size_t i = 1; i < argc; i++)
            ss << std::string(argv[i]) + " ";

        std::cout << MakeUwu(ss.str()) << std::endl;
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
