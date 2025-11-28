#ifndef SCRCPYWINDOW_H
#define SCRCPYWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class ScrcpyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScrcpyWindow(const QString &packageName, const QString &appName, QWidget *parent = nullptr);
    ~ScrcpyWindow();

    void launchScrcpy();

private slots:
    void onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onScrcpyError(QProcess::ProcessError error);
    void onScrcpyStarted();
    void onStopClicked();

private:
    void setupUI();

    QString packageName;
    QString appName;
    QProcess *scrcpyProcess;

    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *statusLabel;
    QPushButton *stopButton;
};

#endif // SCRCPYWINDOW_H
