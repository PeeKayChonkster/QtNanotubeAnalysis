#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    MyGraphicsView(QWidget* parent);

private:
    bool pressingDragButton = false;
    const Qt::MouseButton dragButton = Qt::RightButton;
    const float zoomDelta = 1.05f;

    bool eventFilter(QObject *object, QEvent *event) override;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void si_mousePressLeft(QPoint pos);
};

#endif // MYGRAPHICSVIEW_H
