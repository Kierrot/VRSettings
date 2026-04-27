#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include <QSettings>
using RegistryEntry = QPair<QString, int>;

class RegistryManager
{
public:
    enum class DataType {
        Implicit,
        Explicit,
        RuntimeAvailable,
        RuntimeActive
    };

    RegistryManager();

    QString getRegKey(DataType);
    QList<RegistryEntry> implicitLayers;
    QList<RegistryEntry> explicitLayers;
    QList<RegistryEntry> availableRuntimes;
    QString activeRuntime;

    void updateLists();
    void changeLayersSystemOrder(RegistryManager::DataType layerAddress, QList<RegistryEntry>);
    void setRegistryValueData(const QString &path, const QString &key, int val);

private:
    QList<RegistryEntry> grepRegistryContent(const QString &path);
    QString grepRegistryContent(const QString &path, const QString &search);
    void clearRegistryFolder(const QString &path);
    void createRegistryValue(const QString &path, const QString &key, int val);
    void deleteRegistryValue(const QString &path, const QString &key);

};

#endif // REGISTRYMANAGER_H
