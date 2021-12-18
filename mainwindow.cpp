#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tray->setIcon(QIcon(":/icons/headphones-inv.png"));
    tray->show();
    tray->setToolTip("HeadsetControl");

    QMenu *menu = new QMenu(nullptr);
    menu->addAction("Show", this, SLOT(show()));
    menu->addAction("Turn Lights On", this, SLOT(on_onButton_clicked()));
    menu->addAction("Turn Lights Off", this, SLOT(on_offButton_clicked()));
    menu->addAction("Exit", this, SLOT(close()));

    tray->setContextMenu(menu);

    connect(tray, SIGNAL(DoubleClick), this, SLOT(show()));

    tray->connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                        SLOT(RestoreWindowTrigger(QSystemTrayIcon::ActivationReason)));

    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()  << QString("-c?"));

    proc->waitForFinished();
    QByteArray strdata = proc->readAllStandardOutput();
    QString supportedParams = strdata;
    //supportedParams = "sbnlimvr"; //Uncomment this to enable all "modules"

    if (supportedParams == "") {
        ui->notSupportedFrame->setHidden(false);
        ui->sidetoneFrame->setHidden(true);
        ui->batteryFrame->setHidden(true);
        ui->lightFrame->setHidden(true);
        ui->inactivityFrame->setHidden(true);
        ui->voicepromptFrame->setHidden(true);
        ui->rotateFrame->setHidden(true);
    }
    else {
        ui->notSupportedFrame->setHidden(true);

        if (supportedParams.contains("s")){
            ui->sidetoneFrame->setHidden(false);
            qDebug() << "Sidetone supported";
        }
        else ui->sidetoneFrame->setHidden(true);

        if (supportedParams.contains("b")){
            ui->batteryFrame->setHidden(false);

            QTimer *timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(setBatteryStatus()));
            timer->start(300000);
            this->setBatteryStatus();
            qDebug() << "Battery percentage supported";
        }
        else ui->batteryFrame->setHidden(true);

        if (supportedParams.contains("l")){
            ui->lightFrame->setHidden(false);
            qDebug() << "Light control supported";
        }
        else ui->lightFrame->setHidden(true);
        if (supportedParams.contains("i")){
            ui->inactivityFrame->setHidden(false);
            qDebug() << "Inactivity timer supported";
        }
        else ui->inactivityFrame->setHidden(true);
        if (supportedParams.contains("v")){
            ui->voicepromptFrame->setHidden(false);
            qDebug() << "Voice prompt control supported";
        }
        else ui->voicepromptFrame->setHidden(true);
        if (supportedParams.contains("r")){
            ui->rotateFrame->setHidden(false);
            qDebug() << "Rotate to mute supported";
        }
        else ui->rotateFrame->setHidden(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_onButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 1")
                << QString("-cl 1")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_offButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 0")
                << QString("-cl 0")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_voiceOnButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 1")
                << QString("-cv 1")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_voiceOffButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 0")
                << QString("-cv 0")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_sideToneApply_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 1")
                << QString("-s" + QString::number(ui->sidetoneSlider->sliderPosition()))
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_sideToneOff_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 0")
                << QString("-s 0")
                );
    proc->waitForFinished();
    ui->sidetoneSlider->setValue(0);
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_inactivityOffButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 0")
                << QString("-i 0")
                );
    proc->waitForFinished();
    ui->sidetoneSlider->setValue(0);
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_inactivityApplyButton_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 1")
                << QString("-i" + QString::number(ui->inactivitySlider->sliderPosition()))
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_rotateOn_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 1")
                << QString("-r 1")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::on_rotateOff_clicked()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-n 0")
                << QString("-r 0")
                );
    proc->waitForFinished();
    //qDebug() << proc->readAllStandardError();
}

void MainWindow::setBatteryStatus()
{
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", QStringList()
                << QString("-cb")
                );
    proc->waitForFinished();
    QString batteryStatus = proc->readAllStandardOutput();
    //qDebug() << proc->readAllStandardError();

    if (batteryStatus == "-2"){
        ui->batteryPercentage->setText("Headset Off");
        tray->setToolTip("HeadsetControl \r\nHeadset Off");
    }
    else if (batteryStatus == "-1") {
        ui->batteryPercentage->setText("Headset Charging");
        tray->setToolTip("HeadsetControl \r\nBattery Charging");
        tray->setIcon(QIcon(":/icons/battery-charging-inv.png"));
    }
    else {
        ui->batteryPercentage->setText(batteryStatus);
        tray->setToolTip("HeadsetControl \r\nBattery: " + batteryStatus + "%");
        if (batteryStatus.toInt() >= 70){
            tray->setIcon(QIcon(":/icons/battery-level-full-inv.png"));
            notified = false;
        }
        else if (batteryStatus.toInt() >= 30) {
            tray->setIcon(QIcon(":/icons/battery-medium-inv.png"));
            notified = false;
        }
        else {
            tray->setIcon(QIcon(":/icons/battery-low-inv.png"));
            if (!notified){
                tray->showMessage("Battery Alert!", "The battery of your headset is running low", QIcon(":/icons/battery-low-inv.png"));
                notified = true;
            }
        }
    }
}

void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type())
    {
        case QEvent::LanguageChange:
            this->ui->retranslateUi(this);
            break;
        case QEvent::WindowStateChange:
            {
                if (this->windowState() & Qt::WindowMinimized)
                {
                    QTimer::singleShot(0, this, SLOT(hide()));
                }

                break;
            }
        default:
            break;
    }

    QMainWindow::changeEvent(e);
}

void MainWindow::RestoreWindowTrigger(QSystemTrayIcon::ActivationReason RW)
{
    if(RW == QSystemTrayIcon::DoubleClick)
    {
        show();
        activateWindow();
        raise();
    }
}
