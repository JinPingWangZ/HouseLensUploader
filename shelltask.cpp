#include "shelltask.h"

ShellTask::ShellTask(QObject *parent) :
    QObject(parent)
{
    _process = NULL;
}

ShellTask::~ShellTask()
{
    delete _process;
}

QProcess* ShellTask::executeShellCommand(const QString& command, ShellTask::ShellTaskMode mode, const QString& taskPath, const QString outputFile)
{
    _process = new QProcess;

    if( !taskPath.isEmpty() )
        _process->setWorkingDirectory(taskPath);

    if( !outputFile.isEmpty() ) {
        _process->setStandardOutputFile(outputFile);
        _process->setStandardErrorFile(outputFile);
    }

    _process->start("cmd.exe", QStringList() << "/c" << command);

    _process->waitForStarted(-1);
    if( mode == ShellTaskModeSynchronous ) {
        _process->waitForFinished(-1);
    }

    return _process;
}
