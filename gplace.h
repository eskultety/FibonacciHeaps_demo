#ifndef GPLACE_H
#define GPLACE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include "internal.h"


class gPlace
{
public:
    gPlace();
    gPlace(unsigned m_ID);
    ~gPlace();

    void setX(qreal m_X);
    void setY(qreal m_Y);
    qreal x();
    qreal y();

    void setID(unsigned m_ID);
    unsigned id();

    void setItem(QGraphicsEllipseItem *m_item);
    void setBrushYellow();
    void setBrushGreen();
    void setText(QGraphicsTextItem *m_txt);
    void setKeyText(QGraphicsTextItem *m_txt_key);
    QGraphicsEllipseItem *getItem();
    QBrush &getBrush();
    QGraphicsTextItem *getText();
    QGraphicsTextItem *getKeyText();

    void setFibNode(FibNodePtr m_node);
    FibNodePtr getFibNode();

private:
    unsigned ID;
    qreal X;
    qreal Y;
    QGraphicsEllipseItem *item;
    QBrush brush;
    QGraphicsTextItem *txt;
    QGraphicsTextItem *txt_key;
    FibNodePtr node;
};


extern std::list<gPlace*> glob_places;


#endif // GPLACE_H
