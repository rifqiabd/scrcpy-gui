#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("Qt GUI Scrcpy");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Qt GUI Scrcpy");

    MainWindow window;
    window.show();

    return app.exec();
}
