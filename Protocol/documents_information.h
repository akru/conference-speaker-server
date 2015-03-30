#ifndef DOCUMENTS_INFORMATION_H
#define DOCUMENTS_INFORMATION_H

#include "cs_packet.h"
#include <QJsonObject>
#include <QMap>

struct DocumentsInformation
{
    // Constructor by params
    DocumentsInformation(const QMap<QString, QString> &docs)
        : documents(docs)
    {}

    // Json serializer
    QJsonObject toJson() const;
    // Json deserializer
    static DocumentsInformation fromJson(const QJsonObject &json);

    QMap<QString, QString> documents;
};

#endif // DOCUMENTS_INFORMATION_H
