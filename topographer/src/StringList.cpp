///////////////////////////////////////////////////////////////
//                                                           //
//   StringList.cpp                                          //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "StringList.h"

using namespace std;
using namespace topographer;

StringList::StringList() { }
StringList::~StringList() { }

// Sets a string directly in the list of game strings
//   at, size_t, the position where to put game string
//   gs, GameString, what string to put
// Returns: bool, true if setting string is successful, false otherwise
bool StringList::SetDirect(const size_t at, const GameString & gs)
{
    if (at >= _strings.size())
    {
        return false;
    }

    _strings[at] = gs;

    return true;
}

// Retrieves a string directly at some position, bypassing the
//   key indexing system.
//   at, size_t, the position of string to retrieve
//   out, &GameString, the retrieved string data
// Returns: bool, true if retrieval is successful, false otherwise
bool StringList::GetDirect(const size_t at, GameString & out)
{
    if (at >= _strings.size())
    {
        return false;
    }

    out = _strings[at];
    return true;
}

// Returns the size of the entire list of strings, bypassing the
//   key indexing system.
size_t StringList::GetDirectSize()
{
    return _strings.size();
}

// Retrieves some key index.
//   at, size_t, the position of the key index to retrieve
//   out, &KeyIndex, the retrieved key index data
// Returns: bool, true if retrieval is successful, false otherwise
bool StringList::GetKeyIndex(const size_t at, KeyIndex & out)
{
    if (at >= _keys.size())
    {
        return false;
    }

    out = _keys[at];
    return true;
}

// Returns the size of the entire list of key indexes
size_t StringList::GetKeyIndexSize()
{
    return _keys.size();
}

// Adds an inner string to the last key index, as well as to the
// direct string list. If there is no prior key index available,
// then this string is not indexed.
//   str, GameString, string to add as an inner string to the last
//     key string
void StringList::AddInnerString(const GameString & str)
{
    _strings.push_back(str);

    if (!_keys.empty())
    {
        KeyIndex & key = _keys.back();
        key.numInnerStrings++;
    }
}

// Adds a new key string and also stores it in the direct string
// list.
//   str, GameString, string to add as a new key string
void StringList::AddKeyString(const GameString & str)
{
    _strings.push_back(str);

    _keys.push_back({
        _strings.size() - 1,
        0
    });
}
