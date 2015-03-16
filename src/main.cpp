
#include <QtGui>
#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(project);

    QApplication app(argv, args);

    app.setStyle("windowsmodernstyle");
    app.setWindowIcon(QIcon(":/res/icon32.ico"));

    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 900, 550);
    mainWindow.show();

    return app.exec();
}
