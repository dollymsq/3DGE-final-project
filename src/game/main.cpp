#include "window/window.h"
#include <QtGui/QGuiApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qWarning("RUNNING ON DEBUG MODE!");

    QGuiApplication app(argc, argv);

    Window window;

#ifdef Q_OS_MAC
    QSize screen = window.screen()->size();
    window.setGeometry(0,0,screen.width(),screen.height());
#endif

    window.resize(640, 480);
    window.setTitle("cs195u Final");
    window.setVisibility(QWindow::FullScreen);
    window.showFullScreen();

//    window.show();

    // will tick according to sreen refresh screen rate
    window.setAnimating(true);

    return app.exec();
}
