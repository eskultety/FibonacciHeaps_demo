#include "gedge.h"

std::list<gEdge*> glob_edges;

gEdge::gEdge()
{
    item = NULL;
    txt = NULL;
    from = NULL;
    to = NULL;
    pen = QPen(Qt::black, 2, Qt::SolidLine);
}

gEdge::~gEdge()
{
    delete item;
    delete txt;
}

void gEdge::setItem(QGraphicsLineItem *m_item)
{
    item = m_item;
}

QGraphicsLineItem *gEdge::getItem()
{
    return item;
}

void gEdge::setPenBlack()
{
    pen.setColor(Qt::black);
}

void gEdge::setPenRed()
{
    pen.setColor(Qt::red);
}

QPen &gEdge::getPen()
{
    return pen;
}

void gEdge::setText(QGraphicsTextItem *m_txt)
{
    txt = m_txt;
}

QGraphicsTextItem *gEdge::getText()
{
    return txt;
}

void gEdge::setW(int m_W)
{
    W = m_W;
}

int gEdge::w()
{
    return W;
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
