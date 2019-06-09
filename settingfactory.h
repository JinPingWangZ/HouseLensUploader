#ifndef SETTINGFACTORY_H
#define SETTINGFACTORY_H

#include <QObject>
#include <QSettings>

class SettingFactory : public QObject
{
    Q_OBJECT
public:
    static QSettings* setting();
private:
    SettingFactory(QObject *parent = 0);
    static QSettings* _pSettings;
signals:
    
public slots:
    
};

#endif // SETTINGFACTORY_H
