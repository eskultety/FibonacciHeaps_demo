#include "gplace.h"

std::list<gPlace*> glob_places;

gPlace::gPlace()
{
    item = NULL;
    txt = NULL;
    brush = QBrush(Qt::yellow);
}

gPlace::gPlace(unsigned m_ID)
{
    ID = m_ID;
    item = NULL;
    txt = NULL;
    brush = QBrush(Qt::yellow);
}

gPlace::~gPlace()
{
    delete item;
    delete txt;
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

void gPlace::setBrushBlack()
{
    brush.setColor(Qt::black);
}

void gPlace::setBrushGray()
{
    brush.setColor(Qt::gray);
}

void gPlace::setText(QGraphicsTextItem *m_txt)
{
    txt = m_txt;
}

QGraphicsEllipseItem *gPlace::getItem()
{
    return item;
}

QBrush &gPlace::getBrush()
{
    return brush;
}

QGraphicsTextItem *gPlace::getText()
{
    return txt;
}
