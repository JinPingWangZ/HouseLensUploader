#include "settingfactory.h"

QSettings* SettingFactory::_pSettings = NULL;

QSettings* SettingFactory::setting()
{
    if( _pSettings == NULL ) {
        _pSettings = new QSettings("VideoLens", "VLWinUpload");
    }

    return _pSettings;
}

SettingFactory::SettingFactory(QObject *parent) :
    QObject(parent)
{

}
