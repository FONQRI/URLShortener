#include <httplistener.h>
#include <requestmanager.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

using namespace QtWebApp;

// TODO
// the user should be able to run the app with the host name given to it.
// the html part should be more beautiful.
// the should be a capcha or something to increase security and aviod adding
// urls with programs instead of people

QString createConfigFile()
{
    QString path =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir(path).exists()) qDebug() << QDir().mkdir(path);
    path = path + QDir::separator() + "config.ini";

    if (!QFile(path).exists()) QFile::copy(":/config.ini", path);

    return path;
}

int main(int argc, char *argv[])
{
    QCoreApplication A(argc, argv);

    QSettings S(createConfigFile(), QSettings::IniFormat, &A);
    S.beginGroup("listener");

    HttpListener *V = new HttpListener(&S, new RequestManager(&A), &A);

    return A.exec();
}
