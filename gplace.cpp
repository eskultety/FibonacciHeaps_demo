#include "gplace.h"


std::list<gPlace*> glob_places;


gPlace::gPlace()
{
    item = NULL;
    txt = NULL;
    brush = QBrush(Qt::yellow);
    node = NULL;
}

gPlace::gPlace(unsigned m_ID)
{
    ID = m_ID;
    item = NULL;
    txt = NULL;
    brush = QBrush(Qt::yellow);
    node = NULL;
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

void gPlace::setBrushYellow()
{
    brush.setColor(Qt::yellow);
}

void gPlace::setBrushGreen()
{
    brush.setColor(Qt::green);
}

void gPlace::setText(QGraphicsTextItem *m_txt)
{
    txt = m_txt;
}

void gPlace::setKeyText(QGraphicsTextItem *m_txt_key)
{
    txt_key = m_txt_key;
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

QGraphicsTextItem *gPlace::getKeyText()
{
    return txt_key;
}

void gPlace::setFibNode(FibNodePtr m_node)
{
    node = m_node;
}

FibNodePtr gPlace::getFibNode()
{
    return node;
}
