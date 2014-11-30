#include "gplace.h"

std::list<gPlace*> glob_places;

gPlace::gPlace()
{
    item = NULL;
}

gPlace::gPlace(unsigned m_ID)
{
    ID = m_ID;
}

void gPlace::setX(qreal m_X)
{
    X = m_X;
}

void gPlace::setY(qreal m_Y)
{
    Y = m_Y;
}

qreal gPlace::x()
{
    return X;
}

qreal gPlace::y()
{
    return Y;
}

void gPlace::setID(unsigned m_ID)
{
    ID = m_ID;
}

unsigned gPlace::id()
{
    return ID;
}

void gPlace::setItem(QGraphicsEllipseItem *m_item)
{
    item = m_item;
}

void gPlace::setText(QGraphicsTextItem *m_txt)
{
    txt = m_txt;
}

QGraphicsEllipseItem *gPlace::getItem()
{
    return item;
}

QGraphicsTextItem *gPlace::getText()
{
    return txt;
}
