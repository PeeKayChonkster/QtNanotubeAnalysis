#include "mygraphicsview.h"
#include <QDebug>
#include <QScrollBar>
#include <QWidget>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    installEventFilter(this);
    setRenderHint(QPainter::Antialiasing, true);
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Press event";
    if(event->button() == dragButton)
    {
        setCursor(Qt::ClosedHandCursor);
        dragging = true;
    }

}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "Release event";
    if(event->button() == dragButton)
    {
        setCursor(Qt::ArrowCursor);
        dragging = false;
    }
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "Move event";
    static QPoint lastPos;
    if(dragging)
    {
        QPoint delta = lastPos - event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() + delta.y());
    }

    lastPos = event->pos();
}

void MyGraphicsView::enterEvent(QEvent *event)
{
    qDebug() << "Enter event";
    enterEvent(event);
    setCursor(Qt::CrossCursor);
}


bool MyGraphicsView::eventFilter(QObject *object, QEvent *event)
{
    if(object == verticalScrollBar() && event->type() == QEvent::Wheel)
    {
        return true;
    }

    return false;
}
