#ifndef GEDGE_H
#define GEDGE_H

#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include "gplace.h"


class gEdge
{
public:
    gEdge();
    ~gEdge();

    void setItem(QGraphicsLineItem *m_item);
    QGraphicsLineItem *getItem();

    void setPenBlack();
    void setPenRed();
    QPen &getPen();

    void setText(QGraphicsTextItem *m_txt);
    QGraphicsTextItem *getText();

    void setW(int m_W);
    int w();

    void setFrom(gPlace *m_from);
    void setTo(gPlace *m_to);
    gPlace *getFrom();
    gPlace *getTo();

private:
    QGraphicsLineItem *item;
    QPen pen;
    QGraphicsTextItem *txt;
    int W;
    class gPlace *from;
    class gPlace *to;
};


extern std::list<gEdge*> glob_edges;


#endif // GEDGE_H
