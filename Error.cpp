#include "error.h"

Error::Error(const QString& domain, int code, const QVariantMap& userInfo) : QObject(NULL)
{
    _domain = domain;
    _code = code;
    _userInfo = userInfo;
}

const QString& Error::domain()
{
    return _domain;
}

int Error::code()
{
    return _code;
}

const QVariantMap& Error::userInfo()
{
    return _userInfo;
}

QString Error::localizedDescription()
{
    QString description = _userInfo.value(LocalizedDescriptionKey).toString();
    return description;
}

QString Error::localizedFailureReason()
{
    QString description = _userInfo.value(LocalizedFailureReasonErrorKey).toString();
    return description;
}

