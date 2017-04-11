#include <httplistener.h>
#include <requestmanager.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

using namespace QtWebApp;

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
