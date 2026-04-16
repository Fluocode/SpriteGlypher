#include "MainWindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    // Keep menus inside the window so hiding the menu bar also hides File/Edit/View (e.g. on macOS).
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);

    QApplication a(argc, argv);

    QString initialFile;
    const QStringList args = QCoreApplication::arguments();
    for ( int i = 1; i < args.size(); ++i ) {
        const QString &p = args[i];
        if ( p.endsWith(QLatin1String(".sgf"), Qt::CaseInsensitive) ) {
            initialFile = p;
            break;
        }
    }

    MainWindow w(nullptr, initialFile);
    w.show();

    return a.exec();
}
