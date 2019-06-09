#ifndef APPLICATIONDEFINE_H
#define APPLICATIONDEFINE_H

#include <QString>

enum
{
    SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND = 0,
    SUBMIT_COMPRESS_VIDEO,
    SUBMIT_CONVERT_DNG_PHOTO,
    SUBMIT_COMPRESS_AUDIO,
    NUMBER_OF_SUBMIT_OPTIONS
};

typedef int SUBMIT_OPTION;

QString stringForSubmitOption(SUBMIT_OPTION option);

bool isHDM4VConvert(SUBMIT_OPTION option);
bool hasConvertOption(SUBMIT_OPTION option);
bool hasConvertDNGOption(SUBMIT_OPTION option);
bool hasUploadOption(SUBMIT_OPTION option);
bool hasZipOption(SUBMIT_OPTION option);


#define AppKey_RemoveAudio	"removeAudio"
#define AppKey_URL			"url"
#define FTP_ADDRESS			"videolens.dyndns.tv"
#define FTP_USER			"Editors"

#endif // APPLICATIONDEFINE_H
