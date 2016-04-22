#include "main.h"
#include "Ulit.h"

#include <time.h>

using std::wstring;

wstring GetCurTime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	wchar_t sz[256];
	wsprintfW(sz, L"%d-%02d-%02d(%02d-%02d-%02d)",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);
	return sz;
}

bool StringToBool(std::string &value)
{
    assert(value.c_str() != nullptr);

    if (value == "true")
        return true;
    else
        return false;
}

bool StringToBool(const char *value)
{
    assert(value != nullptr);

    if (strcmp(value, "true") == 0)
        return true;
    else
        return false;
}

int StringToInt(const char *value)
{
    assert(value != nullptr);

    int i;
    sscanf(value, "%d", &i);
    return i;
}

int StringToInt(const std::string &value)
{
    assert(value.c_str() != nullptr);

    int i;
    sscanf(value.c_str(), "%d", &i);
    return i;
}

wstring IntToString(int value)
{
    wchar_t sz[20];
    wsprintf(sz, L"%d", value);
    return sz;
}

wstring ANSIToUnicode(const std::string& str)
{
    int  len = 0;
    len = str.length();
    int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
        0,
        str.c_str(),
        -1,
        NULL,
        0);
    wchar_t *  pUnicode;
    pUnicode = new  wchar_t[unicodeLen + 1];
    memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
    ::MultiByteToWideChar(CP_ACP,
        0,
        str.c_str(),
        -1,
        (LPWSTR)pUnicode,
        unicodeLen);
    wstring  rt;
    rt = (wchar_t*)pUnicode;
    delete[]pUnicode;

    return  rt;
}

std::string UnicodeToANSI(const wstring& str)
{
    char*     pElementText;
    int    iTextLen;
    // wide char to multi char
    iTextLen = WideCharToMultiByte(CP_ACP,
        0,
        str.c_str(),
        -1,
        NULL,
        0,
        NULL,
        NULL);
    pElementText = new char[iTextLen + 1];
    memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
    ::WideCharToMultiByte(CP_ACP,
        0,
        str.c_str(),
        -1,
        pElementText,
        iTextLen,
        NULL,
        NULL);
    std::string strText;
    strText = pElementText;
    delete[] pElementText;
    return strText;
}

wstring UTF8ToUnicode(const std::string& str)
{
    int  len = 0;
    len = str.length();
    int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
        0,
        str.c_str(),
        -1,
        NULL,
        0);
    wchar_t *  pUnicode;
    pUnicode = new  wchar_t[unicodeLen + 1];
    memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
    ::MultiByteToWideChar(CP_UTF8,
        0,
        str.c_str(),
        -1,
        (LPWSTR)pUnicode,
        unicodeLen);
    wstring  rt;
    rt = (wchar_t*)pUnicode;
    delete[]pUnicode;

    return  rt;
}

std::string UnicodeToUTF8(const wstring& str)
{
    char*     pElementText;
    int    iTextLen;
    // wide char to multi char
    iTextLen = WideCharToMultiByte(CP_UTF8,
        0,
        str.c_str(),
        -1,
        NULL,
        0,
        NULL,
        NULL);
    pElementText = new char[iTextLen + 1];
    memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
    ::WideCharToMultiByte(CP_UTF8,
        0,
        str.c_str(),
        -1,
        pElementText,
        iTextLen,
        NULL,
        NULL);
    std::string strText;
    strText = pElementText;
    delete[] pElementText;
    return strText;
}

void SingleToUnicode(const char *source, wstring &str)
{
    size_t size = strlen(source);
    size_t wsize = mbstowcs(0, source, size);
    wchar_t* Buffer = new wchar_t[wsize + 1];
    mbstowcs(Buffer, source, size);
    Buffer[size] = L'\0';
    str = Buffer;

    delete[] Buffer;
}

bool LoadFile(wstring &file, wstring &code)
{
    FILE* f;
    if (_wfopen_s(&f, file.c_str(), L"rb") == EINVAL)
        return false;

    fpos_t fsize;
    size_t size;
    fseek(f, 0, SEEK_END);
    fgetpos(f, &fsize);
    size = (size_t)fsize;
    fseek(f, 0, SEEK_SET);
    char* AnsiBuffer = new char[size + 1];
    fread(AnsiBuffer, 1, size, f);
    AnsiBuffer[size] = '\0';
    fclose(f);

    std::string ansi = AnsiBuffer;
    code = ANSIToUnicode(ansi);

    delete[] AnsiBuffer;

    return true;
}

wstring GetAppPath()
{
    TCHAR modulePath[MAX_PATH];
    GetModuleFileName(NULL, modulePath, MAX_PATH);
    wstring strModulePath(modulePath);
    strModulePath = _wgetcwd(NULL, 0);
    strModulePath += '\\';
    return strModulePath;
}