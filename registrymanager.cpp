#include "registrymanager.h"
#include <windows.h>
#include <QDebug>
#include <wrl/client.h>
#ifdef Q_OS_WIN

QList<RegistryEntry> RegistryManager::getImplicitKeys(){return GrepRegistryContent(getLayerKey(LayerType::Implicit)); }
QList<RegistryEntry> RegistryManager::getExplicitKeys(){return GrepRegistryContent(getLayerKey(LayerType::Explicit));}

QString RegistryManager::getLayerKey(LayerType type){
    switch (type) {
    case LayerType::Implicit:
        return "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit";
    case LayerType::Explicit:
        return "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Explicit";
    default:
        qDebug() << "Unknown Layer Type";
        break;
    }
}

QList<RegistryEntry> RegistryManager::GrepRegistryContent(const QString &subKey){
    QList<RegistryEntry> registryMap;
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS){
        wchar_t keys[MAX_PATH];
        DWORD keySize = 1024, index = 0, val = 0, size = sizeof(DWORD);
        while(RegEnumValueW (hKey, index, keys, &keySize, nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS){
            RegistryEntry pair(QString::fromWCharArray(keys, keySize), (int)val);
            registryMap.insert(index, pair);
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

void RegistryManager::changeLayersSystemOrder(RegistryManager::LayerType layer, QList<RegistryEntry> list){
    for(const auto &pair : list){
        deleteRegistryValue(getLayerKey(layer), pair.first);
    }
    for(const auto &pair : list){
        createRegistryValue(getLayerKey(layer), pair.first, pair.second);
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

