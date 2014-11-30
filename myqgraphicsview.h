#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QPointF>
#include "gplace.h"
#include "gedge.h"

#define NO_TYPE 0
#define PLACE   1
#define EDGE    2


class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent = 0);
    void setDrawType(int m_draw_type);
    void makeItemsSelectable(bool is_selectable);
    void clearView(void);

signals:

public slots:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void selectionChanged(void);

private:
    QGraphicsScene *scene;
    QPen blackpen;
    QPen redpen;

    unsigned id;    // unique identifier of place

    gPlace *edge_start_place;

    QGraphicsLineItem *line;
    QLineF *new_line;
    QPointF edge_start_point;
    bool drawing_edge;      // true while mouse is pressed and draw_type
                            // is EDGE
    int draw_type;
    const qreal diameter = 30;
    const qreal radius = 15;
};

#endif // MYQGRAPHICSVIEW_H
