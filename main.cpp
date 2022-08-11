#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <vector>
#include <cstdio>

#include <thread>

class Observer
{
public:

    virtual void update() = 0;
};

class Observable
{
public:

    void addObserver(Observer *observer)
    {
        mObservers.push_back(observer);
    }

    void notifyUpdate()
    {
        int size = mObservers.size();
        for (int i = 0; i < size; i++)
        {
            mObservers[i]->update();
        }
    }

private:

    std::vector<Observer*> mObservers;
};

class TemperatureModel: public Observable
{
public:

    TemperatureModel(float tempF)
    {
        mTemperatureF = tempF;
    }

    float getF()
    {
        return mTemperatureF;
    }

    float getC()
    {
        return (mTemperatureF - 32.0) * 5.0 / 9.0;
    }

    void setF(float tempF)
    {
        mTemperatureF = tempF;
        notifyUpdate();
    }

    void setC(float tempC)
    {
        mTemperatureF = tempC * 9.0 / 5.0 + 32.0;
        notifyUpdate();
    }

private:

    float mTemperatureF;
};

class ConsoleView: public Observer
{
public:

    ConsoleView(TemperatureModel *model)
    {
        mModel = model;
        mModel->addObserver(this);
    }

    virtual void update()
    {
        printf("Temperature in Celsius: %.2f\n", mModel->getC());
        printf("Temperature in Farenheit: %.2f\n", mModel->getF());
        printf("Input temperature in Celsius: ");
    }

private:

    TemperatureModel *mModel;
};

class Controller
{
public:

    Controller(TemperatureModel *model)
    {
        mModel = model;
    }

    void start()
    {
        mModel->setC(0);

        float temp;
        do
        {
            scanf("%f", &temp);
            mModel->setC(temp);
        }
        while (temp != 0);
    }

private:

    TemperatureModel *mModel;
};

int main(int argc, char* argv[])
{
    printf("Hello World!\n");

    std::thread guiThread([&]()
    {
        QApplication app(argc, argv);

        QHBoxLayout *hLayout = new QHBoxLayout;
        QLabel *label1 = new QLabel("Temperature:");
        QLabel *label2 = new QLabel("- C");
        QLabel *label3 = new QLabel("- F");
        hLayout->addWidget(label1);
        hLayout->addWidget(label2);
        hLayout->addWidget(label3);

        QVBoxLayout *vLayout = new QVBoxLayout;
        QLineEdit *lineEdit1 = new QLineEdit;
        QPushButton *button1 = new QPushButton("Enter");
        vLayout->addWidget(lineEdit1);
        vLayout->addWidget(button1);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(hLayout);
        mainLayout->addLayout(vLayout);

        QWidget *w = new QWidget;
        w->setLayout(mainLayout);
        w->setWindowTitle("Temperature");
        w->show();

        app.exec();
    });

    std::thread consoleThread([&]()
    {
        TemperatureModel model(0);
        ConsoleView view(&model);
        Controller controller(&model);

        controller.start();
    });

    guiThread.join();
    consoleThread.join();

    printf("Bye!\n");

    return 0;
}
