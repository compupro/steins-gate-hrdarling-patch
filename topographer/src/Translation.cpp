///////////////////////////////////////////////////////////////
//                                                           //
//   Translation.cpp                                         //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "Translation.h"

using namespace std;
using namespace topographer;
using namespace std::experimental::filesystem;

Translation::Translation() { }

Translation::~Translation() { }

// Checks to see if a translation file is available
//   filename, string, the file name of the translation file
// Returns: bool, true if translation file exists, false otherwise
bool Translation::IsAvailable(const string & filename)
{
    return exists(TRANSLATION_DATA_DIR + '/' + filename + ".src");
}

// Processes a raw translation file by making a key index
//   and general sanitizing of the string
//   filename, string, the ifle name of the translation file
void Translation::Process(const string & filename)
{
    _name = filename;

    _Load();
}

// Loads a translation file
void Translation::_Load()
{
    cout << "\t...Loading matching translation" << endl;
    ifstream file(TRANSLATION_DATA_DIR + '/' + _name + ".src");

    while(true)
    {
        string str;
        getline(file, str);

        if (file.eof())
        {
            break;
        }

        Utils::TrimString(str);

        if (str.empty())
        {
            continue;
        }

        // Only care about string that start with "#mes"
        if (str.find("#mes") != 0)
        {
            continue;
        }

        // But ignore strings that start with "#mess"
        if (str.size() >=5 && str[4] == 's')
        {
            continue;
        }

        _HandleString(str);
    }

    file.close();

    cout << "\tTotal translation strings: " << _sl.GetDirectSize() << endl;
    cout << "\tTotal translation key indexes: " << _sl.GetKeyIndexSize() << endl;
}

// Sanitizes the string and properly indexes it as either a
//   key string or inner string
void Translation::_HandleString(const string & str)
{
    string s = str;

    // Replace a bunch of stuff in the translation string so that
    //   it is compatible with game control sequences
    Utils::ReplaceString(s, "&cTIPSC;", "<DICT>");
    Utils::ReplaceString(s, "&cMESC;", "</DICT>");
    Utils::ReplaceString(s, "&n", "<LINE>");
    Utils::ReplaceString(s, "&sp(142);", "%t052");
    Utils::ReplaceString(s, "&lfCR1;", "<LINE>\n");
    Utils::ReplaceString(s, "&lfCR2;", "<LINE>\n");
    Utils::ReplaceString(s, "&lfCR3;", "<LINE>\n");
    Utils::ReplaceString(s, "&lfCR4;", "<LINE>\n");
    Utils::ReplaceString(s, "&lfCR5;", "<LINE>\n");
    Utils::ReplaceString(s, "&lfCR6;", "<LINE>\n");
    Utils::ReplaceString(s, "&c5;", "%CFF008");
    Utils::ReplaceString(s, "&c00;", "%CE");
    Utils::ReplaceString(s, "“", " \"");
    Utils::ReplaceString(s, "”", "\" ");
    Utils::ReplaceString(s, "‘", "'");
    Utils::ReplaceString(s, "’", "'");
    Utils::ReplaceString(s, "&rs", "");
    Utils::ReplaceString(s, "&rt&re", "");
    Utils::ReplaceString(s, "&rt", "");
    Utils::ReplaceString(s, "&re", "");
    Utils::ReplaceString(s, "&cPhC;", "");
    Utils::ReplaceString(s, "&lc", "%LC");
    Utils::ReplaceString(s, "、", ", ");
    Utils::ReplaceString(s, "……", "...");
    Utils::ReplaceString(s, "…", "...");
    //Utils::ReplaceString(s, "─", "-"); // ?
    //Utils::ReplaceString(s, "…", "..."); // ?
    //Utils::ReplaceString(s, "", "");
    Utils::ReplaceString(s, "　", " ");
    Utils::ReplaceString(s, "  ", " ");

    bool isKeyIndex = s[4] == '2';

    // There is a small exception on how to extract string from SG03_06
    bool SG03_06_exception = _name == "SG03_06" && s.find("$W(LR_LIP_CRS),VID_CRS,") != string::npos;

    if (SG03_06_exception)
    {
        string startAfter = "$W(LR_LIP_CRS),VID_CRS,";
        size_t lineStart = s.find(startAfter) + startAfter.size();
        size_t lineEnd = s.find_last_of('%');

        string data(s, lineStart, lineEnd - lineStart);
        data = string()
               + "<LINE>" + (char)0x0D + (char)0x0A
               + "<LINE>" + (char)0x0D + (char)0x0A
               + "<LINE>" + (char)0x0D + (char)0x0A
               + data;

        Utils::TrimString(data);

        _sl.AddInnerString({
            0,
            data.size(),
            0,
            {},
            BArr(data.begin(), data.end()),
            {}
        });

        return;
    }

    // Perform some magic to extract strings and store them in the format I want
    if (isKeyIndex)
    {
        // Extracting dialog lines
        string delimSpeakerBegin = ",＠";
        string delimSpeakerEnd = "＠「";

        size_t speakerBegin = s.find(delimSpeakerBegin) + delimSpeakerBegin.size();
        size_t speakerEnd = s.find(delimSpeakerEnd);

        size_t lineEnd = s.find("」");
        string data(s, speakerEnd + delimSpeakerEnd.size(), lineEnd - speakerEnd - delimSpeakerEnd.size());

        Utils::TrimString(data);

        _sl.AddKeyString({
            0,
            data.size(),
            0,
            {},
            BArr(data.begin(), data.end()),
            {}
        });
    }
    else
    {
        // Extracting normal lines
        size_t lineStart = s.find(' ') + 1;
        size_t lineEnd = s.find_last_of('%');

        string data(s, lineStart, lineEnd - lineStart);

        Utils::TrimString(data);

        _sl.AddInnerString({
            0,
            data.size(),
            0,
            {},
            BArr(data.begin(), data.end()),
            {}
        });
    }
}

