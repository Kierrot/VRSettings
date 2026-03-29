#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include <QSettings>

class RegistryManager
{
public:
    QString regImp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit";
    QString regExp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Explicit";

    void setRegistryValue(QString path, QString key, int val);
    QMap<QString, int> GrepRegistryContent(QString &path);
    QMap<QString, int> getImplicitKeys();
    QMap<QString, int> getExplicitKeys();

    void Initialize();

};

#endif // REGISTRYMANAGER_H
