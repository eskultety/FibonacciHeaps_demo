#ifndef GEDGE_H
#define GEDGE_H

#include <QGraphicsLineItem>
#include "gplace.h"


class gEdge
{
public:
    gEdge();

    void setItem(QGraphicsLineItem *m_item);
    QGraphicsLineItem *getItem();

    void setFrom(gPlace *m_from);
    void setTo(gPlace *m_to);
    gPlace *getFrom();
    gPlace *getTo();

private:
    QGraphicsLineItem *item;
    class gPlace *from;
    class gPlace *to;
    // ptr to FibbNode
};


extern std::list<gEdge*> glob_edges;


#endif // GEDGE_H
