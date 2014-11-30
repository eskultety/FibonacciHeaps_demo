#include "myqgraphicsview.h"
#include <QDebug>


MyQGraphicsView::MyQGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    this->setSceneRect(0, 0, 576, 441);
    this->setScene(scene);

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    blackpen = QPen(Qt::black, 2, Qt::SolidLine);
    redpen = QPen(Qt::red, 2, Qt::SolidLine);

    id = 0;

    edge_start_place = NULL;

    line = NULL;
    draw_type = NO_TYPE;
    drawing_edge = false;
}

void MyQGraphicsView::setDrawType(int m_draw_type)
{
    draw_type = m_draw_type;
    if (draw_type == NO_TYPE)
    {
        edge_start_place = NULL;
        makeItemsSelectable(true);
    }
    else
    {
        makeItemsSelectable(false);
    }
}

void MyQGraphicsView::makeItemsSelectable(bool is_selectable)
{
    foreach (QGraphicsItem *item, items())
    {
        if (item->type() != QGraphicsTextItem::Type)
            item->setFlag(QGraphicsItem::ItemIsSelectable, is_selectable);
    }
}

void MyQGraphicsView::clearView()
{
    scene->clear();
    glob_edges.clear();
    glob_places.clear();
    id = 0;
}

void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        QGraphicsView::mousePressEvent(e);

    if (draw_type == NO_TYPE)
    {
        QGraphicsView::mousePressEvent(e);
    }
    else if (draw_type == PLACE)
    {
        QPointF pt = mapToScene(e->pos());

        id++;
        gPlace *p = new gPlace(id);
        p->setX(pt.x() - radius);
        p->setY(pt.y() - radius);

        QGraphicsTextItem *txt = new QGraphicsTextItem(0, scene);
        txt->setZValue(3);
        txt->setPos(p->x(), p->y());
        txt->setPlainText(QString::number(id));
        p->setText(txt);

        QGraphicsEllipseItem *el = new QGraphicsEllipseItem(p->x(), p->y(),
                                                            diameter, diameter,
                                                            0, scene);
        el->setPen(blackpen);
        // makes places to be drawn on the top of edges
        el->setZValue(2);
        el->setBrush(QBrush(Qt::yellow));
        p->setItem(el);
        glob_places.push_back(p);
    }
    else    // draw_type == EDGE
    {
        edge_start_point = mapToScene(e->pos());

        // check if edge start point belongs to some place, if it does,
        // allow to draw that edge (and remember starting place)
        foreach (gPlace *p, glob_places)
        {
            if (p->getItem()->contains(edge_start_point))
            {
                drawing_edge = true;
                edge_start_place = p;

                edge_start_point.setX(p->getItem()->boundingRect().x()+radius);
                edge_start_point.setY(p->getItem()->boundingRect().y()+radius);
                break;
            }
        }
    }

    QGraphicsView::mousePressEvent(e);
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    if (drawing_edge && line != NULL)
    {
        drawing_edge = false;

        // check if edge end point belongs to some place,
        // if not, discard that edge
        bool belongs_edge = false;
        foreach (gPlace *p, glob_places)
        {
            if (p->getItem()->contains(line->line().p2()))
            {
                belongs_edge = true;

                scene->removeItem(line);
                delete line;

                // draw new line with the same end point coordinates
                // as coordinates of center of the place which it belongs to
                QLineF li(edge_start_point, edge_start_point);
                line = new QGraphicsLineItem(li, 0, scene);
                line->setPen(blackpen);
                QPointF pt(p->getItem()->boundingRect().x()+radius,
                           p->getItem()->boundingRect().y()+radius);
                QLineF new_line(edge_start_point, pt);
                line->setLine(new_line);

                gEdge *ed = new gEdge();
                ed->setItem(line);
                ed->setFrom(edge_start_place);
                ed->setTo(p);
                glob_edges.push_back(ed);
                break;
            }
        }

        if (!belongs_edge)
        {
            edge_start_place = NULL;
            scene->removeItem(line);
            delete line;
        }

        line = NULL;
    }

    QGraphicsView::mouseReleaseEvent(e);
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        QGraphicsView::mouseMoveEvent(e);

    if (draw_type == EDGE)
    {
        if (drawing_edge)
        {
            QPointF pt = mapToScene(e->pos());
            if (line == NULL)
            {
                line = new QGraphicsLineItem(QLineF(pt, pt), 0, scene);
                line->setPen(blackpen);
            }
            else
            {
                QLineF new_line(line->line().p1(), pt);
                line->setLine(new_line);
            }
        }
    }

    QGraphicsView::mouseMoveEvent(e);
}

void MyQGraphicsView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
    {
        //scene->selectedItems().contains();
        foreach (QGraphicsItem *item, scene->selectedItems())
        {
            if (item->type() == QGraphicsEllipseItem::Type)
            {
                QGraphicsEllipseItem *el =
                    qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
                foreach (gPlace *p, glob_places)
                {
                    if (p->getItem() == el)
                    {
                        // removes all edges of removing place
                        foreach (gEdge *ed, glob_edges)
                        {
                            if (ed->getFrom()->getItem() == el
                                || ed->getTo()->getItem() == el)
                            {
                                scene->removeItem(ed->getItem());
                                delete ed->getItem();
                                glob_edges.remove(ed);
                            }
                        }
                        scene->removeItem(el);
                        scene->removeItem(p->getText());
                        delete el;
                        delete p->getText();
                        item = NULL;
                        glob_places.remove(p);
                        break;
                    }
                }
            }

            if (item == NULL)
                continue;

            if (item->type() == QGraphicsLineItem::Type)
            {
                QGraphicsLineItem *li =
                    qgraphicsitem_cast<QGraphicsLineItem *>(item);
                foreach (gEdge *ed, glob_edges)
                {
                    if (ed->getItem() == li)
                    {
                        scene->removeItem(li);
                        delete li;
                        glob_edges.remove(ed);
                        break;
                    }
                }
            }
        }
    }

    QGraphicsView::keyPressEvent(e);
}

// Handles selection/deselection of all graphics items in scene
void MyQGraphicsView::selectionChanged()
{
    foreach (QGraphicsItem *item, items())
    {
        if (item->type() == QGraphicsEllipseItem::Type)
        {
            QGraphicsEllipseItem *el =
                qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
            if (el->isSelected())
                el->setBrush(QBrush(Qt::red));
            else
                el->setBrush(QBrush(Qt::yellow));
        }

        if (item->type() == QGraphicsLineItem::Type)
        {
            QGraphicsLineItem *li =
                qgraphicsitem_cast<QGraphicsLineItem *>(item);
            if (li->isSelected())
                li->setPen(redpen);
            else
                li->setPen(blackpen);
        }
    }
}
