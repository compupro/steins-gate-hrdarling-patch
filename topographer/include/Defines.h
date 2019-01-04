///////////////////////////////////////////////////////////////
//                                                           //
//   Defines.h                                               //
//   HaselLoyance 2018, Unlicense                            //
//   https://github.com/HaselLoyance/steins-gate-psp-patch   //
//                                                           //
///////////////////////////////////////////////////////////////

#ifndef TOPOGRAPHER_DEFINES_H
#define TOPOGRAPHER_DEFINES_H

#include <string>

// Path to the enconding table
#define ENCODING_TABLE_PATH std::string() + "encoding.tbl"

// Directory where original BIN files are placed
#define ORIGINAL_DATA_DIR std::string() + "original-data"

// Directory where translation files are placed
#define TRANSLATION_DATA_DIR std::string() + "translation-data"

// Directory where Atlas scripts are generated to
#define ATLAS_DIR std::string() + "atlas-scripts"

// Directory used when generating compile.bat as output
#define PATCH_DATA_DIR std::string() + "patch-data"

#endif
