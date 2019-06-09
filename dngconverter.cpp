#include "dngconverter.h"
#include "preference.h"
#include <QFileInfo>
#include <QDir>

DNGConverter::DNGConverter(QObject *parent) : QObject(parent)
{
    _delegate = NULL;
    _process = NULL;
}

DNGConverter::~DNGConverter()
{
    _delegate = NULL;

    if( this->isRunning() )
        this->cancel();
}

bool DNGConverter::isAvailable()
{
    QString location = Preference::dngConverterLocation();
    QFileInfo dngConverter(location);
    if( dngConverter.exists() )
        return true;
    return false;
}

void DNGConverter::cancel()
{
    if( _process == NULL )
        return;

    _cancel = true;

    if( _process->state() == QProcess::Running ) {
        _process->kill();
        _process->waitForFinished(1000);
    }
}

bool DNGConverter::isRunning()
{
    if( _process == NULL )
        return false;

    return true;
}

void DNGConverter::convertRawFiles(const QVariantList & fileInfos)
{
    if( _delegate )
    {
        if( !_delegate->dngConverterShouldBeginConverting(this) )
            return;

        _delegate->dngConverterDidBeginConverting(this);
    }

    _cancel = false;

    _totalFileInfos = fileInfos;
    _resultFileInfos.clear();

    if( !DNGConverter::isAvailable() && _delegate )
    {
        QVariantMap userInfo;
        userInfo[LocalizedDescriptionKey] = "You have not installed Adobe DNG Converter";
        Error error(DNGConverterErrorDomain, 0, userInfo);
        _delegate->dngConverterDidFailWithError(this, &error);
        return;
    }

    if( this->_totalFileInfos.isEmpty() && _delegate )
    {
        QVariantMap userInfo;
        userInfo[LocalizedDescriptionKey] = "Nothing movie to convert";
        Error error(DNGConverterErrorDomain, 0, userInfo);
        _delegate->dngConverterDidFailWithError(this, &error);
        return;
    }

    _currentIndex = 0;

    nextConvertRawFile();
}

void DNGConverter::nextConvertRawFile()
{
    QVariantMap fileInfo = _totalFileInfos[_currentIndex].toMap();
    QString rawURL = fileInfo[DCRAWURL].toString();
    QString dngURL = fileInfo[DCDNGURL].toString();

    //Adobe DNG Converter.exe -d <output-directory> -o <output-filename>

    QString processPath = Preference::dngConverterLocation();

    QStringList arguments;

    //add default option to prevent opening UI
    arguments << "-c";

    int jpgPreviewSizeIndex = Preference::dngPreviewIndex();
    QStringList jpgPreviewSizeParam; jpgPreviewSizeParam << "-p0" << "-p1" << "-p2";
    if( jpgPreviewSizeIndex>=0 && jpgPreviewSizeIndex<jpgPreviewSizeParam.count() )
        arguments << jpgPreviewSizeParam[jpgPreviewSizeIndex];
    if( Preference::dngEmbedFastLoadData() )
        arguments << "-fl";
    if( Preference::dngUseLossyCompression() )
    {
        arguments << "-lossy";
        int lossyIndex = Preference::dngLossyCompressionIndex();
        if( lossyIndex > 0 )
        {
            //if loosyIndex == 0 then no need to add the argument because it is default.
            lossyIndex -= 1;
            QStringList sideParams;
            sideParams << "600" << "800" << "1024" << "1440" << "1680" << "2048" << "2560";
            if( lossyIndex < sideParams.count() )
            {
                arguments << "-side";
                arguments << sideParams[lossyIndex];
            }
            else
            {
                lossyIndex -= sideParams.count();
                long long countParams[] = {
                    1*1024*1024,
                    2*1024*1024,
                    3*1024*1024,
                    4*1024*1024,
                    6*1024*1024,
                    8*1024*1024,
                    10*1024*1024,
                    15*1024*1024,
                    20*1024*1024,
                };
                if( lossyIndex < sizeof(countParams)/sizeof(countParams[0]) )
                {
                    arguments << "-count";
                    arguments << QString("%1").arg(countParams[lossyIndex]);
                }
            }
        }
    }

    QFileInfo dngFileInfo(dngURL);
    QDir dngDir = dngFileInfo.dir();
    arguments << "-d";
    arguments << dngDir.absolutePath();
    arguments << "-o";
    arguments << dngFileInfo.fileName();
    arguments << rawURL;

    if( _process != NULL )
        delete _process;
    _process = new QProcess;
    connect(_process, SIGNAL(finished(int)), this, SLOT(dngConverterDidFinish(int)));
    _process->start(processPath, arguments);
}

void DNGConverter::dngConverterDidFinish(int retCode)
{
    Q_UNUSED(retCode);

    _process->deleteLater();
    _process = NULL;

    //Cancelled?
    if( _cancel && _delegate )
    {
        _delegate->dngConverterWasCancelled(this);
        return;
    }

    QVariantMap fileInfo = _totalFileInfos[_currentIndex].toMap();
    QString dngURL = fileInfo[DCDNGURL].toString();
    QFileInfo dngFileInfo(dngURL);
    if( dngFileInfo.exists() )
        fileInfo[DCSUCESS] = true;
    _resultFileInfos.append(fileInfo);

    //Progress
    if( _delegate )
    {
        float percent = (float)(_currentIndex+1)/(float)_totalFileInfos.count();
        _delegate->dngConverterDidProgressToPercent(this, percent*100);
    }

    _currentIndex++;

    if( _currentIndex<_totalFileInfos.count() )
    {
        if( _delegate && _delegate->dngConverterShouldContinueProcessing(this) )
        {
            nextConvertRawFile();
            return;
        }
    }

    //Complete
    if( _delegate )
        _delegate->dngConverterDidFinishConverting(this);
}
