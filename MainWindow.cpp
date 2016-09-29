#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SixJoints.h"
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createToolBar();

//    sixJoints = new SixJoints(NULL, Platform::SteerMotor);
//    setCentralWidget(sixJoints);


    //将串口设置窗口设置成一个dock
    serialPort = new SerialPort;
    serialDock = new QDockWidget(this);
    serialDock->setWidget(serialPort);
    serialDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    serialDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    serialDock->close();
    setCentralWidget(serialDock);

    sixJointsForStepperPlatform = new SixJoints(NULL, Platform::StepperMotor);
    stepperPlatformDock = new QDockWidget(this);
    stepperPlatformDock->setWindowTitle(QString("Classical Structure"));
    stepperPlatformDock->setWidget(sixJointsForStepperPlatform);
    stepperPlatformDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    stepperPlatformDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    stepperPlatformDock->close();

    sixJointsForSteerPlatform = new SixJoints(NULL, Platform::SteerMotor);
    steerPlatformDock = new QDockWidget(this);
    steerPlatformDock->setWindowTitle(QString("Link Structure"));
    steerPlatformDock->setWidget(sixJointsForSteerPlatform);
    steerPlatformDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    steerPlatformDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    steerPlatformDock->close();

    //串口连接
    connect(sixJointsForStepperPlatform, SIGNAL(SendDataBySerial(const char*)), serialPort, SLOT(sendData(const char*)));
    connect(sixJointsForSteerPlatform, SIGNAL(SendDataBySerial(const char*)), serialPort, SLOT(sendData(const char*)));

    //发送udp报文
    connect(sixJointsForStepperPlatform, SIGNAL(SendMessage(QString)), serialPort, SLOT(sendMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief MainWindow::createToolBar     设置工具栏
 */
void MainWindow::createToolBar()
{
    toolBar = addToolBar(tr("&Set"));
    toolBar->addAction(serialPortAction);
    toolBar->addAction(steerPlatformAction);
    toolBar->addAction(stepperPlatformAction);
}
/*!
 * \brief MainWindow::createActions     创建QAction
 */
void MainWindow::createActions()
{
    serialPortAction = new QAction(tr("SerialPort"), this);
    connect(serialPortAction, SIGNAL(triggered(bool)), this, SLOT(setSerialPort()));
    steerPlatformAction = new QAction(tr("Link"), this);
    connect(steerPlatformAction, SIGNAL(triggered(bool)), this, SLOT(setSteerPlatform()));
    stepperPlatformAction = new QAction(tr("Classical"), this);
    connect(stepperPlatformAction, SIGNAL(triggered(bool)), this, SLOT(setStepperPlatform()));
}
/*!
 * \brief MainWindow::setSerialPort     打开串口设置功能
 */
void MainWindow::setSerialPort()
{
    addDockWidget(Qt::LeftDockWidgetArea, serialDock);
    serialDock->show();
}

void MainWindow::setSteerPlatform()
{
    addDockWidget(Qt::RightDockWidgetArea, steerPlatformDock);
    stepperPlatformDock->hide();
    steerPlatformDock->show();
}

void MainWindow::setStepperPlatform()
{
    addDockWidget(Qt::RightDockWidgetArea, stepperPlatformDock);
    steerPlatformDock->hide();
    stepperPlatformDock->show();
}

