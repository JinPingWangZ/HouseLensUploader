
#include "ApplicationDefine.h"

QString stringForSubmitOption(SUBMIT_OPTION option)
{
    QString string;
    switch( option ) 
	{
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
            string = "Compress and upload video, convert DNG and upload photo and sound files";
			break;
        case SUBMIT_COMPRESS_VIDEO:
            string = "Compress and upload video files";
			break;
        case SUBMIT_CONVERT_DNG_PHOTO:
            string = "Convert DNG and upload photo files";
			break;
        default:
            string = "Upload audio files";
			break;
    }

    return string;
}

bool hasConvertOption(SUBMIT_OPTION option)
{
    switch( option ) {
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
        case SUBMIT_COMPRESS_VIDEO:
            return true;
    }

    return false;
}

bool hasConvertDNGOption(SUBMIT_OPTION option)
{
    switch( option ) {
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
        case SUBMIT_CONVERT_DNG_PHOTO:
            return true;
    }

    return false;
}

bool hasUploadOption(SUBMIT_OPTION option)
{
    switch( option ) {
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
        case SUBMIT_COMPRESS_VIDEO:
        case SUBMIT_CONVERT_DNG_PHOTO:
            return true;
    }

    return false;
}

bool hasZipOption(SUBMIT_OPTION option)
{
    /*
    switch( option ) {
        case SUBMIT_CONVERT_MOV_AVI_TO_HDM4V_AND_ZIP:
            return true;
    }
    */

    return false;
}


bool isHDM4VConvert(SUBMIT_OPTION option)
{
    switch( option ) {
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
        case SUBMIT_COMPRESS_VIDEO:
            return true;
    }

    return false;
}
