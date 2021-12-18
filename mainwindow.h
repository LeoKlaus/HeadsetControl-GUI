#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool notified = false;
    QSystemTrayIcon *tray = new QSystemTrayIcon(this);

private slots:
    void changeEvent(QEvent *e);

    void RestoreWindowTrigger(QSystemTrayIcon::ActivationReason RW);

    void on_onButton_clicked();

    void on_offButton_clicked();

    void on_voiceOnButton_clicked();

    void on_voiceOffButton_clicked();

    void on_sideToneApply_clicked();

    void on_sideToneOff_clicked();

    void on_inactivityOffButton_clicked();

    void on_inactivityApplyButton_clicked();

    void on_rotateOn_clicked();

    void on_rotateOff_clicked();

    void setBatteryStatus();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
