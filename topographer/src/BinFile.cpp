///////////////////////////////////////////////////////////////
//                                                           //
//   BinFile.cpp                                             //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "BinFile.h"

using namespace std;
using namespace topographer;

BinFile::BinFile(EncodingTable * table)
{
    _encTable = table;
}

BinFile::~BinFile() { }

void BinFile::Process(const string & filename)
{
    _name = filename;

    cout << ORIGINAL_DATA_DIR << '/' << _name << ".BIN:" << endl;

    if (Translation::IsAvailable(_name))
    {
        _hasTranslation = true;
        _translation = Translation();
        _translation.Process(_name);
    }

    _Load();
    _SplitSections();
    _MapPointers();
    _ExtractStrings();

    if (_hasTranslation)
    {
        _MergeTranslation();
    }

    _AdjustStrings();
    _WriteAtlas();
    _WriteSection1();

    cout << endl;
}

void BinFile::_Load()
{
    cout << "\t...Loading BIN file" << endl;
    ifstream file(ORIGINAL_DATA_DIR + '/' + _name + ".BIN", ios::binary);

    while(true)
    {
        uint8_t bytes[2];

        file.read((char *)bytes, 2);

        if (file.eof())
        {
            break;
        }

        if (bytes[0] == 0x0D &&
            bytes[1] == 0x02)
        {
            _splitAddress = file.tellg();
        }

        _section1.insert(_section1.end(), bytes, bytes + 2);
    }

    file.close();

    cout << "\tTotal size " << _section1.size() << " bytes" << endl;
}

void BinFile::_SplitSections()
{
    cout << "\t...Splitting into Sections" << endl;

    _section2 = BArr(
        _section1.begin() + _splitAddress,
        _section1.end()
    );

    _section1.resize(_splitAddress);

    cout << "\tSection 1 size " << _section1.size() << " bytes" << endl;
    cout << "\tSection 2 size " << _section2.size() << " bytes" << endl;
    cout << "\tSplit Address 0x" << Utils::ValueToHexStr(_splitAddress) << endl;
}

void BinFile::_MapPointers()
{
    cout << "\t...Mapping pointers" << endl;

    uint32_t offset = 0;

    for(size_t i = 0; i < _section2.size(); i++)
    {
        // Go to the end of string, and skip the first 0x00 in a string
        if (_section2[i] != 0x00 || offset == i)
        {
            continue;
        }

        uint32_t stringAddress = _splitAddress + offset;

        // Adjust the number of bits per pointer
        if (_psz == psz16b && stringAddress >= 0xFFFF)
        {
            _psz = psz32b;
        }

        // Look for the address that contains current
        //   splitAddress + offset in the reverse order
        uint32_t pointerAddress = _FindPointerToAddress(stringAddress);

        // At this point no matter if address is found or not
        //   I want this to be set to the beginning of the
        //   next string
        offset = i + 1;

        // If Pointer Address exists
        if (pointerAddress == 0)
        {
            continue;
        }

        _pointerTable.push_back({
            pointerAddress,
            stringAddress,
        });
    }

    cout << "\tTotal " << dec << _pointerTable.size() << " pointers" << endl;
    cout << "\tString Address range from 0x"
         << Utils::ValueToHexStr(_pointerTable.front().stringAddress)
         << " to 0x"
         << Utils::ValueToHexStr(_pointerTable.back().stringAddress)
         << endl;
}

void BinFile::_ExtractStrings()
{
    cout << "\t...Extracting string data" << endl;

    // Go through every pointer, extract the string and divide it into the
    //   the main part and the ending part.
    size_t ptSize = _pointerTable.size();
    for (size_t i = 0; i < ptSize; i++)
    {
        Pointer & p = _pointerTable[i];

        // Get the endpoints of the entire string
        BArr::iterator startPoint = _section2.begin() + p.stringAddress - _splitAddress;
        BArr::iterator endPoint;

        if (i + 1 < ptSize)
        {
            endPoint = _section2.begin() + _pointerTable[i + 1].stringAddress - _splitAddress;
        }
        else
        {
            endPoint = _section2.end();
        }

        GameString gs;

        // The entire unsplitted string
        BArr str(startPoint, endPoint);

        // Extract the speaker of the string
        BArr pre;
        _ExtractSpeaker(str, pre);
        gs.pre = pre;
        gs.szPre = pre.size();

        // Extract the associated end sequence
        BArr post;
        size_t endPos = _encTable->FindEndControlSequence(str, post);
        gs.post = post;
        gs.szPost = post.size();

        // Extract the main string block
        BArr data(startPoint + pre.size(), startPoint + endPos);
        gs.data = data;
        gs.szData = data.size();

        // If it is a spoken string, then add it as a key string,
        //   otherwise add it as an inner string
        if (!gs.pre.empty())
        {
            _sl.AddKeyString(gs);
        }
        else
        {
            _sl.AddInnerString(gs);
        }
    }

    cout << "\tTotal strings: " << _sl.GetDirectSize() << endl;
    cout << "\tTotal key indexes: " << _sl.GetKeyIndexSize() << endl;
}

void BinFile::_MergeTranslation()
{
    cout << "\t...Merging translations" << endl;
    size_t szKeyOriginal = _sl.GetKeyIndexSize();
    size_t szKeyTranslation = _translation._sl.GetKeyIndexSize();

    size_t szDirectOriginal = _sl.GetDirectSize();
    size_t szDirectTranslation = _translation._sl.GetDirectSize();

    bool sameKeyIndexSize = szKeyOriginal == szKeyTranslation;
    bool sameDirectSize = szDirectOriginal == szDirectTranslation;

    cout << "\tMerging scenario: "
         << " kO: " << szKeyOriginal << ", kT: " << szKeyTranslation
         << ", dO: " << szDirectOriginal << ", dT: " << szDirectTranslation << ": ";

    // Algorithm that matches strings when they are completely exact,
    //   that is, the same number of key strings and the same number
    //   of inner strings
    if (sameKeyIndexSize && sameDirectSize)
    {
        cout << "Exact-" << endl;

        for (size_t i = 0; i < szDirectOriginal; i++)
        {
            // Game strings for Original and Translation
            GameString gsO, gsT;

            _sl.GetDirect(i, gsO);
            _translation._sl.GetDirect(i, gsT);

            gsO.data = gsT.data;
            gsO.szData = gsT.szData;

            // Setting inner flag that the original string has been replaced
            //   by the translated one
            gsO.__flag = true;

            _sl.SetDirect(i, gsO);
        }

        return;
    }

    // Algorithm that matches strings when there is the same number of
    //   key strings, but different number of inner strings. When
    //   key string has a different number of inner strings, then it
    //   moves onto the next key string to process
    if (sameKeyIndexSize && !sameDirectSize)
    {
        cout << "ExactKeyDiffDirect-" << endl;

        for(size_t i = 0; i < szKeyOriginal; i++)
        {
            // Key indexes for Original and Translation
            KeyIndex kO, kT;
            _sl.GetKeyIndex(i, kO);
            _translation._sl.GetKeyIndex(i, kT);

            // Number of innner strings of the block doesn't match
            if (kO.numInnerStrings != kT.numInnerStrings)
            {
                cout << "\t\tInvalid key index block: " << i + 1 << endl;
                continue;
            }

            for (size_t j = 0; j <= kO.numInnerStrings; j++)
            {
                // Game strings for Original and Translation
                GameString gsO, gsT;

                _sl.GetDirect(kO.index + j, gsO);
                _translation._sl.GetDirect(kT.index + j, gsT);

                gsO.data = gsT.data;
                gsO.szData = gsT.szData;

                // Setting inner flag that the original string has been replaced
                //   by the translated one
                gsO.__flag = true;

                _sl.SetDirect(kO.index + j, gsO);
            }
        }

        return;
    }

    // The last resort algorithm in the worst case scenario, when there
    //   is a different number of key strings. It starts in the normal
    //   order, then once it encounters a difference in number of key
    //   strigns it attempts to match strings in the reverse order instead.
    cout << "DiffKey-" << endl;

    size_t keyIndexReached = 0;

    for(size_t i = 0; i < szKeyOriginal; i++)
    {
        // Key indexes for Original and Translation
        KeyIndex kO, kT;
        _sl.GetKeyIndex(i, kO);
        _translation._sl.GetKeyIndex(i, kT);

        // Number of innner strings of the block doesn't match
        if (kO.numInnerStrings != kT.numInnerStrings)
        {
            cout << "\t\tInvalid key index block: " << i + 1 << endl;

            keyIndexReached = i;

            break;
        }

        for (size_t j = 0; j <= kO.numInnerStrings; j++)
        {
            // Game strings for Original and Translation
            GameString gsO, gsT;

            _sl.GetDirect(kO.index + j, gsO);
            _translation._sl.GetDirect(kT.index + j, gsT);

            gsO.data = gsT.data;
            gsO.szData = gsT.szData;

            // Setting inner flag that the original string has been replaced
            //   by the translated one
            gsO.__flag = true;

            _sl.SetDirect(kO.index + j, gsO);
        }
    }

    for(size_t i = 0; i < szKeyOriginal; i++)
    {
        // Also stop when overlap starts to happen
        if (i >= szKeyOriginal - keyIndexReached)
        {
            break;
        }

        // Key indexes for Original and Translation
        KeyIndex kO, kT;
        _sl.GetKeyIndex(szKeyOriginal - i, kO);
        _translation._sl.GetKeyIndex(szKeyTranslation - i, kT);

        // Number of innner strings of the block doesn't match
        if (kO.numInnerStrings != kT.numInnerStrings)
        {
            cout << "\t\tInvalid key index block: " << i + 1 << endl;
            break;
        }

        for (size_t j = 0; j <= kO.numInnerStrings; j++)
        {
            // Game strings for Original and Translation
            GameString gsO, gsT;

            _sl.GetDirect(kO.index + j, gsO);
            _translation._sl.GetDirect(kT.index + j, gsT);

            gsO.data = gsT.data;
            gsO.szData = gsT.szData;

            // Setting inner flag that the original string has been replaced
            //   by the translated one
            gsO.__flag = true;

            _sl.SetDirect(kO.index + j, gsO);
        }
    }
}

void BinFile::_AdjustStrings()
{
    cout << "\t...Adjusting encoding of strings" << endl;

    size_t szDirectOriginal = _sl.GetDirectSize();
    for (size_t i = 0; i < szDirectOriginal; i++)
    {
        GameString gs;
        _sl.GetDirect(i, gs);

        if (!gs.pre.empty())
        {
            BArr pre;

            _encTable->Sanitize(gs.pre, pre);

            gs.pre = pre;
            gs.szPre = pre.size();
        }

        // Hacky internal flag, skips adjusting strings if set
        //   as a result saves a lot of time
        if (!gs.__flag)
        {
            BArr data;

            _encTable->Sanitize(gs.data, data);

            gs.data = data;
            gs.szData = data.size();
        }


        _sl.SetDirect(i, gs);
    }
}

void BinFile::_WriteSection1()
{
    cout << "\t...Dumping Section 1" << endl;
    ofstream file(ATLAS_DIR + "/data-section1/" + _name + ".BIN", ios::binary);

    file.write((char*)&_section1[0], _section1.size());

    file.close();
}

void BinFile::_WriteAtlas()
{
    cout << "\t...Writing atlas script" << endl;
    ofstream file(string() + ATLAS_DIR + "/" + _name + ".txt");

    // Header of the Atlas file
    file << "#VAR(Table, TABLE)" << endl
         << "#ADDTBL(\"encoding.tbl\", Table)" << endl
         << "#ACTIVETBL(Table)" << endl
         << "#VAR(PTR, CUSTOMPOINTER)" << endl
         << "#CREATEPTR(PTR, \"LINEAR\", 0, " << (int)_psz << ")" << endl
         << "#JMP($" << Utils::ValueToHexStr(_splitAddress) << ")" << endl
         << endl;

    // Write all pointers and strings
    for(size_t i = 0; i < _pointerTable.size(); i++)
    {
        const Pointer & p = _pointerTable[i];

        file << "#WRITE(PTR, $" << Utils::ValueToHexStr(p.address) << ")" << (uint8_t)0x0D << (uint8_t)0x0A;

        GameString gs;
        _sl.GetDirect(i, gs);

        if (!gs.pre.empty())
        {
            file << string(gs.pre.begin(), gs.pre.end()) << (uint8_t)0x0D << (uint8_t)0x0A;
        }
        file << string(gs.data.begin(), gs.data.end()) << (uint8_t)0x0D << (uint8_t)0x0A;
        file << string(gs.post.begin(), gs.post.end())
             << (uint8_t)0x0D << (uint8_t)0x0A
             << (uint8_t)0x0D << (uint8_t)0x0A;
    }

    file.close();
}

uint32_t BinFile::_FindPointerToAddress(uint32_t address)
{
    // Store bytes in reverse
    uint8_t b1 = ((address >> 24) & 0xFF);
    uint8_t b2 = ((address >> 16) & 0xFF);  // ...
    uint8_t b3 = ((address >> 8) & 0xFF); // ...
    uint8_t b4 =  ((address >> 0) & 0xFF);  // 1st byte from righ// 4th

    size_t i = (_pointerTable.size() > 0) ? (_pointerTable.back().address) : 0;
    for(; i < _section1.size(); i++)
    {
        // Skip when on the odd address
        if (i % 2 == 1)
        {
            continue;
        }

        // Instantly quit if we are at the end of the Section 1
        if ((_psz == psz16b && i + 2 >= _section1.size()) ||
            (_psz == psz32b && i + 4 >= _section1.size()))
        {
            return 0;
        }

        // Perform a byte-by-byte search to check that searchFor can be
        //   found in searchIn

        // b4 and b3 are always present
        if (b4 != _section1[i] || b3 != _section1[i + 1])
        {
            continue;
        }

        // b2 and b1 are only checked when I have pointers with a size
        //   of 32 bits
        if (_psz == psz32b)
        {
            if(b2 != _section1[i + 2] && b1 != _section1[i + 3])
            {
                continue;
            }
        }

        // Found the correct pointer
        return i;
    }

    return 0;
}

void BinFile::_ExtractSpeaker(const BArr & bytes, BArr & out)
{
    out.clear();

    if (bytes.empty() || bytes[0] != 0x81 || bytes[1] != 0x79)
    {
        return;
    }

    for(size_t i = 0; i < bytes.size(); i++)
    {
        if (Utils::MatchByteArray(bytes, { 0x81, 0x7A }, i))
        {
            out = BArr(bytes.begin(), bytes.begin() + i + 2);
            return;
        }
    }
}
