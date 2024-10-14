extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <windows.h>
#include <string>
#define UNICODE
#define _UNICODE

inline std::string wstring_to_string(const std::wstring& wstr,
    int code_page = CP_ACP) {
  // support CP_ACP and CP_UTF8 only
  if (code_page != 0 && code_page != CP_UTF8)
    return "";
  int len = WideCharToMultiByte(code_page, 0, wstr.c_str(), (int)wstr.size(),
      NULL, 0, NULL, NULL);
  if (len <= 0)
    return "";
  std::string res;
  char* buffer = new char[len + 1];
  WideCharToMultiByte(code_page, 0, wstr.c_str(), (int)wstr.size(), buffer, len,
      NULL, NULL);
  buffer[len] = '\0';
  res.append(buffer);
  delete[] buffer;
  return res;
}

inline std::wstring string_to_wstring(const std::string& str,
    int code_page = CP_ACP) {
  // support CP_ACP and CP_UTF8 only
  if (code_page != 0 && code_page != CP_UTF8)
    return L"";
  // calc len
  int len =
    MultiByteToWideChar(code_page, 0, str.c_str(), (int)str.size(), NULL, 0);
  if (len <= 0)
    return L"";
  std::wstring res;
  WCHAR* buffer = new WCHAR[len + 1];
  MultiByteToWideChar(code_page, 0, str.c_str(), (int)str.size(), buffer, len);
  buffer[len] = '\0';
  res.append(buffer);
  delete[] buffer;
  return res;
}

std::string GetLastClipboardText() {
  if (!OpenClipboard(nullptr)) {
    return "";
  }
  UINT format = 0;
  int count = 0;
  while ((format = EnumClipboardFormats(format)) != 0) {
    count++;
  }
  format = 0;
  UINT lastFormat = 0;
  while ((format = EnumClipboardFormats(format)) != 0) {
    if (format == CF_UNICODETEXT || format == CF_TEXT) {
      lastFormat = format;
    }
  }
  HANDLE hData = GetClipboardData(lastFormat);
  if (hData != nullptr) {
    LPCVOID pData = GlobalLock(hData);
    if (pData != nullptr) {
      std::wstring text;
      if (lastFormat == CF_UNICODETEXT) {
        text = reinterpret_cast<const wchar_t*>(pData);
      } else {
        std::string str(reinterpret_cast<const char*>(pData));
        text = string_to_wstring(str, CP_ACP);
      }
      GlobalUnlock(hData);
      CloseClipboard();
      return wstring_to_string(text, CP_UTF8);
    }
  }
  CloseClipboard();
  return "";
}

int set_clipboard(lua_State* L) {
  const char* text = luaL_checkstring(L, 1);
  const char* native_text = wstring_to_string(string_to_wstring(std::string(text), CP_UTF8)).c_str();
  if (OpenClipboard(NULL)) {
    EmptyClipboard();
    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strlen(text) + 1) * sizeof(char));
    if (hglbCopy == NULL) {
      CloseClipboard();
      return 0;
    }
    char* lptstrCopy = (char*)GlobalLock(hglbCopy);
    strcpy(lptstrCopy, native_text);
    GlobalUnlock(hglbCopy);
    SetClipboardData(CF_TEXT, hglbCopy);
    CloseClipboard();
  }
  return 0;
}

int get_clipboard(lua_State* L) {
  std::string txt = GetLastClipboardText();
  lua_pushstring(L, txt.c_str());
  return 1;
}

extern "C" __declspec(dllexport) int luaopen_clip(lua_State* L) {
  static const luaL_Reg clip_lib[] = {
    {"set", set_clipboard},
    {"get", get_clipboard},
    {NULL, NULL}
  };
  luaL_newlib(L, clip_lib);
  return 1;
}
