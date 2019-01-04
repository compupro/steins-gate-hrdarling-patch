///////////////////////////////////////////////////////////////
//                                                           //
//   Types.h                                                 //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_TYPES_H
#define TOPOGRAPHER_TYPES_H

#include <vector>
#include <string>

namespace topographer
{
    // Specific type for byte arrays, since they are used instead
    //   of the usual string type.
    typedef std::vector<uint8_t> BArr;

    // Number of bits per pointer, can be either 16 or 32. It is
    //   adjusted automatically in order to generate proper Atlas
    //   scripts with the correct pointer size.
    enum PointerSz
    {
        psz16b = 16,
        psz32b = 32,
    };

    // The encoding rule is any line in the encoding table. For
    //   example the rule "1337=<NICE>".
    //   szOriginal, size_t, size of the original byte sequence
    //   szReplacement, size_t, size of the replacement string
    //   original, BArr, original byte sequence to look for ("0x13 0x37")
    //   replacement, BArr, string to replace original byte
    //     sequence with ("<LEET>")
    struct EncodingRule
    {
        size_t szOriginal;
        size_t szReplacement;
        BArr original;
        BArr replacement;
    };

    // A string in the game. Can be used for any language. For
    //   example the string "text<END>".
    //   szData, size_t, size of the string data
    //   szEnding, size_t, size of the string ending
    //   data, BArr, the data of the string ("text" in byte array form)
    //   ending, BArr, the ending of the string ("<END>" in byte array form)
    struct GameString
    {
        size_t szPre;
        size_t szData;
        size_t szPost;
        BArr pre;
        BArr data;
        BArr post;
        bool __flag = false;
    };

    // An index that stores the position of the key string
    //   when mapping key strings and inner strings. A key string
    //   is any string that starts with '【' (basically a dialog). An
    //   inner string is any other string between current key and the
    //   next key. Used mainly in the StringList class.
    //   index, size_t, the index of the key string in all the strings
    //   numInnerStrings, uint8_t, number of inner strings of the key
    struct KeyIndex
    {
        size_t index = 0;
        uint8_t numInnerStrings = 0;
    };

    // Pointer data that is used when mapping strings and
    //   writing Atlas scripts.
    //   address, uint32_t, the actual address of the pointer. The
    //     writing location in the Atlas script
    //   stringAddress, uint32_t, the address of the beginning of
    //     the string this pointer normally (in the Japanese game)
    //     associates with
    struct Pointer
    {
        uint32_t address;
        uint32_t stringAddress;
    };

    // Names of files that are required to be processed
    const std::vector<std::string> BIN_FILES = {
        "0","3","6","7","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59","60","61","62","63","64","65","66","67","68","69","70","71","72","73","74","75","76","77","78","79","80","81","82","83","84","85","86","87"
    };
}

#endif
