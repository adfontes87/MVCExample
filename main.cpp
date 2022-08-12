#include "main.h"

//int main(int argc, char* argv[])
//{
//    printf("Hello World!\n");

//    TemperatureModel model(0);

//    std::thread guiThread([&]()
//    {
//        QApplication app(argc, argv);

//        GuiViewController gui(&model);
//        gui.start();

//        app.exec();
//    });

//    std::thread consoleThread([&]()
//    {
//        ConsoleView view(&model);
//        Controller controller(&model);

//        controller.start();
//    });

//    guiThread.join();
//    consoleThread.join();

//    printf("Bye!\n");

//    return 0;
//}

/* Экземпляр QApplication должен быть создан в main()-потоке,
 * иначе при завершении приложения будут возникать предупреждения вида
 * "QObject::~QObject: Timers cannot be stopped from another thread". */

int main(int argc, char* argv[])
{
    printf("Hello World!\n");

    TemperatureModel model(0);

    QApplication app(argc, argv);

    GuiViewController gui(&model);
    gui.start();

    std::thread consoleThread([&]()
    {
        ConsoleView view(&model);
        Controller controller(&model);

        controller.start();
    });

    app.exec();

    consoleThread.join();

    printf("Bye!\n");

    return 0;
}
