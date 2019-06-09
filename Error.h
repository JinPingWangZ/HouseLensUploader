#ifndef ERROR_H
#define ERROR_H

#include <QObject>
#include <QVariantMap>

#define LocalizedDescriptionKey QString("LocalizedDescriptionKey")
#define LocalizedFailureReasonErrorKey QString("LocalizedFailureReasonErrorKey")

class Error : public QObject
{
    Q_OBJECT
public:
    explicit Error(const QString& domain, int code, const QVariantMap& userInfo = QVariantMap());
public:
    const QString& domain();
    int code();
    const QVariantMap& userInfo();
    QString localizedDescription();
    QString localizedFailureReason();
private:
    QString _domain;
    int _code;
    QVariantMap _userInfo;
};

#endif // ERROR_H
