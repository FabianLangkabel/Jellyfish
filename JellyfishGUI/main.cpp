#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QResource>

//#include "gui/styles.h"
#include "gui/mainwindow.h"

int main(int argc, char* argv[])
{
    QResource::registerResource("icons.qrc");
    QApplication app(argc, argv);

    MainWindow win;
    win.showMaximized();

    return app.exec();  
}