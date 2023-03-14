#include "mainWindow.h"
#include <ctime>

MainWindow::MainWindow(PSQP *psqp) :
        QMainWindow(), ui(new Ui::MainWindow) {
    this->psqp = psqp;
    graphicsView = NULL;
    graphicsScene = NULL;

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("PSQP");
    createMainFrame();
    createActions();
    createDock();
    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {
    delete ui;
    delete mainFrame;
}

void MainWindow::save() {
    if (graphicsScene == NULL)
        return;

    int w = graphicsScene->width();
    int h = graphicsScene->height();

    QImage *image = new QImage(w, h, QImage::Format_RGB32);
    QPainter *p = new QPainter(image);
    p->setRenderHint(QPainter::Antialiasing);
    graphicsScene->render(p, QRectF(0, 0, w, h), QRectF(0, 0, w, h));
    p->end();

    // Save it..
    image->save("graphicsscene.png", "PNG");
}

void MainWindow::open() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"),
            QDir::currentPath());
    if (!filename.isEmpty())
        loadImage(filename);
}

void MainWindow::loadImage(QString filename) {
    QImage file(filename);
    if (file.isNull()) {
        QMessageBox::information(this, tr("Load Warning"),
                tr("Cannot read file %1.").arg(filename));
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    psqp->setImage(filename);

    if (graphicsScene != NULL) {
        graphicsScene->deleteLater();
        graphicsView->hide();
        resetPuzzleButton->setEnabled(false);
        randomGridButton->setEnabled(false);
        quadProgButton->setEnabled(false);
        shiftButton->setEnabled(false);
    }

    graphicsScene = new GraphicsScene(graphicsView);
    updatePuzzle();

    statusBar()->showMessage(tr("File loaded"), 2000);
    QApplication::restoreOverrideCursor();
}

void MainWindow::createMainFrame() {
    mainFrame = new QFrame(this);
    mainFrame->setFrameShape(QFrame::StyledPanel);
    mainFrame->setFrameShadow(QFrame::Raised);
    ui->centralLayout->addWidget(mainFrame, true);
    mainFrame->showMaximized();

    graphicsView = new QGraphicsView(mainFrame);
    graphicsView->viewport()->setProperty("cursor",
            QVariant(QCursor(Qt::CrossCursor)));
    graphicsView->setAcceptDrops(false);

    QHBoxLayout *hLayout = new QHBoxLayout(mainFrame);
    hLayout->addWidget(graphicsView);

}

void MainWindow::createActions() {
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    ui->actionSave->setShortcuts(QKeySequence::Save);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
}

void MainWindow::createDock() {
    dock = new QDockWidget(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dock->setSizePolicy(sizePolicy);
    // Remove buttons from dock
    dock->setFeatures(dock->features() & ~QDockWidget::DockWidgetClosable 
                                       & ~QDockWidget::DockWidgetFloatable 
                                       & ~QDockWidget::DockWidgetMovable);
    this->addDockWidget(Qt::LeftDockWidgetArea, dock);

    QWidget *w = new QWidget;
    QVBoxLayout *vBox = new QVBoxLayout(w);

    vBox->addWidget(new QLabel(tr("Puzzle size"), dock));

    hGridSpin = new QSpinBox;
    hGridSpin->setValue(5);
    hGridSpin->setRange(0, 5000);
    wGridSpin = new QSpinBox;
    wGridSpin->setValue(5);
    wGridSpin->setRange(0, 5000);
    descriptorCombo = new QComboBox(this);
    descriptorCombo->addItems(psqp->getDescriptorNames());
    pSpin = new QDoubleSpinBox;
    pSpin->setValue(1.0);
    pSpin->setRange(0.0, 50.0);
    pSpin->setDecimals(2);
    qSpin = new QDoubleSpinBox;
    qSpin->setValue(6.0);
    qSpin->setRange(0.0, 50.0);
    qSpin->setDecimals(2);
    QWidget *w2 = new QWidget;
    QHBoxLayout *hBox = new QHBoxLayout(w2);
    hBox->addWidget(wGridSpin);
    hBox->addWidget(new QLabel(tr("x"), dock));
    hBox->addWidget(hGridSpin);
    vBox->addWidget(w2);
    vBox->addWidget(new QLabel(tr("Compatibility metric"), dock));
    vBox->addWidget(descriptorCombo);
    QWidget *w3 = new QWidget;
    QHBoxLayout *hBox2 = new QHBoxLayout(w3);
    vBox->addWidget(new QLabel(tr("P / Q"), dock));
    hBox2->addWidget(pSpin);
    hBox2->addWidget(new QLabel(tr(" / "), dock));
    hBox2->addWidget(qSpin);
    vBox->addWidget(w3);

    resetPuzzleButton = new QPushButton("Reset puzzle");
    resetPuzzleButton->setEnabled(false);
    resetPuzzleButton->connect(resetPuzzleButton, SIGNAL(clicked()), this,
            SLOT(updatePuzzle()));
    vBox->addWidget(resetPuzzleButton);

    randomGridButton = new QPushButton("Randomize puzzle");
    randomGridButton->setEnabled(false);
    connect(randomGridButton, SIGNAL(clicked()), this, SLOT(permutTiles()));
    vBox->addWidget(randomGridButton);

    quadProgButton = new QPushButton("Solve puzzle");
    quadProgButton->setEnabled(false);
    connect(quadProgButton, SIGNAL(clicked()), this, SLOT(solvePuzzle()));
    vBox->addWidget(quadProgButton);

    shiftButton = new QPushButton("Optimize shift");
    shiftButton->setEnabled(false);
    connect(shiftButton, SIGNAL(clicked()), this, SLOT(optimizeShift()));
    vBox->addWidget(shiftButton);

    QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum,
            QSizePolicy::Expanding);
    vBox->addItem(spacer);

    dock->setWidget(w);
    dock->show();
}

void MainWindow::updatePuzzle() {
    graphicsScene->invalidate();
    ncols = wGridSpin->value();
    nrows = hGridSpin->value();
    paramP = pSpin->value();
    paramQ = qSpin->value();
    psqp->setPuzzleSize(ncols, nrows);
    psqp->setDescriptor(descriptorCombo->currentText(), paramP, paramQ);
    statusBar()->showMessage(tr("Puzzle created"), 2000);

    graphicsScene->showTiledImage(psqp->getTiledImage());
    graphicsView->setScene(graphicsScene);
    graphicsView->centerOn(graphicsScene->getCenter());
    graphicsView->show();

    resetPuzzleButton->setEnabled(true);
    randomGridButton->setEnabled(true);
    quadProgButton->setEnabled(true);
    shiftButton->setEnabled(true);
    graphicsView->fitInView(graphicsScene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::permutTiles() {
    psqp->getTiledImage()->permutTiles();
    statusBar()->showMessage(tr("Puzzle randomized"), 2000);
    graphicsScene->showTiledImage(psqp->getTiledImage());
}

void MainWindow::solvePuzzle() {
    graphicsScene->invalidate();
    psqp->runSolver();
    graphicsScene->showTiledImage(psqp->getTiledImage());
}

void MainWindow::optimizeShift() {
    graphicsScene->invalidate();
    psqp->optimizeShift();
    statusBar()->showMessage(tr("Shift optimized"), 2000);
    graphicsScene->showTiledImage(psqp->getTiledImage());
}
