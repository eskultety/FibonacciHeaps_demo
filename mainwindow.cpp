#include "mainwindow.h"
#include "ui_mainwindow.h"

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
        ui->graphicsView->clearView();
}
