#ifndef SHELLTASK_H
#define SHELLTASK_H

#include <QObject>
#include <QProcess>

class ShellTask : public QObject
{
    Q_OBJECT
public:
    enum ShellTaskMode
    {
        ShellTaskModeSynchronous = 0,
        ShellTaskModeAsynchronous = 1
    };

    explicit ShellTask(QObject *parent = 0);
    virtual ~ShellTask();

    QProcess* executeShellCommand(const QString& command, ShellTask::ShellTaskMode mode, const QString& taskPath, const QString outputFile);
private:
    QProcess* _process;
};

#endif // SHELLTASK_H

