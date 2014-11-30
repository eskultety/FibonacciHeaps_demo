#include "gedge.h"

std::list<gEdge*> glob_edges;

gEdge::gEdge()
{
    item = NULL;
    from = NULL;
    to = NULL;
}

void gEdge::setItem(QGraphicsLineItem *m_item)
{
    item = m_item;
}

QGraphicsLineItem *gEdge::getItem()
{
    return item;
}

void gEdge::setFrom(gPlace *m_from)
{
    from = m_from;
}

void gEdge::setTo(gPlace *m_to)
{
    to = m_to;
}

gPlace *gEdge::getFrom()
{
    return from;
}

gPlace *gEdge::getTo()
{
    return to;
}
