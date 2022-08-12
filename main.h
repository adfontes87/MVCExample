#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <vector>
#include <cstdio>

#include <thread>

#include <QObject>

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

    void removeObserver(Observer *observer)
    {
        mObservers.erase(std::find(mObservers.begin(), mObservers.end(), observer));
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
        printf("Input temperature in Celsius: \n");
    }

    ~ConsoleView()
    {
        mModel->removeObserver(this);
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

class GuiViewController: public QObject, public Observer
{
    Q_OBJECT

public:

    GuiViewController(TemperatureModel *model)
    {
        mModel = model;
        mModel->addObserver(this);

        hLayout = new QHBoxLayout;
        label1 = new QLabel("Temperature:");
        label2 = new QLabel(QString("%1 C").arg(mModel->getC()));
        label3 = new QLabel(QString("%1 F").arg(mModel->getF()));
        hLayout->addWidget(label1);
        hLayout->addWidget(label2);
        hLayout->addWidget(label3);

        vLayout = new QVBoxLayout;
        lineEdit1 = new QLineEdit;
        button1 = new QPushButton("Set");
        vLayout->addWidget(lineEdit1);
        vLayout->addWidget(button1);

        mainLayout = new QVBoxLayout;
        mainLayout->addLayout(hLayout);
        mainLayout->addLayout(vLayout);

        w = new QWidget;
        w->setLayout(mainLayout);
        w->setWindowTitle("Temperature");

        connect(button1, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));
    }

    ~GuiViewController()
    {
        mModel->removeObserver(this);

        delete label1;
        delete label2;
        delete label3;
        delete hLayout;
        delete button1;
        delete lineEdit1;
        delete vLayout;
        delete mainLayout;
        delete w;
    }

    void start()
    {
        w->show();
    }

    virtual void update()
    {
        label2->setText(QString("%1 C").arg(mModel->getC()));
        label3->setText(QString("%1 F").arg(mModel->getF()));
    }

private slots:

    void slotButtonClicked(bool checked)
    {
        mModel->setC(lineEdit1->text().toFloat());
    }

private:

    TemperatureModel *mModel;

    QHBoxLayout *hLayout;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;

    QVBoxLayout *vLayout;
    QLineEdit *lineEdit1;
    QPushButton *button1;

    QVBoxLayout *mainLayout;

    QWidget *w;

};

#endif // MAIN_H
