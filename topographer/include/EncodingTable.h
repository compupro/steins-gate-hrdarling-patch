///////////////////////////////////////////////////////////////
//                                                           //
//   EncodingTable.h                                         //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_ENCODING_TABLE_H
#define TOPOGRAPHER_ENCODING_TABLE_H

#include "Defines.h"
#include "Types.h"
#include "Utils.h"
#include <fstream>
#include <iostream>

namespace topographer
{
    // Responsible for loading and accessing all the encoding rules
    //   from the encoding.tbl file. Also provides functions to work
    //   with original game strings and their processing
    class EncodingTable
    {
    public:
        EncodingTable();
        ~EncodingTable();

        // Find an <END> control sequence (or a similar END) in the byte array
        //   input, BArr, the string in which to look for end sequence
        //   out, &BArr, the output of which control sequence was found
        // Returns: size_t, -1 if no end sequence was found, otherwise a
        //   position end sequence was found
        size_t FindEndControlSequence(const BArr & input, BArr & out);

        // Replaces all the characters in byte array with UTF8 alternative
        //   using the encoding rules
        //   bytes, BArrr, the string in which to replaces characters
        //   out, &BArr, the output string with replaces characters
        void Sanitize(const BArr & bytes, BArr & out);

    private:
        // All encoding rules
        std::vector<EncodingRule> _rules;

        // Loads encoding table
        void _Load();

        // Parses a rule string in a table
        //   str, string, a rule string to parse (eg: "1337=<NICE>")
        //   rule, &EncodingRule, an output encoding rule
        // Returns: bool, true if parsing was successful and the rule is valid,
        //   false otherwise
        bool _ParseRuleString(const std::string & str, EncodingRule & rule);
    };
}

#endif
