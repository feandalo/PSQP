#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_mainWindow.h"
#include "ui/graphicsScene.h"
#include "psqp.h"

namespace Ui {
    class MainWindow;
}

/**
 * @brief App's main window.
 */
class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Main window constructor.
     * @param psqp Puzzle solver object.
     */
    MainWindow(PSQP *psqp);
    ~MainWindow();

private slots:
    /**
     * @brief Slot to update puzzle size.
     */
    void updatePuzzle();
    /**
     * @brief Slot to solve puzzle.
     */
    void solvePuzzle();
    /**
     * @brief Slot to ramdonly permut tiles.
     */
    void permutTiles();
    /**
     * @brief Slot to optimize shift of current solution.
     */
    void optimizeShift();

    /**
     * @brief Slot to open new image.
     */
    void open();

    /**
     * @brief Slot to save current image.
     */
    void save();

private:
    /**
     * @brief Create app's main frame.
     */
    void createMainFrame();
    
    /**
     * @brief Create actions for app's menu buttons.
     */
    void createActions();

    /**
     * @brief Create app's left dock for puzzle params and actions.
     */
    void createDock();
    /**
     * @brief Load selected image.
     */
    void loadImage(QString filename);

    /**
     * Puzzle solver object.
     */
    PSQP *psqp;

    /**
     * UI components.
     */
    QFrame *mainFrame;
    QDockWidget *dock;
    QGraphicsView *graphicsView;
    QPushButton *resetPuzzleButton, *randomGridButton, *quadProgButton, *shiftButton;
    QSpinBox *hGridSpin, *wGridSpin;
    QDoubleSpinBox *pSpin, *qSpin;
    QComboBox *descriptorCombo;
    GraphicsScene *graphicsScene;
    
    Ui::MainWindow *ui;

    /**
     * Information about puzzle.
     */
    int ncols, nrows;
    float paramP, paramQ;
};

#endif // MAINWINDOW_H
