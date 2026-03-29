#include "registrymanager.h"
#include <windows.h>
#include <QDebug>
#include <wrl/client.h>
#ifdef Q_OS_WIN

QMap<QString, int> RegistryManager::getImplicitKeys(){return GrepRegistryContent(regImp); }
QMap<QString, int> RegistryManager::getExplicitKeys(){return GrepRegistryContent(regExp);}

QMap<QString, int> RegistryManager::GrepRegistryContent(QString &path){
    QMap<QString, int> registryMap;
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)path.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS){
        wchar_t keys[MAX_PATH];
        DWORD keySize = 1024, index = 0, val = 0, size = sizeof(DWORD);
        while(RegEnumValueW (hKey, index, keys, &keySize, nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS){
            registryMap.insert(QString::fromWCharArray(keys, keySize), val);
            index++;
            keySize = 1024, size = sizeof(DWORD);
        }
    }
    RegCloseKey(hKey);
    return registryMap;
}

void RegistryManager::setRegistryValue(QString path, QString key, int val){
    HKEY hkey;
    DWORD size = sizeof(DWORD);
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)path.utf16(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS){
        RegSetValueExW(hkey, (LPCWSTR)key.utf16(), 0, REG_DWORD, (PBYTE)&val, size);
        RegCloseKey(hkey);
    }
}
#endif

