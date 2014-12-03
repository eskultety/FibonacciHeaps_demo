#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "myqgraphicsview.h"
#include "xmlfileio.h"
#include "simulationdialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    MyQGraphicsView *view;
    XmlFileIO fio;
    SimulationDialog *simdlg;

    bool place_button_checked;
    bool edge_button_checked;
};

#endif // MAINWINDOW_H
