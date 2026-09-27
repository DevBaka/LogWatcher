#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("LogWatcher");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LogWatcher");
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
