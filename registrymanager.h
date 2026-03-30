#ifndef REGISTRYMANAGER_H
#define REGISTRYMANAGER_H

#include <QSettings>

class RegistryManager
{
public:
    QString getImpPath() { return regImp; }
    QString getExpPath() { return regExp; }
    QMap<QString, int> getImplicitKeys();
    QMap<QString, int> getExplicitKeys();
    void setRegistryValue(const QString &path, const QString &key, int val);

private:
    QString regImp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit";
    QString regExp = "SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Explicit";
    QMap<QString, int> GrepRegistryContent(const QString &path);
};

#endif // REGISTRYMANAGER_H
