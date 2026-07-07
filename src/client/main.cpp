//
// Created by fran on 06/07/2026.
//

#include <QtGui>
#include <QApplication>
#include <QLabel>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QLabel label("Hello from the client !");
    label.show();
    return app.exec();
}
