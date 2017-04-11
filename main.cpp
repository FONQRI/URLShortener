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
    QString path = QDir::currentPath();
    QString configPath = path + QDir::separator() + "config.ini";

    if (!QFile(configPath).exists()) QFile::copy(":/config.ini", configPath);
    QFile(configPath)
        .setPermissions(QFileDevice::WriteUser | QFileDevice::ReadUser);

    QString workDir = path + QDir::separator() + "data";
    if (!QDir(workDir).exists()) QDir().mkdir(workDir);

    for (const QString &file : QDir(":/data").entryList())
        {
            QString filePath = workDir + QDir::separator() + file;
            QFile::copy(":/data/" + file, filePath);
            QFile(filePath).setPermissions(QFileDevice::WriteUser |
                                           QFileDevice::ReadUser);
        }

    return configPath;
}

int main(int argc, char *argv[])
{
    QCoreApplication A(argc, argv);

    QString configFile = createConfigFile();

    QSettings *S = new QSettings(configFile, QSettings::IniFormat, &A);
    S->beginGroup("listener");

    QSettings *S2 = new QSettings(configFile, QSettings::IniFormat, &A);
    S2->beginGroup("files");

    QSettings *S3 = new QSettings(configFile, QSettings::IniFormat, &A);
    S3->beginGroup("templates");

    RequestManager::templateCache = new TemplateCache(S3, &A);
    RequestManager::staticFileController = new StaticFileController(S2, &A);

    HttpListener *V = new HttpListener(S, new RequestManager(&A), &A);

    return A.exec();
}
