///////////////////////////////////////////////////////////////
//                                                           //
//   EncodingTable.cpp                                       //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "EncodingTable.h"

using namespace std;
using namespace topographer;

EncodingTable::EncodingTable()
{
    _Load();
}

EncodingTable::~EncodingTable() { }

// Find an <END> control sequence (or a similar END) in the byte array
//   input, BArr, the string in which to look for end sequence
//   out, &BArr, the output of which control sequence was found
// Returns: size_t, -1 if no end sequence was found, otherwise a
//   position end sequence was found
size_t EncodingTable::FindEndControlSequence(const BArr & input, BArr & out)
{
    out.clear();

    for (size_t i = 0; i < _rules.size(); i++)
    {
        EncodingRule & r = _rules[i];

        // Only check the rules that end with '>'
        if (r.replacement.back() != 0x3E)
        {
            return -1;
        }

        // If the rule corresponds to the proper byte sequence, then
        //   it is the one we are looking for
        if (Utils::MatchByteArray(input, r.original, input.size() - r.szOriginal))
        {
            out = r.replacement;
            return input.size() - r.szOriginal;
        }
    }
}

// Replaces all the characters in byte array with UTF8 alternative
//   using the encoding rules
//   bytes, BArrr, the string in which to replaces characters
//   out, &BArr, the output string with replaces characters
void EncodingTable::Sanitize(const BArr & input, BArr & out)
{
    out.clear();

    for (size_t i = 0; i < input.size(); i++)
    {
        for(size_t j = 0; j < _rules.size(); j++)
        {
            const EncodingRule & r = _rules[j];

            // If this is not the matching rule then move onto the next one
            if (!Utils::MatchByteArray(input, r.original, i))
            {
                // Should never happen
                if (j == _rules.size() - 1)
                {
                    cout << "WARNING: Byte that cannot be replaced: ";

                    for(int z=0; z < input.size(); z++)
                    {
                        cout << (char)input[z];
                    }

                    cout << endl;
                }

                continue;
            }

            // This is the matching rule, so add the replacement to the
            //   output string
            out.insert(
                out.end(),
                r.replacement.begin(),
                r.replacement.end()
            );

            // Skip over bytes of the sequence in the original string, so that
            //   there are no lookups of already matched bytes
            i += r.original.size() - 1;

            break;
        }
    }
}

// Loads encoding table
void EncodingTable::_Load()
{
    cout << "Encoding table: " << ENCODING_TABLE_PATH << endl;

    ifstream file(ENCODING_TABLE_PATH);

    while (true)
    {
        string str;
        getline(file, str);

        if (file.eof())
        {
            break;
        }

        EncodingRule rule;
        if(_ParseRuleString(str, rule))
        {
            _rules.push_back(rule);
        }
    }

    file.close();

    // The ' ' rule gets trimmed, so we add it back in
    _rules.push_back({
        1,
        1,
        { 0x20 },
        { ' ' }
    });

    cout << "Total rules: " << _rules.size() << endl;
}

// Parses a rule string in a table
//   str, string, a rule string to parse (eg: "1337=<NICE>")
//   rule, &EncodingRule, an output encoding rule
// Returns: bool, true if parsing was successful and the rule is valid,
//   false otherwise
bool EncodingTable::_ParseRuleString(const std::string & str, EncodingRule & rule)
{
    string s = str;

    Utils::TrimString(s);

    if (s.empty())
    {
        return false;
    }

    size_t pos = s.find('=');
    if (pos == string::npos || pos == s.size() - 1)
    {
        return false;
    }

    // Divides the rule into two parts
    string original(s, 0, pos); // "0x13 0x37" from example
    string replacement(s, pos + 1, s.size() - pos); // "<NICE>" from example

    // Parse hex byte string into byte array
    BArr bytes;
    for (size_t i = 0; i < original.size(); i += 2)
    {
        uint8_t b = (uint8_t)strtol(original.substr(i, 2).c_str(), NULL, 16);
        bytes.push_back(b);
    }

    // Save the rule data in the structure
    rule.original = bytes;
    rule.replacement = BArr(replacement.begin(), replacement.end());
    rule.szOriginal = rule.original.size();
    rule.szReplacement = rule.replacement.size();

    return true;
}
