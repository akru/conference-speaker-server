#include "documents_information.h"
#include "cs_packet.h"
#include <QVariant>

// Json serializer
QJsonObject DocumentsInformation::toJson() const
{
    QJsonObject obj;
    foreach (const QString &k, documents.keys()) {
        obj.insert(k, documents.value(k));
    }
    return obj;
}

// Json deserializer
DocumentsInformation DocumentsInformation::fromJson(const QJsonObject &json)
{
    QMap<QString, QVariant> jsonDocs = json.toVariantMap();
    QMap<QString, QString> docs;
    foreach (const QString &k, jsonDocs.keys()) {
        docs.insert(k, jsonDocs.value(k).toString());
    }
    return DocumentsInformation(docs);
}
