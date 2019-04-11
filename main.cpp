#include <QCoreApplication>
#include "serverWeb.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServerWeb servers(8000);

    return a.exec();
}
