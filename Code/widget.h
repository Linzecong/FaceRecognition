#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "PCACom.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void dirClick();
    void imgClick();
    void filteClick();

    QImage getGrey(QImage image, int flag=0);

    void getCom();
    void viewAll();
    QImage getLBP(QImage img);

    void train();
    void test();
    void testAll();

    void clearClick();


private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
