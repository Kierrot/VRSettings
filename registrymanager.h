#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include <QSettings>

class RegistryManager
{
public:
    QString getImpPath() { return regImp; }
    QString getExpPath() { return regExp; }
    QList<QPair<QString, int>> getImplicitKeys();
    QList<QPair<QString, int>> getExplicitKeys();
    //QMap<QString, int> changeLayersOrder(QMap<QString, int>);
    void setRegistryValue(const QString &path, const QString &key, int val);

private:
    QString regImp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit";
    QString regExp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Explicit";

    QList<QPair<QString, int>> GrepRegistryContent(const QString &path);
    //QMap<QString, int> GrepRegistryContent(const QString &path);
    //void clearRegistryFolder(const QString &path);
    //void createRegistryKey(const QString &path, const QString &key, int val);
    //void deleteRegistryKey(const QString &path, const QString &key, int val);
};

#endif // REGISTRYMANAGER_H
