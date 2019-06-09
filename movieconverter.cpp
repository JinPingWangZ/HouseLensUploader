#include "movieconverter.h"
#include <QApplication>
#include <QProcess>
#include "shelltask.h"
#include <QDebug>
#include <QDateTime>

MovieConverter::MovieConverter(QObject *parent) :
    QObject(parent), _videoSize(1280, 720)
{
    _delegate = NULL;
    _process = NULL;
}

MovieConverter::~MovieConverter()
{
    if( this->isRunning() ) {
        this->cancel();
    }
}

void MovieConverter::convertMovies(const QVariantList& mediaInfoArray)
{
    _cancel = false;
    _totalMovieInfos = mediaInfoArray;
    _currentMovieIndex = 0;

    _successedMovieInfos.clear();
    _failedMovieInfos.clear();
    _remainedMovieInfos = mediaInfoArray;

    if( _delegate != NULL )
        _delegate->movieConverterDidBeginConverting(this);

    _delegate->movieConverterDidBeginConverting(this);

    if( mediaInfoArray.size() == 0 ) {
        QVariantMap userInfo;
        userInfo[LocalizedDescriptionKey] = "Nothing movie to convert";
        Error error(MovieConvertErrorDomain, MCError_NothingConvertedFile, userInfo);

        _delegate->movieConverterDidFailWithError(this, &error);
        return;
    }

    this->convertMovieAtIndex(_currentMovieIndex);
}

void MovieConverter::convertMovieAtIndex(int index)
{
    _totalDuration = -1.0f;
    _currentMovieProgress = 0;
    _currentMovieIndex = index;

    QVariantMap movieInfo = _totalMovieInfos[index].toMap();

    QString oldMovieURL = movieInfo.value(MCOldMovieURL).toString();
    QString newMovieURL = movieInfo.value(MCNewMovieURL).toString();
    bool removeAudioFlag = movieInfo.value(MCRemoveAudio).toBool();

    /*
String[] Command = {"/opt/local/bin/mencoder", dir.getPath() + "/" + filelist[i], "-vf", "scale=480:270,harddup", "-ovc", "lavc", "-oac", "pcm", "-lavcopts", "vcodec=mpeg4:vbitrate=946:vglobal=1:mbd=2:trell=yes:v4mv=yes", "-of", "lavf", "-lavfopts", "format=mp4", "-ofps", "59.940", "-o", dir.getPath() + "/" + outputfilename};
*/
    QString mencoderPath = qApp->applicationDirPath() + "/bin/ffmpeg.exe";
    QString command;

    QStringList arguments;
    if( removeAudioFlag ) {
        arguments << "-y";
        arguments << "-i";
        arguments << oldMovieURL;
        arguments << "-s";
        arguments << QString("%1x%2").arg(_videoSize.width()).arg(_videoSize.height());
        arguments << "-vcodec";
        arguments << "libx264";
        arguments << "-an";
        arguments << "-loglevel";
        arguments << "info";
        arguments << newMovieURL;

//        qDebug() << "arguments: " << arguments << "\n";
    }
    else {
        arguments << "-y";
        arguments << "-i";
        arguments << oldMovieURL;
        arguments << "-s";
        arguments << QString("%1x%2").arg(_videoSize.width()).arg(_videoSize.height());
        arguments << "-vcodec";
        arguments << "libx264";
        arguments << "-acodec";
        arguments << "libfaac";
        arguments << "-loglevel";
        arguments << "info";
        arguments << newMovieURL;

//        qDebug() << "arguments: " << arguments << "\n";
    }

    if( _process != NULL )
        delete _process;

    _process = new QProcess;

    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(ffmpegHasStderr()));
    connect(_process, SIGNAL(finished(int)), this, SLOT(ffmpegDidFinish(int)));

    qDebug() << "ffmpeg start " << oldMovieURL << "\n";
    qDebug () << "start time = " << QDateTime::currentDateTime() << "\n";

    _process->start(mencoderPath, arguments);
    return;
}

int MovieConverter::percentOfCurrentMovieConverting()
{
//    if( !_process->waitForReadyRead(0) )
//        return -1;

    QByteArray stderrData = _process->readAllStandardError();
    QString logContent(stderrData);
//    qDebug() << "stderr: " << logContent << "\n";

    // if we didn't get total duration of video, we should get it from log file.
    if( _totalDuration < 0 ) {
        //   Duration: 00:00:19.71, start: 0.000000, bitrate: 46743 kb/s
        int location = logContent.indexOf("Duration:");

        if( location == -1 )
            return -1;

        QString nextContent = logContent.mid(location+QString("Duration:").length());
        int location2 = nextContent.indexOf(", start:");

        if( location2 == -1 )
            return -1;

        QString totalDuration = nextContent.left(location2);

//        qDebug() << "totalDuration " << totalDuration << "\n";

        QStringList timeComponents = totalDuration.split(":");

        if( timeComponents.count() != 3 ) {
            qDebug() << "Why duration is not hh:mm:ss? "<< timeComponents << "\n";
            return -1;
        }

        int hours = timeComponents[0].toInt();
        int mins = timeComponents[1].toInt();
        float seconds = timeComponents[2].toDouble();

        _totalDuration = hours*3600.0f + mins*60.0f + seconds;
    }

    // if video converting was finished, then we return 100%
    while( 1 ) {
        // video:473kB audio:0kB global headers:0kB muxing overhead 1.495949%
        int location = logContent.indexOf("video:");
        if( location == -1 )
            break;

        logContent = logContent.mid(location+QString("video:").length());

        location = logContent.indexOf("audio:");
        if( location == -1 )
            break;

        return 100;
    }

    // frame=  591 fps= 44 q=0.0 Lsize=     503kB time=00:00:19.65 bitrate= 209.6kbits/s
    int location1 = logContent.indexOf("time=");
    if( location1 == - 1)
        return -1;

    QString nextContent = logContent.mid(location1+QString("time=").length());
    int location2 = nextContent.indexOf("bitrate=");
    if( location2 == -1 )
        return -1;

    QString timeString = nextContent.left(location2);
    QStringList timeComponents = timeString.split(":");

    if( timeComponents.count() != 3 ) {
        qDebug() << "Why time is not hh:mm:ss? " << timeComponents << "\n";
        return -1;
    }

    int hours = timeComponents[0].toInt();
    int mins = timeComponents[1].toInt();
    double seconds = timeComponents[2].toDouble();

    double currentDuration = hours*3600.0f + mins*60.0f + seconds;
    return currentDuration/_totalDuration*100.0f;
}

bool MovieConverter::isConvertingSuccess()
{
    QByteArray data = _process->readAllStandardError();

    QString logContent(data);

    if( logContent.isEmpty() )
        return false;

    // video:473kB audio:0kB global headers:0kB muxing overhead 1.495949%

    int location = logContent.indexOf("video:");
    if( location == -1 )
        return false;

    logContent = logContent.mid(location+6);

    location = logContent.indexOf("audio:");

    if( location == -1 )
        return false;

    return true;
}

bool MovieConverter::isRunning()
{
    if( _process == NULL )
        return false;

    return true;
}

void MovieConverter::cancel()
{
    if( _process == NULL )
        return;

    _cancel = true;
    if( _process->state() == QProcess::Running ) {
        _process->kill();
        _process->waitForFinished(1000);
    }
}

void MovieConverter::ffmpegHasStdout()
{
    QString logMsg = QString(_process->readAllStandardOutput());
    qDebug() << "Converter::ffmpegHasStdout(): " << logMsg;
}

void MovieConverter::ffmpegHasStderr()
{
//    QString logMsg = QString(_process->readAllStandardError());
//    qDebug() << "stderr:" << logMsg << "\n";
//    return;
    int percent = this->percentOfCurrentMovieConverting();

    if( percent >= 0 )
        _currentMovieProgress = percent;

//    if( percent >= 0 )
//        qDebug() << "percent = " << percent << "\n";

    if( percent == 100 ) {
        qDebug () << "end time = " << QDateTime::currentDateTime() << "\n";
    }

    int count = _totalMovieInfos.count();
    float fTotalPercent = ((float)percent/100.0f + _currentMovieIndex) * 100.0f / count;

    if( _delegate != NULL )
        _delegate->movieConverterDidProgressToPercent(this, fTotalPercent);
}

void MovieConverter::ffmpegDidFinish(int retCode)
{
    _process->deleteLater();
    _process = NULL;
    Q_UNUSED(retCode);

    qDebug() << "ffmpeg return " << retCode << "\n";

    if( _cancel ) {
        if( _delegate != NULL )
            _delegate->movieConverterWasCancelled(this);

        return;
    }

    QVariantMap movieInfo = _totalMovieInfos[_currentMovieIndex].toMap();
    if( retCode == 0 ) {
        _successedMovieInfos.append(movieInfo);
    }
    else {
        _failedMovieInfos.append(movieInfo);
    }

    _remainedMovieInfos.removeAt(0);

    int count = _totalMovieInfos.count();
    if( _delegate != NULL )
        _delegate->movieConverterDidProgressToPercent(this, (_currentMovieIndex+1)*100.0f/count);

    // all movie has been converted.
    if( _currentMovieIndex == count-1 ) {
        if( _successedMovieInfos.count() == 0 ) {
            QVariantMap userInfo;
            userInfo[LocalizedDescriptionKey] = "Nothing has not been converted";
            Error error(MovieConvertErrorDomain, MCError_NothingConvertedFile, userInfo);
            if( _delegate != NULL )
                _delegate->movieConverterDidFailWithError(this, &error);
        }
        else {
            if( _delegate != NULL )
                _delegate->movieConverterDidFinishConvert(this);
        }
    }
    // convert next movie
    else {
        ++_currentMovieIndex;
        this->convertMovieAtIndex(_currentMovieIndex);
    }
}
