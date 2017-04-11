#include "requestmanager.h"

TemplateCache *RequestManager::templateCache = 0;
StaticFileController *RequestManager::staticFileController = 0;

void RequestManager::createDatabase()
{
    QString path = QDir::currentPath();

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(path + QDir::separator() + "DATABASE");

    database.open();

    query = new QSqlQuery(database);
    query->exec(
        "CREATE TABLE IF NOT EXISTS urls (id INTEGER, shorturl STRING, url "
        "STRING)");
}

void RequestManager::getLastID()
{
    query->exec("SELECT id,surl FROM links ORDER BY id ASC");

    if (query->last())
        {
            lastid = query->value(0).toInt();
            lasturl = query->value(1).toString();
        }
    else
        {
            lastid = -1;
            lasturl = QString();
        }
}

void RequestManager::getNextURL(int place)
{
    if (place == -1) place = lasturl.count() - 1;

    if (lasturl.isEmpty())
        {
            lasturl = "0";
            lasturl = lasturl;
            return;
        }

    QChar last = lasturl[place].toLatin1();

    if (last.isDigit())
        {
            int i = last.digitValue();

            if (i == 9)
                lasturl[place] = QChar('A');
            else
                {
                    lasturl[place] = QString::number(i + 1)[0];
                }
        }
    else if (last.isLetter())
        {
            int i = last.toLatin1();

            if (i == 122)
                passToNextChar(place);
            else if (i == 90)
                lasturl[place] = QChar('a');
            else
                lasturl[place] = QChar(char(i + 1));
        }
}

void RequestManager::passToNextChar(int place)
{
    lasturl[place] = QChar('0');

    if (place == 0)
        lasturl.insert(0, QChar('0'));
    else if (lasturl[place - 1].toLatin1() == 122)
        passToNextChar(--place);
    else
        getNextURL(--place);
}

RequestManager::RequestManager(QObject *parent) : HttpRequestHandler(parent)
{
    createDatabase();
    getLastID();
}

void RequestManager::service(HttpRequest &request, HttpResponse &response)
{
    QString path = request.getPath();

    if (path == "/style.css")
        staticFileController->service(request, response);
    else if (path == "/")
        response.redirect("/add.html");
    else if (path == "/add.html")
        add(request, response);
    else
        get(request, response);
}

void RequestManager::add(HttpRequest &request, HttpResponse &response)
{
    QString url = request.getParameter("link");
    response.setHeader("Content-Type", "text/html; charset=UTF-8");

    if (url.isEmpty())
        {
            Template T = templateCache->getTemplate("add");
            T.setVariable("shorterLink", shorterLink);
            T.setCondition("shorted", shorterLink != nullptr);
            response.write(T.toUtf8(), true);
            return;
        }

    ++lastid;
    getNextURL();

    QString C = "INSERT INTO urls VALUES(%1, '%2', '%3')";
    C = C.arg(QString::number(lastid), lasturl, url);
    query->exec(C);

    response.write(lasturl.toUtf8(), true);
}

void RequestManager::get(HttpRequest &request, HttpResponse &response)
{
    QString url = request.getPath();
    url.remove(0, 1);

    QString C = QString("SELECT url FROM urls WHERE shorturl='%1'").arg(url);
    query->exec(C);

    if (query->last())
        response.redirect(query->value(0).toByteArray());
    else
        {
            response.setHeader("Content-Type", "text/html; charset=UTF-8");
            response.write(templateCache->getTemplate("404").toUtf8(), true);
        }
}
