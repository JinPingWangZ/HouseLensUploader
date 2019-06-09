#ifndef MOVIECONVERTER_H
#define MOVIECONVERTER_H

#include <QObject>
#include "Error.h"
#include <QVariantList>
#include <QThread>
#include <QVariantMap>
#include <QSize>
#include <QProcess>

enum {
    MCError_NothingConvertedFile = 0
};

typedef int MCERROR;

#define MovieConvertErrorDomain "MovieConvertErrorDomain"
#define MCOldMovieURL "MCOldMovieURL"
#define MCNewMovieURL "MCNewMovieURL"
#define MCRemoveAudio "MCRemoveAudio"

enum {
    MovieConverterErrorFotal = 0,
};

class MovieConverter;
class MovieConverterDelegate
{
public:
    virtual bool movieConverterShouldBeginConverting(MovieConverter* converter) { Q_UNUSED(converter); return true; }
    virtual void movieConverterDidBeginConverting(MovieConverter* converter) { Q_UNUSED(converter) }
    virtual void movieConverterWasCancelled(MovieConverter* converter) { Q_UNUSED(converter) }
    // percent = [1, 100]
    virtual void movieConverterDidProgressToPercent(MovieConverter* converter, float percent) { Q_UNUSED(converter); Q_UNUSED(percent) }
    virtual void movieConverterDidFailWithError(MovieConverter* converter, Error* error) { Q_UNUSED(converter); Q_UNUSED(error) }
    virtual void movieConverterDidFinishConvert(MovieConverter* converter) { Q_UNUSED(converter) }
};

class MovieConverter : public QObject
{
    Q_OBJECT
public:
    explicit MovieConverter(QObject *parent = 0);
    ~MovieConverter();
private:
    QVariantList            _successedMovieInfos;
    QVariantList            _failedMovieInfos;
    QVariantList            _remainedMovieInfos;
    QVariantList            _totalMovieInfos;

    int                     _currentMovieIndex;
	int						_currentMovieProgress;
    MovieConverterDelegate* _delegate;
    QSize					_videoSize;

    float					_totalDuration;// seconds

    bool                    _cancel;
    QProcess*               _process;
    // property {
public:
    void setDelegate(MovieConverterDelegate* delegate) { _delegate = delegate; }
    MovieConverterDelegate* delegate() { return _delegate; }

    void setVideoSize(QSize size) { _videoSize = size; }
    QSize videoSize() { return _videoSize; }

    int totalMovieCount() { return _totalMovieInfos.count(); }

    int successedMovieCount() { return _successedMovieInfos.count(); }
    int failedMovieCount() { return _failedMovieInfos.count(); }
    int currentMovieIndex() { return _currentMovieIndex; }

    const QVariantList& totalMovieInfos() { return _totalMovieInfos; }
    const QVariantList& successedMovieInfos() { return _successedMovieInfos; }
    const QVariantList& failedMovieInfos() { return _failedMovieInfos; }
    const QVariantList& remainedMovieInfos() { return _remainedMovieInfos; }

    const QVariantMap currentMovieInfo() { return _totalMovieInfos[_currentMovieIndex].toMap(); }

    int currentMovieProgress() { return _currentMovieProgress; }
    bool isRunning();
    // }
public:
    void convertMovies(const QVariantList& mediaInfos);
    void cancel();
private Q_SLOTS:
    void ffmpegHasStdout();
    void ffmpegHasStderr();
    void ffmpegDidFinish(int retCode);
private:
    int percentOfCurrentMovieConverting();
    void convert();
    bool isConvertingSuccess();
    void convertMovieAtIndex(int index);
};

#endif // MOVIECONVERTER_H
