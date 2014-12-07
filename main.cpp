#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "gplace.h"
#include "gedge.h"
#include "interface.h"
#include "Prim.h"


runMode mode = RUN;
bool ready;
bool sim_terminate;
unsigned char speed = 1;
unsigned cur_line;
std::mutex shared_mtx;
std::mutex uni_mtx;
std::condition_variable cv;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
