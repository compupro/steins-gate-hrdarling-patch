///////////////////////////////////////////////////////////////
//                                                           //
//   BinFile.h                                               //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_BIN_FILE_H
#define TOPOGRAPHER_BIN_FILE_H

#include "Defines.h"
#include "Types.h"
#include "Utils.h"
#include "EncodingTable.h"
#include "Translation.h"

namespace topographer
{
    class BinFile
    {
    public:
        BinFile(EncodingTable * table);
        ~BinFile();

        void Process(const std::string & filename);

    private:
        void _Load();
        void _SplitSections();
        void _MapPointers();
        void _ExtractStrings();
        void _MergeTranslation();
        void _AdjustStrings();
        void _WriteSection1();
        void _WriteAtlas();

        uint32_t _FindPointerToAddress(uint32_t address);
        void _ExtractSpeaker(const BArr & bytes, BArr & out);

        std::string _name;
        bool _hasTranslation = false;
        uint32_t _splitAddress;
        BArr _section1;
        BArr _section2;
        std::vector<Pointer> _pointerTable;
        PointerSz _psz = psz16b;
        StringList _sl;
        Translation _translation;
        EncodingTable * _encTable;
    };
}

#endif
