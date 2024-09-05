#include "loaddevicewindow.h"
#include "ui_loaddevicewindow.h"

LoaddeviceWindow::LoaddeviceWindow(const QStringList &devices, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loaddevicewindow)
{
    setModal(true);
    ui->setupUi(this);

    ui->devicelistComboBox->addItems(devices);
}

int LoaddeviceWindow::getDeviceIndex()
{
    return ui->devicelistComboBox->currentIndex();
}

LoaddeviceWindow::~LoaddeviceWindow()
{
    delete ui;
}
