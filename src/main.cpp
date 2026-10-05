#include "MainWindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFont font;
    font.setFamily("Malgun Gothic");
    font.setPointSize(10);
    app.setFont(font);

    MainWindow window;
    window.show();

    return app.exec();
}
