///////////////////////////////////////////////////////////////
//                                                           //
//   StringList.h                                            //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_STRING_LIST_H
#define TOPOGRAPHER_STRING_LIST_H

#include "Defines.h"
#include "Types.h"
#include <vector>
#include <string>

namespace topographer
{
    // An abstraction over a normal list of strings. Provides the idea
    //   of an indexed list, where every dialog string (that is, a string
    //   that starts with '【') is a key, and every string between it and
    //   the next key is an inner string. This comes to a great use
    //   when trying to automatically map English strings to Japanese ones,
    //   since key strings act as "blocks" on which the translation
    //   matching algorithm can operate on.
    class StringList
    {
    public:
        StringList();
        ~StringList();

        // Sets a string directly in the list of game strings
        //   at, size_t, the position where to put game string
        //   gs, GameString, what string to put
        // Returns: bool, true if setting string is successful, false otherwise
        bool SetDirect(const size_t at, const GameString & gs);

        // Retrieves a string directly at some position, bypassing the
        //   key indexing system.
        //   at, size_t, the position of the string to retrieve
        //   out, &GameString, the retrieved string data
        // Returns: bool, true if retrieval is successful, false otherwise
        bool GetDirect(const size_t at, GameString & out);

        // Returns the size of the entire list of strings, bypassing the
        //   key indexing system.
        size_t GetDirectSize();

        // Retrieves some key index.
        //   at, size_t, the position of the key index to retrieve
        //   out, &KeyIndex, the retrieved key index data
        // Returns: bool, true if retrieval is successful, false otherwise
        bool GetKeyIndex(const size_t at, KeyIndex & out);

        // Returns the size of the entire list of key indexes
        size_t GetKeyIndexSize();

        // Adds an inner string to the last key index, as well as to the
        // direct string list. If there is no prior key index available,
        // then this string is not indexed.
        //   str, GameString, string to add as an inner string to the last
        //     key string
        void AddInnerString(const GameString & str);

        // Adds a new key string and also stores it in the direct string
        // list.
        //   str, GameString, string to add as a new key string
        void AddKeyString(const GameString & str);

    private:
        // Vector of all game strings
        std::vector<GameString> _strings;

        // Vector of key indexes
        std::vector<KeyIndex> _keys;
    };
}

#endif
