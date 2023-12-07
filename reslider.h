#ifndef RESLIDER_H
#define RESLIDER_H

#include <QSlider>
#include <QObject>
#include <QMouseEvent>
#include <QWidget>
#include <QDebug>

class reSlider : public QSlider
{
    Q_OBJECT
public:
    reSlider(QWidget *parent = 0);
    ~reSlider();

Q_SIGNALS:
    void sliderPressAt(int);//当鼠标点击时，抛出包含鼠标X坐标位置信息的信号

public Q_SLOTS:
                //void setValue(int value);
protected:  //mouse
    void mousePressEvent(QMouseEvent *event);  //单击
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) ;
    // virtual bool event(QEvent *event) override;

    int m_value;

};

#endif // RESLIDER_H
