#include "loaddevicewindow.h"
#include "ui_loaddevicewindow.h"

LoaddeviceWindow::LoaddeviceWindow(const QList<Device *> &devices, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loaddevicewindow)
{
    setModal(true);
    ui->setupUi(this);

    for (Device *device : devices) {
        ui->devicelistComboBox->addItem(device->device);
    }
}

int LoaddeviceWindow::getDeviceIndex()
{
    return ui->devicelistComboBox->currentIndex();
}

LoaddeviceWindow::~LoaddeviceWindow()
{
    delete ui;
}
