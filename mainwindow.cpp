#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QVersionNumber>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uix=ui;

    tray->setIcon(QIcon(":/icons/headphones-inv.png"));
    tray->show();
    tray->setToolTip("HeadsetControl");

    menu = new QMenu(nullptr);
    menu->addAction("Show", this, SLOT(show()));
    menu->addAction("Exit", this, SLOT(close()));

    tray->setContextMenu(menu);

    connect(tray, SIGNAL(DoubleClick), this, SLOT(show()));

    tray->connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                  SLOT(RestoreWindowTrigger(QSystemTrayIcon::ActivationReason)));

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);

    connect(ui->actionCheck_Updates, &QAction::triggered, this, &MainWindow::checkForUpdates);

    this->loadDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::checkForUpdates(){
    if(!jsonInfo.isEmpty()){
        QVersionNumber local_hc=QVersionNumber::fromString(jsonInfo["version"].toString());
        const QVersionNumber& local_gui=GUI_VERSION;
        QString v1 = getLatestGitHubReleaseVersion("Sapd","HeadsetControl");
        QString v2 = getLatestGitHubReleaseVersion("nicola02nb","HeadsetControl-GUI");
        QVersionNumber remote_hc =QVersionNumber::fromString(v1);
        QVersionNumber remote_gui =QVersionNumber::fromString(v2);
        QString s1 = "up-to date";
        QString s2 = "up-to date";
        if(!(v1=="") && remote_hc>local_hc){
            s1="Newer version ->"+remote_hc.toString();
        }

        if(!(v2=="") && remote_gui>local_gui){
            s2="Newer version ->"+remote_hc.toString();
        }
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *l1=new QLabel("HeadsetControl:\t\t"+s1);
        QLabel *l2=new QLabel("HeadsetControl-GUI:\t"+s2);
        layout->addWidget(l1);
        layout->addWidget(l2);
        showDialog("Check for updates",layout);
    }
}

QString MainWindow::sendCommand(QStringList args){
    QProcess *proc = new QProcess();
    proc->start("headsetcontrol", args);
    proc->waitForFinished();
    QString output=proc->readAllStandardOutput();
    //qDebug() << args;
    //qDebug() << output;
    return output;
}

void MainWindow::disableFrames(){
    ui->notSupportedFrame->setHidden(false);
    ui->tabWidget->hide();
    ui->deviceinfoFrame->setHidden(true);
    ui->sidetoneFrame->setHidden(true);
    ui->batteryFrame->setHidden(true);
    ui->lightFrame->setHidden(true);
    ui->inactivityFrame->setHidden(true);
    ui->voicepromptFrame->setHidden(true);
    ui->rotatetomuteFrame->setHidden(true);
    ui->chatmixFrame->setHidden(true);
    ui->equalizerFrame->setHidden(true);
    ui->muteledbrightnessFrame->setHidden(true);
    ui->micvolumeFrame->setHidden(true);
}

void MainWindow::loadDevices(){

    QStringList args=QStringList() << QString("--output") << QString("JSON");
    //args=QStringList() << QString("--test-device") << QString("0")  << QString("--output") << QString("JSON");    //Uncomment this to enable all "modules"

    QJsonDocument jsonDoc = QJsonDocument::fromJson(sendCommand(args).toUtf8());
    jsonInfo=jsonDoc.object();

    if(!jsonDoc.isNull()){
        deviceList=jsonInfo["devices"].toArray();
        this->loadFeatures();
    }else {
        this->disableFrames();
    }
}

void MainWindow::loadFeatures(int deviceIndex){
    if(deviceIndex<0) return;
    Ui::MainWindow *ui=uix;
    ui->tabWidget->show();

    usingDevice=deviceList[deviceIndex].toObject();
    QJsonArray caps=usingDevice["capabilities"].toArray();
    for (const QJsonValue &value : caps) {
        capabilities.insert(value.toString());
        qDebug()<<value.toString();
    }

    ui->notSupportedFrame->setHidden(true);

    QString device, vendor, product;
    device=usingDevice["device"].toString();
    vendor=usingDevice["vendor"].toString();
    product=usingDevice["product"].toString();
    ui->deviceinfovalueLabel->setText("Device: "+device+"\r\nVendor: "+vendor+"\r\nProduct: "+product);
    ui->deviceinfoFrame->setHidden(false);

    if (capabilities.contains("CAP_SIDETONE")){
        ui->sidetoneFrame->setHidden(false);
        qDebug() << "Sidetone supported";
    }
    else ui->sidetoneFrame->setHidden(true);
    if (capabilities.contains("CAP_BATTERY_STATUS")){
        ui->batteryFrame->setHidden(false);

        QTimer *timerBattery = new QTimer(this);
        connect(timerBattery, SIGNAL(timeout()), this, SLOT(setBatteryStatus()));
        timerBattery->start(300000);
        this->setBatteryStatus();
        qDebug() << "Battery percentage supported";
    }
    else ui->batteryFrame->setHidden(true);
    if (capabilities.contains("CAP_LIGHTS")){
        ui->lightFrame->setHidden(false);
        menu->addAction("Turn Lights On", this, SLOT(on_onButton_clicked()));
        menu->addAction("Turn Lights Off", this, SLOT(on_offButton_clicked()));
        qDebug() << "Light control supported";
    }
    else ui->lightFrame->setHidden(true);
    if (capabilities.contains("CAP_INACTIVE_TIME")){
        ui->inactivityFrame->setHidden(false);
        qDebug() << "Inactivity timer supported";
    }
    else ui->inactivityFrame->setHidden(true);
    if (capabilities.contains("CAP_VOICE_PROMPTS")){
        ui->voicepromptFrame->setHidden(false);
        qDebug() << "Voice prompt control supported";
    }
    else ui->voicepromptFrame->setHidden(true);
    if (capabilities.contains("CAP_ROTATE_TO_MUTE")){
        ui->rotatetomuteFrame->setHidden(false);
        qDebug() << "Rotate to mute supported";
    }
    else ui->rotatetomuteFrame->setHidden(true);
    if (capabilities.contains("CAP_CHATMIX_STATUS")){
        ui->chatmixFrame->setHidden(false);

        QTimer *timerChatmix = new QTimer(this);
        connect(timerChatmix, SIGNAL(timeout()), this, SLOT(setChatmixStatus()));
        timerChatmix->start(300000);
        this->setChatmixStatus();
        qDebug() << "Chatmix supported";
    }
    else ui->chatmixFrame->setHidden(true);
    if (capabilities.contains("CAP_EQUALIZER")){
        ui->equalizerFrame->setHidden(false);
        int n=10;
        int max=10;
        int min=-10;
        QHBoxLayout *mainLayout = ui->equalizerLayout;
        for (int var = 0; var < n; ++var) {
            QVBoxLayout *lb = new QVBoxLayout();
            QSlider *s = new QSlider(Qt::Vertical);
            s->setMaximum(max);
            s->setMinimum(min);
            s->setValue((max+min)/2);
            s->setTickInterval(max/2);
            s->setTickPosition(QSlider::TicksBothSides);
            QLabel *l = new QLabel(QString::number(var));
            l->setFixedSize(30, 20);
            l->setStyleSheet("QLabel {\nmin-width: 30px;\nmax-width: 30px;\n}");
            l->setAlignment(Qt::AlignHCenter);

            lb->addWidget(l);
            lb->addWidget(s);

            slidersEq.append(s);
            mainLayout->addLayout(lb);
        }
        qDebug() << "Equalizer supported";
    }
    else ui->equalizerFrame->setHidden(true);
    if (capabilities.contains("CAP_MICROPHONE_MUTE_LED_BRIGHTNESS")){
        ui->muteledbrightnessFrame->setHidden(false);
        qDebug() << "Muted led brightness supported";
    }
    else ui->muteledbrightnessFrame->setHidden(true);
    if (capabilities.contains("CAP_MICROPHONE_VOLUME")){
        ui->micvolumeFrame->setHidden(false);
        qDebug() << "Microphone volume supported";
    }
    else ui->micvolumeFrame->setHidden(true);
}

void MainWindow::on_onButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("1");
    sendCommand(args);
}

void MainWindow::on_offButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("0");
    sendCommand(args);
}

void MainWindow::on_voiceOnButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("1");
    sendCommand(args);
}

void MainWindow::on_voiceOffButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("0");
    sendCommand(args);
}

void MainWindow::on_rotateOn_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("1");
    sendCommand(args);
}

void MainWindow::on_rotateOff_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("0");
    sendCommand(args);
}

void MainWindow::setBatteryStatus()
{
    QStringList args=QStringList() << QString("--battery");
    QString batteryStatus = sendCommand(args);

    QStringList lines = batteryStatus.split("\n");

    // Extract the status value
    QString statusLine = lines[3];
    QStringList statusParts = statusLine.split(": ");
    QString status = statusParts[1].trimmed();

    QString level="none";
    if(status != "BATTERY_UNAVAILABLE"){
        // Extract the level value
        QString levelLine = lines[4];
        QStringList levelParts = levelLine.split(": ");
        level = levelParts[1].trimmed();
        level.remove("%");
    }

    if (status == "BATTERY_UNAVAILABLE"){
        ui->batteryPercentage->setText("Headset Off");
        tray->setToolTip("HeadsetControl \r\nHeadset Off");
        tray->setIcon(QIcon(":/icons/headphones-inv.png"));
    }
    else if (status == "BATTERY_CHARGING") {
        ui->batteryPercentage->setText("Headset Charging "+level+"%");
        tray->setToolTip("HeadsetControl \r\nBattery Charging");
        tray->setIcon(QIcon(":/icons/battery-charging-inv.png"));
    }
    else {
        ui->batteryPercentage->setText(level + "%");
        tray->setToolTip("HeadsetControl \r\nBattery: " + level + "%");
        if (level.toInt() >= 70){
            tray->setIcon(QIcon(":/icons/battery-level-full-inv.png"));
            notified = false;
        }
        else if (level.toInt() >= 30) {
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

void MainWindow::on_sidetoneSlider_valueChanged(){
    QStringList args=QStringList() << QString("--sidetone") << QString::number(ui->sidetoneSlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::on_inactivitySlider_valueChanged(){
    QStringList args=QStringList() << QString("--inactive-time") << QString::number(ui->inactivitySlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::setChatmixStatus(){
    QStringList args=QStringList() << QString("--chatmix");
    QString chatmixStatus = sendCommand(args);
    int value=chatmixStatus.mid(chatmixStatus.indexOf(':')+1).toInt();
    ui->chatmixvalueLabel->setText(QString::number(value));
}

void MainWindow::on_equalizerPresetcomboBox_currentIndexChanged(){
    int preset=ui->equalizerPresetcomboBox->currentIndex()-1;
    if(preset>=0 && preset<=3){
        this->setSliders(flat);
        QStringList args=QStringList() << QString("--equalizer-preset") << QString::number(preset);
        sendCommand(args);
    }
}

void MainWindow::on_applyEqualizer_clicked(){
    uix->equalizerPresetcomboBox->setCurrentIndex(0);
    QString s="";
    for (QSlider* slider : slidersEq) {
        s+= QString::number(slider->value())+",";
    }
    s.removeLast();
    QStringList args=QStringList() << QString("--equalizer") << s;
    sendCommand(args);
}

void MainWindow::on_muteledbrightnessSlider_valueChanged(){
    QStringList args=QStringList() << QString("--microphone-mute-led-brightness") << QString::number(ui->muteledbrightnessSlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::on_micvolumeSlider_valueChanged(){
    QStringList args=QStringList() << QString("--microphone-volume") << QString::number(ui->micvolumeSlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::setSliders(QVector<int> values){
    int i=0;
    if(values.length()<=slidersEq.length()){
        for (QSlider* slider : slidersEq) {
            slider->setValue(values[i++]);
        }
    }
    else{
        qDebug() << "ERROR: Longer Equalizer Preset";
    }
}

void MainWindow::showDialog(QString title, QLayout* layout){
    QDialog dialog;
    dialog.setWindowTitle(title);
    dialog.setWindowIcon(QIcon(":/icons/headphones.png"));
    dialog.setLayout(layout);
    QPushButton *closeButton = new QPushButton("Close");
    QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);
    dialog.exec();
}

void MainWindow::showAbout(){
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *l1 = new QLabel("<a href='https://github.com/nicola02nb/HeadsetControl-GUI'>This</a> is a forked version of <a href='https://github.com/LeoKlaus/HeadsetControl-GUI'>HeadsetControl-GUI</a>.");
    l1->setTextFormat(Qt::RichText);
    l1->setOpenExternalLinks(true);
    l1->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QLabel *l2=new QLabel("Made by <a href='https://github.com/nicola02nb/HeadsetControl-GUI'>nicola02nb</a>");
    l2->setTextFormat(Qt::RichText);
    l2->setOpenExternalLinks(true);
    l2->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QLabel *version=new QLabel("Version: "+GUI_VERSION.toString());

    layout->addWidget(l1);
    layout->addWidget(l2);
    layout->addWidget(version);    

    showDialog("About this program",layout);
}

void MainWindow::showCredits(){
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *infoLabel = new QLabel("Big shout-out to:");
    QLabel *l1=new QLabel(" - Sapd for <a href='https://github.com/Sapd/HeadsetControl'>HeadsetCoontrol</a>");
    l1->setTextFormat(Qt::RichText);
    l1->setOpenExternalLinks(true);
    l1->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QLabel *l2=new QLabel(" - LeoKlaus for <a href='https://github.com/LeoKlaus/HeadsetControl-GUI'>HeadsetControl-GUI</a>");
    l2->setTextFormat(Qt::RichText);
    l2->setOpenExternalLinks(true);
    l2->setTextInteractionFlags(Qt::TextBrowserInteraction);

    layout->addWidget(infoLabel);
    layout->addWidget(l1);
    layout->addWidget(l2);

    showDialog("Credit to",layout);
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
