#ifndef LOADDEVICEWINDOW_H
#define LOADDEVICEWINDOW_H

#include <QDialog>

namespace Ui {
class loaddevicewindow;
}

class loaddeviceWindow : public QDialog
{
    Q_OBJECT

public:
    explicit loaddeviceWindow(const QStringList& devices, QWidget *parent = nullptr);
    ~loaddeviceWindow();

    int getDeviceIndex();  

private:
    Ui::loaddevicewindow *ui;
};

#endif // LOADDEVICEWINDOW_H
