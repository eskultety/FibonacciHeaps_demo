#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "gplace.h"
#include "gedge.h"
#include "interface.h"
#include "Prim.h"


bool pause_execution;
bool ready;
bool terminate;
unsigned char speed;
std::mutex shared_mtx;
std::mutex uni_mtx;
std::condition_variable cv;
std::unique_lock<std::mutex> u_lock(uni_mtx);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
