#ifndef GPLACE_H
#define GPLACE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>


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
    void setBrushBlack();
    void setBrushGray();
    void setText(QGraphicsTextItem *m_txt);
    QGraphicsEllipseItem *getItem();
    QBrush &getBrush();
    QGraphicsTextItem *getText();

private:
    unsigned ID;
    qreal X;
    qreal Y;
    QGraphicsEllipseItem *item;
    QBrush brush;
    QGraphicsTextItem *txt;
    // ptr to FibbNode
};


extern std::list<gPlace*> glob_places;


#endif // GPLACE_H
