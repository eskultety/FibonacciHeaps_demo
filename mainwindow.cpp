#include "mainwindow.h"
#include "ui_mainwindow.h"


Prim prim;
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

void simulation()
{
    while (!ready)
        cv.wait(u_lock);
    if (terminate)
        return;
    try {
        prim.PrimMinSpanningTree(weight, 0);
    } catch (Prim::PrimException& e) {
        std::cerr << e.what();
        sigEvent(SIG_ERROR);
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    place_button_checked = edge_button_checked = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Place
void MainWindow::on_pushButton_clicked()
{
    if (place_button_checked)
    {
        ui->graphicsView->setDrawType(NO_TYPE);
        ui->pushButton->setChecked(false);
        place_button_checked = false;
    }
    else
    {
        ui->graphicsView->setDrawType(PLACE);
        ui->pushButton_2->setChecked(false);
        edge_button_checked = false;
        ui->pushButton->setChecked(true);
        place_button_checked = true;
    }
}

// Edge
void MainWindow::on_pushButton_2_clicked()
{
    if (edge_button_checked)
    {
        ui->graphicsView->setDrawType(NO_TYPE);
        ui->pushButton_2->setChecked(false);
        edge_button_checked = false;
    }
    else
    {
        ui->graphicsView->setDrawType(EDGE);
        ui->pushButton->setChecked(false);
        place_button_checked = false;
        ui->pushButton_2->setChecked(true);
        edge_button_checked = true;
    }
}

// Clear
void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Editor", "Really clear all?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
        ui->graphicsView->clearAll();
}

// Save
void MainWindow::on_actionSave_triggered()
{
    fio.saveToFile();
}

// Load
void MainWindow::on_actionLoad_triggered()
{
    fio.LoadFromFile();
    if (fio.graphChanged())
    {
        ui->graphicsView->clearView();
        ui->graphicsView->redrawView();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if (glob_places.empty() || glob_edges.empty())
    {
        QMessageBox::information(0, "Simulator", "No graph to simulate on.");
        return;
    }

    std::thread Simulation(simulation);

    simdlg = new SimulationDialog(&prim, &psignal, this);
    simdlg->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint
                           | Qt::WindowMaximizeButtonHint
                           | Qt::WindowCloseButtonHint);
    simdlg->exec();

    terminate = true;
    ready = true;
    cv.notify_one();

    Simulation.join();

    delete simdlg;
    ui->graphicsView->clearView();
    ui->graphicsView->redrawView();
}
