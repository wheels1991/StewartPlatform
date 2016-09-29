#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SixJoints.h"
#include <QMainWindow>
#include "SerialPort.h"

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
    void setSerialPort();
    void setSteerPlatform();
    void setStepperPlatform();
private:
    void createActions();
    void createToolBar();

    Ui::MainWindow *ui;
//    SixJoints *sixJoints;
    SixJoints *sixJointsForSteerPlatform;
    SixJoints *sixJointsForStepperPlatform;
    SerialPort *serialPort;

    QDockWidget *serialDock;
    QDockWidget *steerPlatformDock;
    QDockWidget *stepperPlatformDock;

    QAction *serialPortAction;
    QAction *steerPlatformAction;
    QAction *stepperPlatformAction;

    QToolBar *toolBar;
};

#endif // MAINWINDOW_H
