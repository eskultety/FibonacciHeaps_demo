#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPointF>
#include "gplace.h"
#include "gedge.h"

namespace Ui {
class SimulationDialog;
}

class SimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationDialog(QWidget *parent = 0);
    ~SimulationDialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    void drawGraph();

    QStringList prim_code;
    int prim_pos;
    void initPrimCode();
    void printPrim();

    Ui::SimulationDialog *ui;
    QGraphicsScene *scene;
    QGraphicsScene *scene2;
    QPen blackpen;
    QPen redpen;

    QGraphicsLineItem *line;
    QLineF *new_line;
    const qreal diameter = 30;
    const qreal radius = 15;
};

#endif // SIMULATIONDIALOG_H