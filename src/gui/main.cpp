/**
 * @file main.cpp
 * @brief FileForge GUI Application Entry Point
 */

#include "mainwindow.hpp"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application info
    app.setApplicationName("FileForge");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("FileForge");
    app.setOrganizationDomain("fileforge.app");

    // Use Fusion style for consistent cross-platform look
    app.setStyle(QStyleFactory::create("Fusion"));

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}
