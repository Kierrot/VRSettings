#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QSettings>

using RegistryEntry = QPair<QString, int>;

class DataManager
{
public:
    enum class DataType {
        Implicit,
        Explicit,
        RuntimeAvailable,
        RuntimeActive
    };

    DataManager();

    QString getRegKey(DataType);
    QList<RegistryEntry> implicitLayers;
    QList<RegistryEntry> explicitLayers;
    QList<RegistryEntry> availableRuntimes;
    QString activeRuntime;

    void updateLists();
    void changeLayersSystemOrder(DataManager::DataType layerAddress, QList<RegistryEntry>);
    void setRegistryValueData(const QString &path, const QString &key, int val);
    void setRegistryValueData(const QString &path, const QString &key, QString val);

    struct Item {
        DataManager::DataType type;
        QString displayName;
        QString registryKey;
        int isActive;
    };
    QList<DataManager::Item> fetchData(DataManager::DataType dataType);

private:
    QList<RegistryEntry> grepRegistryContent(const QString &path);
    QList<DataManager::Item> moveToFront(QList<DataManager::Item> &list);

    QString grepRegistryContent(const QString &path, const QString &search);

    void clearRegistryFolder(const QString &path);
    void createRegistryValue(const QString &path, const QString &key, int val);
    void deleteRegistryValue(const QString &path, const QString &key);

};

#endif // DATAMANAGER_H
