#include "InstallUtils.h"

#include <QDir>

namespace
{
   QString appendShrinkyPath(const QString &aBase)
   {
      QDir dir(aBase);
      dir.setPath(dir.absolutePath() + dir.separator() + "Shrinky");
      return QDir::toNativeSeparators(QDir::cleanPath(dir.absolutePath()));
   }
}

#ifdef Q_OS_WIN32

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Winbase.h>
#include <shlobj.h>

typedef HRESULT (*FunSHGetFolderPath)(
    HWND,
    int,
    HANDLE,
    DWORD,
    LPTSTR);

QString defaultInstallPath()
{
   TCHAR result[MAX_PATH];
   result[0] = TEXT('\0');
   SHGetFolderPath(
      0,
      CSIDL_APPDATA,
      0,
      0,
      result);
   return appendShrinkyPath(
      QString::fromStdWString(
         std::wstring(result)));
}
#else
QString defaultInstallPath()
{
   return "/";
}
#endif
