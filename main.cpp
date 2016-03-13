#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include "quietinstaller.h"

//#define QT_NO_DEBUG_OUTPUT

int main(int argc, char *argv[])
{
    //embed fonts
    QFile res1(":/fonts/bell-gothic-black.ttf");
    if (res1.open(QIODevice::ReadOnly))
    {
        QFontDatabase::addApplicationFontFromData(res1.readAll());
    }
    QFile res2(":/fonts/bell-gothic-bold.ttf");
    if (res2.open(QIODevice::ReadOnly))
    {
        QFontDatabase::addApplicationFontFromData(res2.readAll());
    }
    QFile res3(":/fonts/bell-gothic-light.ttf");
    if (res3.open(QIODevice::ReadOnly))
    {
        QFontDatabase::addApplicationFontFromData(res3.readAll());
    }

    bool quiet = false;

    QApplication a(argc, argv);

    if (argc>1)
    {
        QString arg1 = argv[1];
        qDebug() << arg1;
        if (arg1 == "-q" || arg1 == "-quiet" || arg1 == "-s" || arg1 == "-silent")
        {
            quiet = true;

            QuietInstaller qi = QuietInstaller();
        }
        else
        {
            qWarning() << "Unknown argument";
        }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
