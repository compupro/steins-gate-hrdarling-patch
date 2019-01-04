///////////////////////////////////////////////////////////////
//                                                           //
//   Translation.h                                           //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_TRANSLATION_H
#define TOPOGRAPHER_TRANSLATION_H

#include "Defines.h"
#include "Types.h"
#include "Utils.h"
#include "StringList.h"
#include <iostream>
#include <string>
#include <fstream>
#include <experimental/filesystem>

namespace topographer
{
    // A translation class is responsible for loading and
    //   parsing the translation files. Basically it extracts
    //   the English strings and stores them in the same
    //   key indexed way as Japanese strings in original BINs.
    class Translation
    {
        // So that it has access to _sl
        friend class BinFile;
    public:
        Translation();
        ~Translation();

        // Checks to see if a translation file is available
        //   filename, string, the file name of the translation file
        // Returns: bool, true if translation file exists, false otherwise
        static bool IsAvailable(const std::string & filename);

        // Processes a raw translation file by making a key index
        //   and general sanitizing of the string
        //   filename, string, the ifle name of the translation file
        void Process(const std::string & filename);

    private:
        // The name of the translation file
        std::string _name;

        // List of translation strings with proper indexing
        StringList _sl;

        // Loads a translation file
        void _Load();

        // Sanitizes the string and properly indexes it as either a
        //   key string or inner string
        void _HandleString(const std::string & str);
    };
}

#endif
