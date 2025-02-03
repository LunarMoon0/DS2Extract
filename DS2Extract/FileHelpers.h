#ifndef _FILEHELPERS_H_
#define _FILEHELPERS_H_

#include "StringBuffer.h"

bool LoadTextfileToBufferA(const char* pFileName, CStringBufferA &buf);
bool LoadTextfileToBufferW(const char* pFileName, CStringBuffer &buf);
bool LoadTextfileToBufferA(const wchar_t* pFileName, CStringBufferA &buf);
bool LoadTextfileToBufferW(const wchar_t* pFileName, CStringBuffer &buf);

#endif //!_FILEHELPERS_H_
