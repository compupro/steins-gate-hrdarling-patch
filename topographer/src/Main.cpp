///////////////////////////////////////////////////////////////
//                                                           //
//   Main.cpp                                                //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "Topographer.h"
#include <iostream>
#include <experimental/filesystem>

using namespace std;
using namespace topographer;
using namespace std::experimental::filesystem;

void RecreateDirectories()
{
    //remove_all(ATLAS_DIR);
    create_directory(ATLAS_DIR);

    //remove_all(PATCH_DATA_DIR);
    create_directory(ATLAS_DIR + "/data-section1");
}

void GenerateScripts()
{
    ofstream file("atlas-compile.bat");

    file << "rmdir /s /q " << PATCH_DATA_DIR << endl;
    file << "xcopy /s /e /y " << ATLAS_DIR << "\\" << "data-section1" << "\\* "
         << PATCH_DATA_DIR << "\\" << endl;

    for(size_t i = 0; i < BIN_FILES.size(); i++)
    {
        file << "Atlas.exe "
             << PATCH_DATA_DIR << "\\" << BIN_FILES[i] << ".BIN "
             << ATLAS_DIR << "\\" << BIN_FILES[i] << ".txt"
             << endl;
    }

    file << "pause" << endl;

    file.close();
}

int main()
{
    RecreateDirectories();

    EncodingTable ec;

    for (const auto & file : BIN_FILES)
    {
        BinFile bf(&ec);
        bf.Process(file);
    }

    GenerateScripts();

    return 0;
}
