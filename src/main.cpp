#include "MainWindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    // Keep menus inside the window so hiding the menu bar also hides File/Edit/View (e.g. on macOS).
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
