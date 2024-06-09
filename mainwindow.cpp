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
#include <QDialogButtonBox>

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
    connect(ui->actionLoad_Device, &QAction::triggered, this, &MainWindow::selectDevice);

    this->disableFrames();
    this->loadDevices();
    if(deviceQuantity>0){
        this->loadDevice();
    }
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
        QString s1 = "up-to date v"+local_hc.toString();
        QString s2 = "up-to date v"+local_gui.toString();
        if(!(v1=="") && remote_hc>local_hc){
            s1="Newer version -><a href=\"https://github.com/Sapd/HeadsetControl/releases/latest\">"+remote_hc.toString()+"</a>";
        }
        if(!(v2=="") && remote_gui>local_gui){
            s2="Newer version -><a href=\"https://github.com/nicola02nb/HeadsetControl-GUI/releases/latest\">"+remote_gui.toString()+"</a>";
        }
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *l1=new QLabel("HeadsetControl:\t\t"+s1);
        l1->setTextFormat(Qt::RichText);
        l1->setOpenExternalLinks(true);
        l1->setTextInteractionFlags(Qt::TextBrowserInteraction);
        QLabel *l2=new QLabel("HeadsetControl-GUI:\t"+s2);
        l2->setTextFormat(Qt::RichText);
        l2->setOpenExternalLinks(true);
        l2->setTextInteractionFlags(Qt::TextBrowserInteraction);
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
    ui->deviceinfoFrame->setHidden(true);
    ui->batteryFrame->setHidden(true);

    ui->tabWidget->hide();
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(0, false);

    ui->lightFrame->setHidden(true);
    ui->voicepromptFrame->setHidden(true);
    ui->sidetoneFrame->setHidden(true);
    ui->inactivityFrame->setHidden(true);
    ui->chatmixFrame->setHidden(true);

    ui->equalizerpresetFrame->setHidden(true);
    ui->equalizerFrame->setHidden(true);

    ui->rotatetomuteFrame->setHidden(true);
    ui->muteledbrightnessFrame->setHidden(true);
    ui->micvolumeFrame->setHidden(true);
}

void MainWindow::loadDevices(){

    QStringList args=QStringList() << QString("--output") << QString("JSON");
    //args=QStringList() << QString("--test-device") << QString("0")  << QString("--output") << QString("JSON");    //Uncomment this to enable all "modules"

    //QString test="{\"name\":\"HeadsetControl\",\"version\":\"3.0.0\",\"api_version\":\"1.0\",\"hidapi_version\":\"0.14.0\",\"device_count\":2,\"devices\":[{\"status\":\"success\",\"device\":\"Dev1\",\"vendor\":\"Dev1\",\"product\":\"Dev1\",\"id_vendor\":\"0x1038\",\"id_product\":\"0x2202\",\"capabilities\":[\"CAP_SIDETONE\",\"CAP_BATTERY_STATUS\",\"CAP_INACTIVE_TIME\"],\"capabilities_str\":[\"sidetone\",\"battery\",\"inactive time\",\"chatmix\",\"equalizer preset\",\"equalizer\"],\"battery\":{\"status\":\"BATTERY_AVAILABLE\",\"level\":75},\"chatmix\":65},{\"status\":\"success\",\"device\":\"Dev2\",\"vendor\":\"Dev2\",\"product\":\"Dev2\",\"id_vendor\":\"0x1038\",\"id_product\":\"0x2202\",\"capabilities\":[\"CAP_SIDETONE\",\"CAP_BATTERY_STATUS\",\"CAP_INACTIVE_TIME\",\"CAP_CHATMIX_STATUS\",\"CAP_EQUALIZER_PRESET\",\"CAP_EQUALIZER\"],\"capabilities_str\":[\"sidetone\",\"battery\",\"inactive time\",\"chatmix\",\"equalizer preset\",\"equalizer\"],\"battery\":{\"status\":\"BATTERY_AVAILABLE\",\"level\":75},\"chatmix\":65}]}";

    QJsonDocument jsonDoc = QJsonDocument::fromJson(sendCommand(args).toUtf8());
    //jsonDoc=QJsonDocument::fromJson(test.toUtf8()); //test fort multiple devices
    jsonInfo=jsonDoc.object();

    if(!jsonDoc.isNull()){
        deviceQuantity=jsonInfo["device_count"].toInt();
        deviceList=jsonInfo["devices"].toArray();
    }
}

void MainWindow::loadDevice(int deviceIndex){
    if(deviceIndex<0) return;

    Ui::MainWindow *ui=uix;

    usingDevice=deviceList[deviceIndex].toObject();
    QJsonArray caps=usingDevice["capabilities"].toArray();
    QSet<QString> capabilities;
    for (const QJsonValue &value : caps) {
        capabilities.insert(value.toString());
        //qDebug()<<value.toString();
    }

    if(timerBattery!=nullptr){
        timerBattery->stop();
        timerBattery=nullptr;
    }
    if(timerChatmix!=nullptr){
        timerChatmix->stop();
        timerChatmix=nullptr;
    }

    ui->notSupportedFrame->setHidden(true);

    QString device, vendor, product;
    device=usingDevice["device"].toString();
    vendor=usingDevice["vendor"].toString();
    product=usingDevice["product"].toString();
    ui->deviceinfovalueLabel->setText(device+"\n"+vendor+"\n"+product);
    ui->deviceinfoFrame->setHidden(false);
    if (capabilities.contains("CAP_BATTERY_STATUS")){
        ui->batteryFrame->setHidden(false);
        timerBattery = new QTimer(this);
        connect(timerBattery, SIGNAL(timeout()), this, SLOT(setBatteryStatus()));
        timerBattery->start(UPDATE_TIME);
        this->setBatteryStatus();
        qDebug() << "Battery percentage supported";
    }

    ui->tabWidget->show();

    if (capabilities.contains("CAP_LIGHTS")){
        ui->lightFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        menu->addAction("Turn Lights On", this, SLOT(on_onButton_clicked()));
        menu->addAction("Turn Lights Off", this, SLOT(on_offButton_clicked()));
        qDebug() << "Light control supported";
    }
    if (capabilities.contains("CAP_SIDETONE")){
        ui->sidetoneFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        qDebug() << "Sidetone supported";
    }
    if (capabilities.contains("CAP_VOICE_PROMPTS")){
        ui->voicepromptFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        qDebug() << "Voice prompt control supported";
    }
    if (capabilities.contains("CAP_INACTIVE_TIME")){
        ui->inactivityFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        qDebug() << "Inactivity timer supported";
    }
    if (capabilities.contains("CAP_CHATMIX_STATUS")){
        ui->chatmixFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        timerChatmix = new QTimer(this);
        connect(timerChatmix, SIGNAL(timeout()), this, SLOT(setChatmixStatus()));
        timerChatmix->start(UPDATE_TIME);
        this->setChatmixStatus();
        qDebug() << "Chatmix supported";
    }

    if (capabilities.contains("CAP_EQUALIZER_PRESET")){
        ui->equalizerpresetFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
        qDebug() << "Eqaulizer preset supported";
    }
    if (capabilities.contains("CAP_EQUALIZER")){
        ui->equalizerFrame->setHidden(false);        
        ui->tabWidget->setTabEnabled(1, true);
        int n=10;
        int max=10;
        int min=-10;
        QHBoxLayout *equalizerLayout = ui->equalizerLayout;
        clearLayout(equalizerLayout);
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
            equalizerLayout->addLayout(lb);
        }
        qDebug() << "Equalizer supported";
    }

    if (capabilities.contains("CAP_ROTATE_TO_MUTE")){
        ui->rotatetomuteFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
        qDebug() << "Rotate to mute supported";
    }
    if (capabilities.contains("CAP_MICROPHONE_MUTE_LED_BRIGHTNESS")){
        ui->muteledbrightnessFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
        qDebug() << "Muted led brightness supported";
    }
    if (capabilities.contains("CAP_MICROPHONE_VOLUME")){
        ui->micvolumeFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(2, true);
        qDebug() << "Microphone volume supported";
    }
}

void MainWindow::on_onlightButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("1");
    sendCommand(args);
}

void MainWindow::on_offlightButton_clicked()
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

void MainWindow::on_sidetoneSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--sidetone") << QString::number(ui->sidetoneSlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::on_inactivitySlider_sliderReleased(){
    QStringList args=QStringList() << QString("--inactive-time") << QString::number(ui->inactivitySlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::setChatmixStatus(){
    Ui::MainWindow *ui=uix;
    QStringList args=QStringList() << QString("--chatmix");
    QString chatmixValue= sendCommand(args);
    int value=chatmixValue.mid(chatmixValue.indexOf(':')+1).toInt();
    ui->chatmixvalueLabel->setText(QString::number(value));
    QString chatmixStatus="";
    if(value<65)chatmixStatus="Game";
    else if(value>65)chatmixStatus="Chat";
    ui->chatmixstatusLabel->setText(chatmixStatus);
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

void MainWindow::on_muteledbrightnessSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--microphone-mute-led-brightness") << QString::number(ui->muteledbrightnessSlider->sliderPosition());
    sendCommand(args);
}

void MainWindow::on_micvolumeSlider_sliderReleased(){
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

void MainWindow::selectDevice(){
    QDialog dialog;
    dialog.setWindowTitle("Select device to load");

    QVBoxLayout layout(&dialog);

    QLabel labelWidget("Select device:");
    layout.addWidget(&labelWidget);

    QStringList devices=QStringList();
    for (int i = 0; i < deviceQuantity; ++i){
        devices<<deviceList[i].toObject()["device"].toString();
    }

    QComboBox comboBox;
    comboBox.addItems(devices);
    layout.addWidget(&comboBox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        selectedDevice = comboBox.currentIndex();
        if (selectedDevice>=0 && selectedDevice<deviceQuantity) {
            this->disableFrames();
            this->loadDevice(selectedDevice);
        }
    }
}

void MainWindow::clearLayout(QLayout* layout){
    if (!layout) {
        return;
    }

    QLayoutItem* item;
    while ((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());         // Delete the layout if it exists
        }
        if (item->widget()) {
            delete item->widget(); // Delete the widget
        }
        delete item; // Delete the layout item
    }
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
