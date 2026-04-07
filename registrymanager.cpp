#include "registrymanager.h"
#include <windows.h>
#include <QDebug>
#include <wrl/client.h>
#ifdef Q_OS_WIN

QList<QPair<QString, int>> RegistryManager::getImplicitKeys(){return GrepRegistryContent(regImp); }
QList<QPair<QString, int>> RegistryManager::getExplicitKeys(){return GrepRegistryContent(regExp);}
QList<QPair<QString, int>> RegistryManager::GrepRegistryContent(const QString &path){
    QList<QPair<QString, int>> registryMap;
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)path.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS){
        wchar_t keys[MAX_PATH];
        DWORD keySize = 1024, index = 0, val = 0, size = sizeof(DWORD);
        while(RegEnumValueW (hKey, index, keys, &keySize, nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS){
            QPair<QString, int> pair(QString::fromWCharArray(keys, keySize), (int)val);
            registryMap.insert(index, pair);
            qDebug() << QString::fromWCharArray(keys, keySize), val;
            index++;
            keySize = 1024, size = sizeof(DWORD);
        }
    }
    RegCloseKey(hKey);
    return registryMap;
}

void RegistryManager::setRegistryValue(const QString &path, const QString &key, int val) {
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)path.utf16(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {

        qDebug() << "Changing key:" << path << key << "to value:" << val;
        LSTATUS status = RegSetValueExW(hKey, (LPCWSTR)key.utf16(), 0, REG_DWORD, (const BYTE*)&val, sizeof(val));
        if(status != ERROR_SUCCESS) {
            qDebug() << "ERROR status:" << status;
        }
        RegCloseKey(hKey);
    } else {
        qDebug() << "Can't find path:" << path;
    }
}

#endif

