#include "reslider.h"
#include <QDebug>
#include <QEvent>
#include <QCoreApplication>


reSlider::reSlider(QWidget *parent) : QSlider(parent)
{

    m_value=0;

}


reSlider::~reSlider()
{

}

void reSlider::mousePressEvent(QMouseEvent *event)
{
    //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    QSlider::mousePressEvent(event);
    //获取鼠标的位置
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value>maximum()){
        value=maximum();
    }
    if(value<minimum()){
        value=minimum();
    }
    m_value=value+0.5;//四舍五入
    setValue(value+0.5);
    qDebug() << "'点击'进度条到当前位置：" << m_value;
    //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
    QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
    QCoreApplication::sendEvent(parentWidget(), &evEvent);
    emit sliderPressAt(m_value);//抛出位置信号
}

void reSlider::mouseMoveEvent(QMouseEvent *event){
    QSlider::mouseMoveEvent(event);
    //qDebug()<<"mouseMoveEvent";
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value > maximum()){
        value = maximum();
    }
    if(value < minimum()){
        value = minimum();
    }
    m_value=value+0.5;//四舍五入
    setValue(m_value);
    //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
    QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
    QCoreApplication::sendEvent(parentWidget(), &evEvent);
    emit sliderPressAt(m_value);
}

void reSlider::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);

}
