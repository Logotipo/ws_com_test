#include <QtCore/QCoreApplication>
#include "modbusmaster.h"
#include "consolemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConsoleManager *consoleManager = new ConsoleManager();
    consoleManager->start();

    return a.exec();
}
