#include "mainwindow.h"
#include <QApplication>
#include <SDL.h>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("David Tran");
    QCoreApplication::setApplicationName("Qtboy");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cout << "Could not initialize SDL_Audio: " << SDL_GetError();
    }
    MainWindow mainWin(nullptr, "QtBoy");
    mainWin.show();
    return app.exec();
}
