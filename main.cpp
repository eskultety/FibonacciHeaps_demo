#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <mutex>
#include <thread>
#include "gplace.h"
#include "gedge.h"


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
