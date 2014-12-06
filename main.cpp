#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <mutex>
#include <thread>
#include "gplace.h"
#include "gedge.h"
#include "Prim.h"


bool pause_execution;
unsigned char speed;
std::mutex shared_mtx;
std::mutex uni_mtx;

void prim()
{
    std::cout << "Hello from prim()\n";
}

int main(int argc, char *argv[])
{
    int gui_ret;


    std::thread Prim(prim);


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    gui_ret = a.exec();


    Prim.join();


    return gui_ret;
}
