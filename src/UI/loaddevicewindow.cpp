#include "loaddevicewindow.h"
#include "ui_loaddevicewindow.h"

loaddeviceWindow::loaddeviceWindow(const QStringList& devices, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loaddevicewindow)
{
    setModal(true);
    ui->setupUi(this);

    ui->devicelistComboBox->addItems(devices);
}

int loaddeviceWindow::getDeviceIndex(){
    ui->devicelistComboBox->currentIndex();
}

loaddeviceWindow::~loaddeviceWindow()
{
    delete ui;
}
