#pragma once

std::wstring GetCurTime();

bool StringToBool(std::string &value);
bool StringToBool(const char *value);
int StringToInt(const char *value);
int StringToInt(const std::string &value);

std::wstring IntToString(int value);

std::string UnicodeToANSI(const std::wstring& str);
std::wstring ANSIToUnicode(const std::string& str);
std::wstring UTF8ToUnicode(const std::string& str);
std::string UnicodeToUTF8(const std::wstring& str);

bool LoadFile(std::wstring &file, std::wstring &code);
std::wstring GetAppPath();