#include "registrymanager.h"
#include <windows.h>
#include <QDebug>
#ifdef Q_OS_WIN

RegistryManager::RegistryManager(){
    updateLists();
}

QString RegistryManager::getRegKey(DataType type){
    switch (type) {
    case DataType::Implicit:
        return "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit";
    case DataType::Explicit:
        return "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Explicit";
    case DataType::RuntimeAvailable:
        return "SOFTWARE\\Khronos\\OpenXR\\1\\AvailableRuntimes";
    case DataType::RuntimeActive:
        return "SOFTWARE\\Khronos\\OpenXR\\1";
    default:
        qDebug() << "Unknown Data Type";
        break;
    }
}

void RegistryManager::updateLists(){
    implicitLayers = grepRegistryContent(getRegKey(DataType::Implicit));
    explicitLayers = grepRegistryContent(getRegKey(DataType::Explicit));
    availableRuntimes = grepRegistryContent(getRegKey(DataType::RuntimeAvailable));
    activeRuntime = grepRegistryContent(getRegKey(DataType::RuntimeActive), "ActiveRuntime");
}

QString RegistryManager::grepRegistryContent(const QString &subKey, const QString &search){
    HKEY hKey;
    wchar_t buffer[MAX_PATH];
    DWORD size = sizeof(buffer);
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS){
        if(RegQueryValueExW(hKey, (LPCWSTR)search.utf16(), NULL, NULL, (LPBYTE)buffer, &size) == ERROR_SUCCESS){
            RegCloseKey(hKey);
            return QString::fromWCharArray(buffer);
        }
    }
    RegCloseKey(hKey);
    return NULL;
}


QList<RegistryEntry> RegistryManager::grepRegistryContent(const QString &subKey){
    QList<RegistryEntry> registryMap;
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS){
        wchar_t keys[MAX_PATH];
        DWORD keySize = 1024, index = 0, val = 0, size = sizeof(DWORD);
        while(RegEnumValueW (hKey, index, keys, &keySize, nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS){
            RegistryEntry pair(QString::fromWCharArray(keys, keySize), (int)val);
            registryMap.insert(index, pair);
            //qDebug() << "RegistryEntry" << pair;
            index++;
            keySize = 1024, size = sizeof(DWORD);
        }
    }
    RegCloseKey(hKey);
    return registryMap;
}

void RegistryManager::setRegistryValueData(const QString &subKey, const QString &valueName, int value) {
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
        qDebug() << "Changing key:" << valueName << "to value:" << value;
        LSTATUS status = RegSetValueExW(hKey, (LPCWSTR)valueName.utf16(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    } else {
        qDebug() << "Can't find path:" << subKey;
    }
}

void RegistryManager::changeLayersSystemOrder(RegistryManager::DataType layer, QList<RegistryEntry> list){
    for(const auto &pair : list){
        deleteRegistryValue(getRegKey(layer), pair.first);
    }
    for(const auto &pair : list){
        createRegistryValue(getRegKey(layer), pair.first, pair.second);
        qDebug() << "valuename " << pair.first << "val " << pair.second;
    }
}

void RegistryManager::createRegistryValue(const QString &subKey, const QString &valueName, int value){
    HKEY hKey;
    RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    setRegistryValueData(subKey, valueName, value);
    //qDebug() << "key created. key" << valueName << "val" << value;
    RegCloseKey(hKey);
}

void RegistryManager::deleteRegistryValue(const QString &subKey, const QString &valueName) {
    LSTATUS status = RegDeleteKeyValueW(
        HKEY_LOCAL_MACHINE,
        (LPCWSTR)subKey.utf16(),
        (LPCWSTR)valueName.utf16()
    );
}
#endif

