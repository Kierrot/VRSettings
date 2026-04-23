#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include <QSettings>
using RegistryEntry = QPair<QString, int>;

class RegistryManager
{
public:
    enum class LayerType {
        Implicit,
        Explicit
    };
    QString getLayerKey(LayerType);
    QList<RegistryEntry> getImplicitKeys();
    QList<RegistryEntry> getExplicitKeys();
    void changeLayersSystemOrder(RegistryManager::LayerType layerAddress, QList<RegistryEntry>);
    void setRegistryValueData(const QString &path, const QString &key, int val);

private:
    QList<RegistryEntry> GrepRegistryContent(const QString &path);
    void clearRegistryFolder(const QString &path);
    void createRegistryValue(const QString &path, const QString &key, int val);
    void deleteRegistryValue(const QString &path, const QString &key);

};

#endif // REGISTRYMANAGER_H
