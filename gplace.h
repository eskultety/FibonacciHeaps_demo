#ifndef GPLACE_H
#define GPLACE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>


class gPlace
{
public:
    gPlace();
    gPlace(unsigned m_ID);

    void setX(qreal m_X);
    void setY(qreal m_Y);
    qreal x();
    qreal y();

    void setID(unsigned m_ID);
    unsigned id();

    void setItem(QGraphicsEllipseItem *m_item);
    void setText(QGraphicsTextItem *m_txt);
    QGraphicsEllipseItem *getItem();
    QGraphicsTextItem *getText();

private:
    unsigned ID;
    qreal X;
    qreal Y;
    QGraphicsEllipseItem *item;
    QGraphicsTextItem *txt;
    // ptr to FibbNode
};


extern std::list<gPlace*> glob_places;


#endif // GPLACE_H
