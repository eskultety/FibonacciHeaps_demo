#include "simulationdialog.h"
#include "ui_simulationdialog.h"


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

void sigEvent(int sig, unsigned u, unsigned v)
{
    psignal.qSigEvent(sig, u, v);
}

void simulation(unsigned root)
{
    std::unique_lock<std::mutex> u_lock(uni_mtx);
    while (!ready && !sim_terminate)
        cv.wait(u_lock);
    ready = false;
    if (sim_terminate)
        return;
    u_lock.unlock();
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
    stepping = false;
    connect(&psignal, SIGNAL(sig(int)), this, SLOT(sig_backend(int)));

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
    initPrimCode();
    printPrimCode(false);
}

SimulationDialog::~SimulationDialog()
{
    continueSimulation(false);

    delete ui;
    Simulation->join();
    delete Simulation;

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

    prim_pos = 0;
    Simulation = new std::thread(simulation, root_id);
}

void SimulationDialog::continueSimulation(bool cont)
{
    std::unique_lock<std::mutex> u_lock(uni_mtx);
    if (!cont)
        sim_terminate = true;
    ready = true;
    cv.notify_one();
    u_lock.unlock();
}

void SimulationDialog::exitError(QString msg)
{
    QMessageBox::information(0, "Simulator", msg);
    this->close();
}

void SimulationDialog::drawHeapNode(FibNodePtr fb, qreal x, qreal y)
{
    QString tmp_key;
    QGraphicsTextItem *txt_key = new QGraphicsTextItem(0, scene2);
    txt_key->setFont(QFont("Helvetica", 12, QFont::Bold));
    txt_key->setZValue(3);
    txt_key->setPos(x, y);
    if (fb->key == INT_MIN)
        tmp_key = QString();
    else if (fb->key == INT_MAX)
        tmp_key = QString::fromUtf8("\u221e");
    else
        tmp_key = QString::number(fb->key);
    txt_key->setPlainText(tmp_key);

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem(x, y,
                                                        diameter, diameter,
                                                        0, scene2);
    el->setPen(blackpen);
    el->setZValue(2);
    if (fb->mark)
        el->setBrush(QBrush(Qt::darkCyan));
    else
        el->setBrush(QBrush(Qt::yellow));
}

void SimulationDialog::drawHeapLine(bool dashed, qreal x1, qreal y1,
                                    qreal x2, qreal y2)
{
    QLineF li(QPointF(x1, y1), QPointF(x2, y2));
    line = new QGraphicsLineItem(li, 0, scene2);
    QPen pen = QPen(Qt::black, 2, Qt::SolidLine);
    if (dashed)
        pen.setStyle(Qt::DashLine);
    line->setPen(pen);
}

qreal SimulationDialog::drawHeapNeighbours(FibNodePtr fb, qreal max_x, qreal y)
{
    qreal x = max_x;
    FibNodePtr begin = fb;
    FibNodePtr ptr = fb->right;

    // draw all neighbours of fb children except fb neighbours
    while (ptr != begin)
    {
        x += shift;
        drawHeapLine(false, x+radius, y+radius, x-shift+radius, y+radius);
        drawHeapNode(ptr, x, y);
        ptr = ptr->right;
    }

    return x;
}

qreal SimulationDialog::drawHeapTree(FibNodePtr fb, qreal x)
{
    qreal x_shift = x;
    qreal x_local_max;
    qreal y = shift;

    FibNodePtr last_child = NULL;
    FibNodePtr ptr = fb;

    // draw fb and all its children
    while (ptr != NULL)
    {
        drawHeapNode(ptr, x, y);

        last_child = ptr;
        ptr = ptr->child;
        y += shift;
    }

    y -= shift;
    ptr = last_child;
    while (ptr != fb)
    {
        drawHeapLine(false, x+radius, y+radius, x+radius, y-shift+radius);
        x_local_max = drawHeapNeighbours(ptr, x, y);
        if (x_local_max > x_shift)
            x_shift = x_local_max;
        ptr = ptr->parent;
        y -= shift;
    }

    return x_shift;
}

void SimulationDialog::drawHeap(FibNodePtr min)
{
    if (min == NULL)
    {
        scene2->clear();
        return;
    }

    scene2->clear();
    qreal max_x = shift;
    qreal y = shift;
    FibNodePtr ptr = min;

    do
    {
        if (ptr != min)
            drawHeapLine(true, max_x+radius, y+radius,
                         max_x-shift+radius, y+radius);
        max_x = drawHeapTree(ptr, max_x);
        max_x += shift;
        ptr = ptr->right;
    } while (ptr != min);
}

// Run simulation
void SimulationDialog::on_pushButton_clicked()
{
    if (running || step_in_progress)
        return;

    if (!stepping)
    {
        resetColors();
        drawGraph();
        scene2->clear();
    }

    ui->verticalSlider->setEnabled(false);

    shared_mtx.lock();
        mode = RUN;
    shared_mtx.unlock();

    running = true;

    continueSimulation();
}

// Step forward
void SimulationDialog::on_pushButton_2_clicked()
{
    if (running || step_in_progress)
        return;

    if (!stepping)
    {
        stepping = true;
        resetColors();
        drawGraph();
        scene2->clear();
    }

    shared_mtx.lock();
        mode = STEP;
    shared_mtx.unlock();

    step_in_progress = true;

    continueSimulation();
}

void SimulationDialog::sig_backend(int signum)
{
    unsigned u = psignal.getU();
    unsigned v = psignal.getV();

    switch (signum)
    {
    case SIG_PRIM_STEP_FINISHED:
        drawGraph();
        if (running)
            continueSimulation();
        break;
    case SIG_FIB_STEP_FINISHED:
        drawHeap(prim->PrimGetHeapMin());
        if (running)
            continueSimulation();
        break;
    case SIG_FINISHED_ALL:
        ui->verticalSlider->setEnabled(true);
        running = false;
        stepping = false;
        initPrimCode();
        printPrimCode(false);
        QMessageBox::information(0, "Simulator", "Simulation finished.");

        std::cerr << "Min.: " << prim->PrimGetMSTCost() << std::endl;

        Simulation->join();
        delete Simulation;
        delete prim;
        initSimulation();
        break;
    case SIG_MIN_EXTRACTED:
        actualizeGraph(u, v);
        if (running)
            continueSimulation();
        break;
    case SIG_MST_UPDATED:
        actualizeGraph(u, v);
        if (running)
            continueSimulation();
        break;
    case SIG_NEXT_LINE:
        printPrimCode(true);
        if (running)
            continueSimulation();
        break;
    case SIG_ERROR:
        exitError("Simulation error!");
        break;
    default:
        if (running)
            continueSimulation();
        break;
    }

    step_in_progress = false;
}

void SimulationDialog::drawGraph()
{
    scene->clear();

    foreach (gPlace *p, glob_places)
    {
        QString tmp_key;
        QGraphicsTextItem *txt_key = new QGraphicsTextItem(0, scene);
        txt_key->setFont(QFont("Helvetica", 12, QFont::Bold));
        txt_key->setZValue(3);
        txt_key->setPos(p->x()+6, p->y()+1);
        if (p->getFibNode()->key == INT_MIN)
            tmp_key = QString();
        else if (p->getFibNode()->key == INT_MAX)
            tmp_key = QString::fromUtf8("\u221e");
        else
            tmp_key = QString::number(p->getFibNode()->key);
        txt_key->setPlainText(tmp_key);
        p->setKeyText(txt_key);

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

void SimulationDialog::actualizeGraph(unsigned u, unsigned v)
{
    if (u != UINT_MAX && v != UINT_MAX)
    {
        foreach (gEdge *e, glob_edges)
        {
            if (e->getFrom()->id() == u && e->getTo()->id() == v)
                e->setPenRed();
            if (e->getFrom()->id() == v && e->getTo()->id() == u)
                e->setPenRed();
        }
        drawGraph();
    }
    else if (u != UINT_MAX)
    {
        foreach (gPlace *p, glob_places)
        {
            if (p->id() == u)
                p->setBrushGreen();
        }
        drawGraph();
    }
    else
        return;
}

// Text of Prim algorithm
void SimulationDialog::initPrimCode()
{
    prim_pos = 0;
    shared_mtx.lock();
    cur_line = prim_pos;
    shared_mtx.unlock();
}

void SimulationDialog::printPrimCode(bool actualize)
{
    if (actualize)
    {
        GET_LINE(prim_pos);
    }

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

void SimulationDialog::resetColors()
{
    foreach (gEdge *e, glob_edges)
    {
        e->getFrom()->setBrushYellow();
        e->getTo()->setBrushYellow();
        e->setPenBlack();
    }
}

void SimulationDialog::on_verticalSlider_valueChanged(int value)
{
    shared_mtx.lock();
    speed = pow(2, value);
    shared_mtx.unlock();
}
