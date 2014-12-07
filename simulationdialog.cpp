#include "simulationdialog.h"
#include "ui_simulationdialog.h"

#include <QDebug>


Prim *prim;
PrimSignal psignal;


int weight(unsigned u, unsigned v)
{
    foreach (gEdge *e, glob_edges)
    {
        if (e->getFrom()->id() == u || e->getTo()->id() == u)
        {
            if (e->getFrom()->id() == v || e->getTo()->id() == v)
                return e->w();
        }
    }

    return -1;
}

void sigEvent(unsigned event)
{
    psignal.qSigEvent(event);
}

void simulation(unsigned root)
{
    while (!ready)
        cv.wait(u_lock);
    if (sim_terminate)
        return;
    try {
        prim->PrimMinSpanningTree(weight, root);
    } catch (Prim::PrimException &e) {
        std::cerr << e.what();
        sigEvent(SIG_ERROR);
    }
}


SimulationDialog::SimulationDialog(unsigned m_root_id, QWidget *parent) :
    QDialog(parent), ui(new Ui::SimulationDialog)
{
    root_id = m_root_id;
    initSimulation();
    running = false;
    step_in_progress = false;
    connect(&psignal, SIGNAL(sig(unsigned)), this, SLOT(sig_backend(unsigned)));

    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    scene2 = new QGraphicsScene();
    ui->graphicsView_2->setScene(scene2);

    blackpen = QPen(Qt::black, 2, Qt::SolidLine);
    redpen = QPen(Qt::red, 2, Qt::SolidLine);

    drawGraph();

    ui->verticalSlider->setTickInterval(1);
    ui->verticalSlider->setRange(0, 3);
    ui->verticalSlider->setValue(0);

    ui->textEdit->setReadOnly(true);
    ui->textEdit->setCurrentFont(QFont("Courier", 10));
    initPrimCode();
    printGraph();
}

SimulationDialog::~SimulationDialog()
{
    sim_terminate = true;
    ready = true;
    cv.notify_one();

    delete ui;
    Simulation.join();

    delete prim;
    sim_terminate = false;
    ready = false;
}

void SimulationDialog::initSimulation()
{
    prim = new Prim();

    FibNodePtr ptr;
    foreach (gPlace *p, glob_places)
    {
        if ((ptr = prim->PrimAddVertex( p->id() )) == NULL)
            exitError("Error in backend: On adding FibNodePtr.");

        p->setFibNode(ptr);
    }

    foreach (gEdge *e, glob_edges)
    {
        try {
            prim->PrimAddEdge(e->getFrom()->id(), e->getTo()->id());
        } catch (Prim::PrimException &e) {
            exitError("Error in backend: On creating adjacency list.");
        }
    }

    Simulation = std::thread(simulation, root_id);
}

void SimulationDialog::exitError(QString msg)
{
    QMessageBox::information(0, "Simulator", msg);
    this->close();
}

// Run simulation
void SimulationDialog::on_pushButton_clicked()
{
    if (running || step_in_progress)
        return;

    ui->verticalSlider->setEnabled(false);

    shared_mtx.lock();
        mode = RUN;
    shared_mtx.unlock();

    running = true;
    prim_pos = 0;

    ready = true;
    cv.notify_one();
}

// Step forward
void SimulationDialog::on_pushButton_2_clicked()
{
    if (running || step_in_progress)
        return;

    if (prim->getPrimStatus())
    {
        QMessageBox::information(0, "Simulator", "Simulation finished.");
        return;
    }

    shared_mtx.lock();
        mode = STEP;
    shared_mtx.unlock();

    step_in_progress = true;
    prim_pos = 0;

    ready = true;
    cv.notify_one();
}

void SimulationDialog::sig_backend(unsigned event)
{
    switch (event)
    {
    case SIG_PRIM_STEP_FINISHED:
        if (running)
        {
            ready = true;
            cv.notify_one();
        }
        actualizeGraph();
        printGraph();
        break;
    case SIG_FIB_STEP_FINISHED:
        if (running)
        {
            ready = true;
            cv.notify_one();
        }
        break;
    case SIG_FINISHED_ALL:
        ui->verticalSlider->setEnabled(true);
        running = false;
        QMessageBox::information(0, "Simulator", "Simulation finished.");

        qDebug() << "Min.: " << prim->getPrimMSTCost();

        Simulation.join();
        delete prim;
        initSimulation();
        break;
    case SIG_ERROR:
        exitError("Simulation error!");
        break;
    default:
        break;
    }

    step_in_progress = false;
}

void SimulationDialog::drawGraph()
{
    scene->clear();

    foreach (gPlace *p, glob_places)
    {
        QGraphicsTextItem *txt = new QGraphicsTextItem(0, scene);
        txt->setFont(QFont("Helvetica", 12, QFont::Bold));
        txt->setZValue(3);
        txt->setPos(p->x(), p->y());
        txt->setPlainText(QString::number(p->id()));
        p->setText(txt);

        QGraphicsEllipseItem *el = new QGraphicsEllipseItem(p->x(), p->y(),
                                                            diameter, diameter,
                                                            0, scene);
        el->setPen(blackpen);
        // makes places to be drawn on the top of edges
        el->setZValue(2);
        el->setBrush(p->getBrush());
        p->setItem(el);
    }

    foreach (gEdge *e, glob_edges)
    {
        QLineF li(QPointF(e->getFrom()->x()+radius, e->getFrom()->y()+radius),
                  QPointF(e->getTo()->x()+radius, e->getTo()->y()+radius));
        line = new QGraphicsLineItem(li, 0, scene);
        line->setPen(e->getPen());
        e->setItem(line);

        QGraphicsTextItem *txt = new QGraphicsTextItem(0, scene);
        txt->setDefaultTextColor(Qt::blue);
        txt->setFont(QFont("Helvetica", 14, QFont::Bold));
        txt->setZValue(3);
        qreal x = e->getItem()->boundingRect().x()
                  + (e->getItem()->boundingRect().width() / 2) - 5;
        qreal y = e->getItem()->boundingRect().y()
                  + (e->getItem()->boundingRect().height() / 2) - 8;
        txt->setPos(x, y);
        txt->setPlainText(QString::number(e->w()));
        e->setText(txt);
    }
}

// Text of Prim algorithm
void SimulationDialog::initPrimCode()
{
    prim_pos = 0;
    prim_code << QString::fromUtf8("PRIM(G,w,r)")
              << QString::fromUtf8("1    for each u \u03f5 V")
              << QString::fromUtf8("2        do key[u] \u2190 \u221e")
              << QString::fromUtf8("3           \u03c0[u]   \u2190 NIL")
              << QString::fromUtf8("4    key[r] \u2190 0")
              << QString::fromUtf8("5    Q \u2190 V")
              << QString::fromUtf8("6    while Q != \u2205")
              << QString::fromUtf8("7          do u \u2190 EXTRACT-MIN(Q)")
              << QString::fromUtf8("8             if \u03c0[u] != NIL")
              << QString::fromUtf8("9                then A = A \u22c3 {(u,\u03c0[u])}")
              << QString::fromUtf8("10            for each v \u03f5 Adj[u]")
              << QString::fromUtf8("11                do if v \u03f5 Q and w(u,v) < key[v]")
              << QString::fromUtf8("12                      then \u03c0[v]   \u2190 u")
              << QString::fromUtf8("13                           key[v] \u2190 w(u,v)");
}

void SimulationDialog::actualizeGraph()
{
    unsigned from, to;
    EdgeSet set = prim->getPrimMST();
    std::tuple<unsigned, unsigned>tpl = set.back();
    std::tie(from, to) = tpl;

    foreach (gEdge *e, glob_edges)
    {
        if (e->getFrom()->id() == from && e->getTo()->id() == to)
        {
            e->setPenRed();
            e->getFrom()->setBrushGray();
        }
    }
}

void SimulationDialog::printGraph()
{
    prim_pos++;
    if (prim_pos >= prim_code.size())
        return;

    ui->textEdit->clear();
    int i = 0;
    foreach (QString str, prim_code)
    {
        if (prim_pos == i++)
        {
            ui->textEdit->setTextColor(QColor(Qt::red));
            ui->textEdit->append(str);
            ui->textEdit->setTextColor(QColor(Qt::black));
        }
        else
        {
            ui->textEdit->append(str);
        }
    }
}

void SimulationDialog::on_verticalSlider_valueChanged(int value)
{
    shared_mtx.lock();
    speed = pow(2, value);
    shared_mtx.unlock();
}
