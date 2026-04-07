#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

class FileManager
{
public:
        static QJsonDocument openJson(const QString &jsonPath);
        static QString getJsonElementByName(const QString &jsonPath, const QString &property);
        static QJsonValue getJsonElementByName(const QJsonObject &obj, const QString &property);

};

#endif // FILEMANAGER_H
