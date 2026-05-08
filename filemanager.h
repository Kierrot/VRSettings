#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "datamanager.h"

using RegistryEntry = QPair<QString, int>;
class FileManager
{
public:
        static QJsonDocument openJson(const QString &jsonPath);
        static QString getJsonElementByName(const QString &jsonPath, const QString &property);
        static QJsonValue getJsonElementByName(const QJsonObject &obj, const QString &property);

        //static bool appendValue(const QString &jsonPath, RegistryEntry pair);
        //static bool deleteValue(const QString &jsonPath, RegistryEntry pair); -- хочется qlistview ии подробную настроку рантаймов системных и кастомных
        //static bool updateCustomRuntimes(const QString &jsonPath, RegistryEntry pair); -- это когда будем  менять поярдок рантаймов
        static bool createJSON(const QString &jsonPath);
        static bool isFileExists(const QString &jsonPath);
        static QList<RegistryEntry> getCustomRuntimes();
};

#endif // FILEMANAGER_H
