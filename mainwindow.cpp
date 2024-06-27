#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Device.h"
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
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tray->setIcon(QIcon(":/icons/headphones-inv.png"));
    tray->show();
    tray->setToolTip("HeadsetControl");

    menu = new QMenu(nullptr);
    menu->addAction("Show", this, SLOT(show()));
    ledOn  = menu->addAction("Turn Lights On", this, SLOT(on_onButton_clicked()));
    ledOff = menu->addAction("Turn Lights Off", this, SLOT(on_offButton_clicked()));
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
    if(deviceList.length()){
        this->loadDevice();
    }

    ui->actionLoad_Device->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disableFrames(){
    ledOn->setEnabled(false);
    ledOff->setEnabled(false);

    ui->notSupportedFrame->setHidden(false);
    ui->deviceinfoFrame->setHidden(true);
    ui->batteryFrame->setHidden(true);
    ui->savesettingsButton->setHidden(true);

    //ui->tabWidget->hide();
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(0, false);

    ui->lightFrame->setHidden(true);
    ui->voicepromptFrame->setHidden(true);
    ui->notificationFrame->setHidden(true);
    ui->sidetoneFrame->setHidden(true);
    ui->inactivityFrame->setHidden(true);
    ui->chatmixFrame->setHidden(true);
    ui->volumelimiterFrame->setHidden(true);

    ui->equalizerpresetFrame->setHidden(true);
    ui->equalizerFrame->setHidden(true);
    ui->applyEqualizer->setEnabled(false);

    ui->rotatetomuteFrame->setHidden(true);
    ui->muteledbrightnessFrame->setHidden(true);
    ui->micvolumeFrame->setHidden(true);

    ui->btwhenonFrame->setHidden(true);
    ui->btcallvolumeFrame->setHidden(true);
}

void MainWindow::loadDevices(){
    deviceList=mergeDevices(getSavedDevices(), getConnectedDevices());
}

void MainWindow::updateDevice(){
    //serializeDevices(deviceList, "devices.json");
    QList<Device*> newDl=getConnectedDevices();
    selectedDevice->updateDevice(newDl);
}

void MainWindow::updateGUI(){
    setBatteryStatus();
    setChatmixStatus();
}

void MainWindow::loadDevice(int deviceIndex){
    if(deviceIndex<0) return;

    selectedDevice=deviceList.value(deviceIndex);
    QSet<QString>& capabilities=selectedDevice->capabilities;

    if(timerGUI!=nullptr){
        timerGUI->stop();
        timerGUI=nullptr;
    }
    timerGUI = new QTimer(this);
    connect(timerGUI, SIGNAL(timeout()), this, SLOT(updateDevice()));
    connect(timerGUI, SIGNAL(timeout()), this, SLOT(updateGUI()));
    timerGUI->start(UPDATE_TIME);

    ui->notSupportedFrame->setHidden(true);

    //Inffo section
    ui->deviceinfovalueLabel->setText(selectedDevice->device+"\n"+selectedDevice->vendor+"\n"+selectedDevice->product);
    ui->deviceinfoFrame->setHidden(false);
    ui->savesettingsButton->setHidden(false);
    if (capabilities.contains("CAP_BATTERY_STATUS")){
        ui->batteryFrame->setHidden(false);
        this->setBatteryStatus();
        qDebug() << "Battery percentage supported";
    }

    //ui->tabWidget->show();
    //Other Section
    if (capabilities.contains("CAP_LIGHTS")){
        ui->lightFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        ledOn->setEnabled(true);
        ledOff->setEnabled(true);
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
        qDebug() << "Voice prompt supported";
    }
    if (capabilities.contains("CAP_NOTIFICATION_SOUND")){
        ui->notificationFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        qDebug() << "Notification sound supported";
    }
    if (capabilities.contains("CAP_INACTIVE_TIME")){
        ui->inactivityFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        qDebug() << "Inactivity timer supported";
    }
    if (capabilities.contains("CAP_CHATMIX_STATUS")){
        ui->chatmixFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(0, true);
        this->setChatmixStatus();
        qDebug() << "Chatmix supported";
    }
    //Eualizer Section
    if (capabilities.contains("CAP_EQUALIZER_PRESET")){
        ui->equalizerpresetFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
        qDebug() << "Eqaulizer preset supported";
    }
    if (capabilities.contains("CAP_EQUALIZER")){
        ui->equalizerFrame->setHidden(false);        
        ui->tabWidget->setTabEnabled(1, true);
        qDebug() << "Equalizer supported";
    }
    if (capabilities.contains("CAP_VOLUME_LIMITER")){
        ui->volumelimiterFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
        qDebug() << "Volume limiter preset supported";
    }
    //Microphone Section
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
    //Bluetooth Section
    if (capabilities.contains("CAP_BT_WHEN_POWERED_ON")){
        ui->btwhenonFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(3, true);
        qDebug() << "Bluetooth when powered on volume supported";
    }
    if (capabilities.contains("CAP_BT_CALL_VOLUME")){
        ui->btcallvolumeFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(3, true);
        qDebug() << "Bluetooth call volume volume supported";
    }

    loadGUIValues();
}

void MainWindow::loadGUIValues(){
    if(selectedDevice->lights>=0){
        ui->onlightButton->setChecked(selectedDevice->lights);
        ui->offlightButton->setChecked(!selectedDevice->lights);
    }
    if(selectedDevice->sidetone>=0){
        ui->sidetoneSlider->setSliderPosition(selectedDevice->sidetone);
    }
    if(selectedDevice->voice_prompts>=0){
        ui->voiceOnButton->setChecked(selectedDevice->voice_prompts);
        ui->voiceOffButton->setChecked(!selectedDevice->voice_prompts);
    }
    if(selectedDevice->inactive_time>=0){
        ui->inactivitySlider->setSliderPosition(selectedDevice->inactive_time);
    }

    ui->equalizerPresetcomboBox->clear();
    ui->equalizerPresetcomboBox->addItem("-");
    ui->equalizerPresetcomboBox->setCurrentIndex(0);
    for (int i = 0; i < selectedDevice->presets_list.size(); ++i) {
        ui->equalizerPresetcomboBox->addItem(selectedDevice->presets_list.at(i).name);
    }
    if(selectedDevice->equalizer_preset>=0){
        ui->equalizerPresetcomboBox->setCurrentIndex(selectedDevice->equalizer_preset);
    }

    QHBoxLayout *equalizerLayout = ui->equalizerLayout;
    clearLayout(equalizerLayout);
    int i;
    for (i = 0; i < selectedDevice->equalizer.bands_number; ++i) {
        QVBoxLayout *lb = new QVBoxLayout();
        QSlider *s = new QSlider(Qt::Vertical);
        s->setMaximum(selectedDevice->equalizer.band_max);
        s->setMinimum(selectedDevice->equalizer.band_min);
        s->setSingleStep(selectedDevice->equalizer.band_step);
        s->setTickInterval(selectedDevice->equalizer.band_step);
        s->setTickPosition(QSlider::TicksBothSides);
        if(selectedDevice->equalizer_curve.size()==selectedDevice->equalizer.bands_number){
            s->setValue(selectedDevice->equalizer_curve.value(i));
        } else{
            s->setValue(selectedDevice->equalizer.band_baseline);
        }

        QLabel *l = new QLabel(QString::number(i));
        l->setAlignment(Qt::AlignHCenter);

        lb->addWidget(l);
        lb->addWidget(s);

        slidersEq.append(s);
        equalizerLayout->addLayout(lb);
    }
    if(i!=0){
        ui->applyEqualizer->setEnabled(true);
    }

    if(selectedDevice->volume_limiter>=0){
        ui->volumelimiterOnButton->setChecked(selectedDevice->volume_limiter);
        ui->volumelimiterOffButton->setChecked(!selectedDevice->volume_limiter);
    }

    if(selectedDevice->rotate_to_mute>=0){
        ui->rotateOn->setChecked(selectedDevice->rotate_to_mute);
        ui->rotateOff->setChecked(!selectedDevice->rotate_to_mute);
    }
    if(selectedDevice->mic_mute_led_brightness>=0){
        ui->muteledbrightnessSlider->setSliderPosition(selectedDevice->mic_mute_led_brightness);
    }
    if(selectedDevice->mic_volume>=0){
        ui->micvolumeSlider->setSliderPosition(selectedDevice->mic_volume);
    }

    if(selectedDevice->bt_call_volume>=0){
        switch (selectedDevice->bt_call_volume) {
        case 0:
            ui->btbothRadioButton->setChecked(true);
            break;
        case 1:
            ui->btpcdbRadioButton->setChecked(true);
            break;
        case 2:
            ui->btonlyRadioButton->setChecked(true);
            break;
        default:
            break;
        }
    }
    if(selectedDevice->bt_when_powered_on>=0){
        ui->btwhenonOnButton->setChecked(selectedDevice->bt_when_powered_on);
        ui->btwhenonOffButton->setChecked(!selectedDevice->bt_when_powered_on);
    }
}

//Info Section Events
void MainWindow::setBatteryStatus()
{
    QString status=selectedDevice->battery.status;
    QString level=QString::number(selectedDevice->battery.level);

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
    else if(status == "BATTERY_AVAILABLE"){
        ui->batteryPercentage->setText(level + "%");
        tray->setToolTip("HeadsetControl \r\nBattery: " + level + "%");
        if (level.toInt() >= 75){
            tray->setIcon(QIcon(":/icons/battery-level-full-inv.png"));
        }
        else if (level.toInt() >= 25) {
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
    } else{
        ui->batteryPercentage->setText(status);
        tray->setToolTip("HeadsetControl");
        tray->setIcon(QIcon(":/icons/headphones-inv.png"));
    }
}

void MainWindow::on_savesettingsButton_clicked(){
    serializeDevices(deviceList, "devices.json");
}

//Other Section Events
void MainWindow::on_onlightButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->lights=1;
}

void MainWindow::on_offlightButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->lights=0;
}

void MainWindow::on_sidetoneSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--sidetone") << QString::number(ui->sidetoneSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->sidetone=ui->sidetoneSlider->value();
}

void MainWindow::on_voiceOnButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->voice_prompts=1;
}

void MainWindow::on_voiceOffButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->voice_prompts=0;
}

void MainWindow::on_notification0ButtonButton_clicked()
{
    QStringList args=QStringList() << QString("--notificate") << QString("0");
    Action s=sendAction(args);
    if(s.status!="success"){

    }
}

void MainWindow::on_notification1ButtonButton_clicked()
{
    QStringList args=QStringList() << QString("--notificate") << QString("1");
    Action s=sendAction(args);
    if(s.status!="success"){

    }
}

void MainWindow::on_rotateOn_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->rotate_to_mute=1;
}

void MainWindow::on_rotateOff_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->rotate_to_mute=0;
}

void MainWindow::on_inactivitySlider_sliderReleased(){
    QStringList args=QStringList() << QString("--inactive-time") << QString::number(ui->inactivitySlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->inactive_time=ui->inactivitySlider->value();
}

void MainWindow::setChatmixStatus(){
    int chatmix = selectedDevice->chatmix;
    QString chatmixValue = QString::number(chatmix);
    QString chatmixStatus;
    if(chatmix<65)chatmixStatus="Game";
    else if(chatmix>65)chatmixStatus="Chat";
    ui->chatmixstatusLabel->setText(chatmixValue+" "+chatmixStatus);
}

//Equalizer Section Events
void MainWindow::on_equalizerPresetcomboBox_currentIndexChanged(){
    int preset=ui->equalizerPresetcomboBox->currentIndex();
    if(preset==0){
        //setSliders(selectedDevice->equalizer.band_baseline);
    } else if(preset>=1 && preset<=selectedDevice->presets_list.length()){
        this->setSliders(selectedDevice->presets_list.value(preset-1).values);
        QStringList args=QStringList() << QString("--equalizer-preset") << QString::number(preset-1);
        Action s=sendAction(args);
        if(s.status=="success")
            selectedDevice->equalizer_preset=ui->equalizerPresetcomboBox->currentIndex();
    }
}

void MainWindow::on_applyEqualizer_clicked(){
    ui->equalizerPresetcomboBox->setCurrentIndex(0);
    QString eq_string="";
    QList<int> values;
    for (QSlider* slider : slidersEq) {
        eq_string+= QString::number(slider->value())+",";
        values.append(slider->value());
    }
    eq_string.removeLast();
    QStringList args=QStringList() << QString("--equalizer") << eq_string;
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->equalizer_curve=values;
        selectedDevice->equalizer_preset=-1;
    }
}

void MainWindow::setSliders(int value){
    for (QSlider* slider : slidersEq) {
        slider->setValue(value);
    }
}

void MainWindow::setSliders(QList<double> values){
    int i=0;
    if(values.length()<=selectedDevice->equalizer.bands_number){
        for (QSlider* slider : slidersEq) {
            slider->setValue(values[i++]);
        }
    }
    else{
        qDebug() << "ERROR: Longer Equalizer Preset";
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

void MainWindow::on_volumelimiterOffButton_clicked(){
    QStringList args=QStringList() << QString("--volume-limiter") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->volume_limiter=0;
}

void MainWindow::on_volumelimiterOnButton_clicked(){
    QStringList args=QStringList() << QString("--volume-limiter") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->volume_limiter=1;
}

//Microphone Section Events
void MainWindow::on_muteledbrightnessSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--microphone-mute-led-brightness") << QString::number(ui->muteledbrightnessSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->mic_mute_led_brightness=ui->muteledbrightnessSlider->value();
}

void MainWindow::on_micvolumeSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--microphone-volume") << QString::number(ui->micvolumeSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->mic_volume=ui->micvolumeSlider->value();
}

//Bluetooth Section Events
void MainWindow::on_btwhenonOffButton_clicked(){
    QStringList args=QStringList() << QString("--bt-when-powered-on") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->bt_when_powered_on=0;
}

void MainWindow::on_btwhenonOnButton_clicked(){
    QStringList args=QStringList() << QString("--bt-when-powered-on") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->bt_when_powered_on=1;
}

void MainWindow::on_btbothRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->bt_call_volume=0;
}

void MainWindow::on_btpcdbRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->bt_call_volume=1;
}

void MainWindow::on_btonlyRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("2");
    Action s=sendAction(args);
    if(s.status=="success")
        selectedDevice->bt_call_volume=2;
}

//Tool Bar Events
void MainWindow::selectDevice(){
    QDialog dialog;
    dialog.setWindowTitle("Select device to load");

    QVBoxLayout layout(&dialog);

    QLabel labelWidget("Select device:");
    layout.addWidget(&labelWidget);

    QStringList devices=QStringList();
    for (int i = 0; i < deviceList.length(); ++i){
        devices<<deviceList.value(i)->device;
    }

    QComboBox comboBox;
    comboBox.addItems(devices);
    layout.addWidget(&comboBox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        int index = comboBox.currentIndex();
        if (index>=0) {
            this->disableFrames();
            this->loadDevice(index);
        }
    }
}

void MainWindow::checkForUpdates(){
        const QVersionNumber& local_hc=getHCVersion();
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
