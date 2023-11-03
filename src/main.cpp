#include <QApplication>
#include "main_viewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Main_Viewer window_main;
    window_main.setWindowTitle("menuVis");
    window_main.show();
    return a.exec();
}

// to make app bundle run the following command
// /Applications/QT/5.9/clang_64/bin/macdeployqt /Users/silverburla/Documents/UBC/UBC/CPSC\ 547/project/code/menuVis/build/release/menuVis.app
