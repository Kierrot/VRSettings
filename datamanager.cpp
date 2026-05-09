#include "datamanager.h"
#include "filemanager.h"
#include <windows.h>
#include <QDebug>
#ifdef Q_OS_WIN

DataManager::DataManager(){
    updateLists();
}

QString DataManager::getRegKey(DataType type){
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

void DataManager::updateLists(){
    implicitLayers = grepRegistryContent(getRegKey(DataType::Implicit));
    explicitLayers = grepRegistryContent(getRegKey(DataType::Explicit));
    availableRuntimes = grepRegistryContent(getRegKey(DataType::RuntimeAvailable));
    activeRuntime = grepRegistryContent(getRegKey(DataType::RuntimeActive), "ActiveRuntime");
}

QString DataManager::grepRegistryContent(const QString &subKey, const QString &search){
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

QList<RegistryEntry> DataManager::grepRegistryContent(const QString &subKey){
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

void DataManager::setRegistryValueData(const QString &subKey, const QString &valueName, QVariant value, DataManager::RegType type) {
    HKEY hKey;
    DWORD winType, dataSize;
    const BYTE* dataPtr;
    int intVal;
    QString strVal;
    qDebug() << "Changing key:" << valueName << "to value:" << value;
    switch(type){
    case RegType::DWord :
        winType = REG_DWORD;
        intVal = value.toInt();
        dataPtr = (const BYTE*)&intVal;
        dataSize = sizeof(REG_DWORD);
        break;
    case RegType::String :
        winType = REG_SZ;
        strVal = value.toString();
        dataPtr = (const BYTE*)strVal.utf16();
        dataSize = (value.toString().length() + 1) * sizeof(short);
        break;
    }

    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, (LPCWSTR)valueName.utf16(), 0, winType, dataPtr, dataSize);
        RegCloseKey(hKey);
    } else {
        qDebug() << "Can't find path:" << subKey;
    }
}


void DataManager::changeLayersSystemOrder(DataManager::DataType layer, QList<RegistryEntry> list){
    qDebug() << "=== CHANGING LAYERS ORDER. LIST ===";
    for(const auto &pair : list){
        deleteRegistryValue(getRegKey(layer), pair.first);
    }
    for(const auto &pair : list){
        createRegistryValue(getRegKey(layer), pair.first);
        setRegistryValueData(getRegKey(layer), pair.first, pair.second, DataManager::RegType::DWord);
        qDebug() << pair.first <<  pair.second;
    }
}

void DataManager::createRegistryValue(const QString &subKey, const QString &valueName){
    HKEY hKey;
    RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)subKey.utf16(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    RegCloseKey(hKey);
}

void DataManager::deleteRegistryValue(const QString &subKey, const QString &valueName) {
    LSTATUS status = RegDeleteKeyValueW(
        HKEY_LOCAL_MACHINE,
        (LPCWSTR)subKey.utf16(),
        (LPCWSTR)valueName.utf16()
    );
}

QList<DataManager::Item> DataManager::moveToFront(QList<DataManager::Item> &list) {
    for (int i = 0; i < list.size(); ++i) {
        if (list[i].isActive == 1) {
            list.move(i, 0);
            break;
        }
    }
    return list;
}

QList<DataManager::Item> DataManager::fetchData(DataManager::DataType dataType) {
    QList<RegistryEntry> list;
    switch(dataType){
    case DataType::Explicit :
        list = DataManager::explicitLayers;
        break;
    case DataType::Implicit :
        list = DataManager::implicitLayers;
        break;
    case DataType::RuntimeAvailable :
        list = DataManager::availableRuntimes;
        break;
    default :
        break;
    }

    QString activeRuntimeName = FileManager::getJsonElementByName(activeRuntime, "name");
    bool foundActive = false;

    QList<DataManager::Item> items;

    for (const auto &pair : list) {
        QString rkey = pair.first, name = FileManager::getJsonElementByName(rkey, "name");
        int isActive;

        if(dataType == DataManager::DataType::RuntimeAvailable &&
            name == activeRuntimeName){
            isActive = 1;
            foundActive = true;
        }

        else
            isActive = pair.second;

        items.append({
            dataType,
            name,
            rkey,
            isActive
        });
    }

    if(dataType == DataManager::DataType::RuntimeAvailable && !foundActive){
        items.prepend({
            DataManager::DataType::RuntimeActive,
            FileManager::getJsonElementByName(activeRuntime, "name"),
            activeRuntime,
            1
        });
    }

    if(dataType == DataManager::DataType::RuntimeAvailable)
        moveToFront(items);

    return items;
}
#endif

