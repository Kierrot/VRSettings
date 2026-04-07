#include "filemanager.h"
#include <qDebug>

QJsonDocument FileManager::openJson(const QString &jsonPath){
    QFile jsonFile = jsonPath;

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qDebug() << ".json reading error:" << jsonFile.errorString();
        return QJsonDocument();
    }

    QByteArray data = jsonFile.readAll();
    jsonFile.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(doc.isNull()){
        qDebug() << ".json reading error:" << error.errorString();
        return QJsonDocument();
    }
    return doc;
}

QString FileManager::getJsonElementByName(const QString &jsonPath, const QString &property){
    QJsonDocument jsonDoc = openJson(jsonPath);
    QJsonObject obj = jsonDoc.object();
    auto result = getJsonElementByName(obj, property);
    if(!result.isNull())
        return result.toString();
    return "Property not found";
}

QJsonValue FileManager::getJsonElementByName(const QJsonObject &obj, const QString &property){
    QJsonValue value;
    if (obj.contains(property)) {
        value = obj.value(property);
        return value.toString();
    }

    for(auto it = obj.begin(); it != obj.end(); ++it){
        if(it.value().isObject()){
            value = getJsonElementByName(it.value().toObject(), property);
           if (!value.isUndefined() && !value.isNull())
                return value;
        }
    }
    return QJsonValue();
}
