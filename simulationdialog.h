#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPointF>
#include <QMessageBox>
#include <cmath>
#include "interface.h"
#include "Prim.h"
#include "primsignal.h"
#include "gplace.h"
#include "gedge.h"


int weight(unsigned u, unsigned v);
void simulation(unsigned root);


namespace Ui {
class SimulationDialog;
}

class SimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationDialog(unsigned m_root_id, QWidget *parent = 0);
    ~SimulationDialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void sig_backend(int signum);
    void on_verticalSlider_valueChanged(int value);
    void sigExit();

private:
    void initSimulation();
    void continueSimulation(bool cont=true);
    void exitError(QString msg);
    void drawHeapNode(FibNodePtr fb, qreal x, qreal y);
    void drawHeapLine(bool dashed, qreal x1, qreal y1, qreal x2, qreal y2);
    qreal drawHeapNeighbours(FibNodePtr fb, qreal max_x, qreal y);
    qreal drawHeapTree(FibNodePtr fb, qreal x);
    void drawHeap(FibNodePtr min);
    void drawGraph();
    void actualizeGraph(unsigned u, unsigned v);
    void initPrimCode();
    void printPrimCode(bool actualize);
    void resetColors();

    std::thread *Simulation;
    bool running;
    bool step_in_progress;
    bool stepping;
    bool thread_finished;
    unsigned root_id;

    QStringList prim_code;
    int prim_pos;

    Ui::SimulationDialog *ui;
    QGraphicsScene *scene;
    QGraphicsScene *scene2;
    QPen blackpen;
    QPen redpen;

    QGraphicsLineItem *line;
    QLineF *new_line;
    const qreal diameter = 30;
    const qreal radius = 15;
    const qreal shift = 70;
};

#endif // SIMULATIONDIALOG_H
