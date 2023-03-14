#include <QtGui>
#include <QApplication>
#include <iostream>

#include "mainWindow.h"

using namespace std;

/*
    Custom message handler.
*/
void myMessageHandler(QtMsgType type, const char *msg) {
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("%1\n").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1\n").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1\n").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1\n").arg(msg);
        abort();
        break;
    }
    cout << qPrintable(txt);
}

int main(int argc, char *argv[]) {
    qInstallMsgHandler(myMessageHandler);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/ico.png"));

    PSQP *psqp = new PSQP();
    // Run in ui
    if (argc == 1) {
        MainWindow *mainWindow = new MainWindow(psqp);
        mainWindow->show();
        return app.exec();
    }

    // Run in cli
    if (argc < 7) {
        std::cout << "Usage: PSQP <input image> <number of columns> "
                                 "<number of rows> <descriptor>[Pomeranz|Gallagher] "
                                 "<parameter p> <parameter q>" << '\n';
        return -1;
    }

    // Read arguments
    QString inputImage = argv[1];
    int ncols = atoi(argv[2]);
    int nrows = atoi(argv[3]);
    QString desc = argv[4];
    int paramP = atof(argv[5]);
    int paramQ = atof(argv[6]);

    // Set puzzle parameters and run PSQP
    psqp->setImage(inputImage);
    psqp->setPuzzleSize(ncols, nrows);
    psqp->setDescriptor(desc, paramP, paramQ);
    psqp->getTiledImage()->permutTiles();
    psqp->runSolver();
    psqp->optimizeShift();
    psqp->optimizeShift();
    psqp->optimizeShift();
    delete psqp;
    return 0;
}
