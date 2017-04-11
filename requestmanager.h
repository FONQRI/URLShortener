#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <httprequesthandler.h>
#include <staticfilecontroller.h>
#include <templatecache.h>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

using namespace QtWebApp;

class RequestManager : public HttpRequestHandler
{
    Q_OBJECT

    int lastid;
    QString lasturl;
    QSqlQuery *query;
    QSqlDatabase database;

    void createDatabase();
    void getLastID();

    void getNextURL(int place = -1);
    void passToNextChar(int place);

public:
    explicit RequestManager(QObject *parent = 0);

    static TemplateCache *templateCache;
    static StaticFileController *staticFileController;

    void service(HttpRequest &request, HttpResponse &response);
    void add(HttpRequest &request, HttpResponse &response);
    void get(HttpRequest &request, HttpResponse &response);
};

#endif  // REQUESTMANAGER_H
