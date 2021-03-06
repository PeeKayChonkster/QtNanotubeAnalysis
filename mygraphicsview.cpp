#include "mygraphicsview.h"
#include <QDebug>
#include <QScrollBar>
#include <QWidget>
#include "mainwindow.h"
#include "tools.hpp"

MyGraphicsView::MyGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    installEventFilter(this);
    setRenderHint(QPainter::Antialiasing, true);
}

float MyGraphicsView::getScale() const
{
    return currScale;
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Press event";
    if(event->button() == dragButton)
    {
        setCursor(Qt::ClosedHandCursor);
        pressingDragButton = true;
    }
    if(event->button() == Qt::LeftButton)
    {
        emit si_mousePressLeft(event->pos());
    }

    Tools::getMainWindow()->mousePressEventGV(event);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "Release event";
    if(event->button() == dragButton)
    {
        setCursor(Qt::ArrowCursor);
        pressingDragButton = false;
    }

    Tools::getMainWindow()->mouseReleaseEventGV(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "Move event";
    static QPoint lastPos;
    if(pressingDragButton)
    {
        QPoint delta = lastPos - event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() + delta.y());
    }
    lastPos = event->pos();

    Tools::getMainWindow()->mouseMoveEventGV(event);
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    //qDebug() << "Wheeling!";
    float factor;
    if(event->angleDelta().y() > 0.0f)
    {
        if(event->modifiers() & Qt::ShiftModifier)
        {
            factor = zoomDelta * 2.0f;
        }
        else
        {
            factor = zoomDelta;
        }
    }
    else
    {
        if(event->modifiers() & Qt::ShiftModifier)
        {
            factor = 1.0f/(zoomDelta * 2.0f);
        }
        else
        {
            factor = 1.0f/zoomDelta;
        }
    }
    QPointF viewportPos = event->position();
    QPointF scenePos = mapToScene(QPoint(event->position().x(), event->position().y()));
    currScale *= factor;
    scale(factor, factor);
    QPointF deltaViewportPos = viewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
    QPointF viewportCenter = mapFromScene(scenePos) - deltaViewportPos;
    centerOn(mapToScene(viewportCenter.toPoint()));
}

bool MyGraphicsView::eventFilter(QObject *object, QEvent *event)
{
    if(object == verticalScrollBar() && event->type() == QEvent::Wheel)
    {
        return true;
    }
    if(object == horizontalScrollBar() && event->type() == QEvent::Wheel)
    {
        return true;
    }

    return false;
}
