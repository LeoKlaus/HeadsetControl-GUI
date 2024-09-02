#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "device.h"
#include "dialoginfo.h"
#include "settingswindow.h"
#include "loaddevicewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->bindEvents();

    settings=loadSettingsFromFile(PROGRAM_SETTINGS_FILENAME);
    darkMode = isOsDarkMode();

    if(darkMode){
        this->setWindowIcon(QIcon(":/icons/headphones-inv.png"));
        trayIconPath = ":/icons/headphones-inv.png";
    }
    else{
        this->setWindowIcon(QIcon(":/icons/headphones.png"));
        trayIconPath = ":/icons/headphones.png";
    }

    tray->setIcon(QIcon(trayIconPath));
    tray->show();
    tray->setToolTip("HeadsetControl");

    menu = new QMenu(nullptr);
    menu->addAction(tr("Show"), this, SLOT(show()));
    ledOn  = menu->addAction(tr("Turn Lights On"), this, &MainWindow::onlightButton_clicked);
    ledOff = menu->addAction(tr("Turn Lights Off"), this, &MainWindow::offlightButton_clicked);
    menu->addAction(tr("Exit"), this, &QApplication::quit);

    tray->setContextMenu(menu);
    tray->connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    this->disableFrames();

    QString exe = "headsetcontrol";
#ifdef Q_OS_WIN
    exe = exe+".exe";
#endif

    if(!fileExists(exe)){
        openFileExplorer(".");
        DialogInfo* dialog=new DialogInfo(this);
        dialog->setTitle(tr("Missing headsetcontrol"));
        dialog->setLabel(tr("Missing headsetcontrol<br>"
                            "Download <a href='https://github.com/Sapd/HeadsetControl/releases/latest'>headsetcontrol</a> in the opened folder."));
        dialog->exec();
    }

    this->loadDevices();
    if(deviceList.length() && n_connected>0){
        this->loadDevice();
    }

    this->setMaximumHeight(this->minimumHeight());
    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bindEvents(){
    // Tool Bar
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::editProgramSetting);
    connect(ui->actionLoad_Device, &QAction::triggered, this, &MainWindow::selectDevice);
    connect(ui->actionCheck_Updates, &QAction::triggered, this, &MainWindow::checkForUpdates);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);

    // Other Section
    connect(ui->onlightButton, &QPushButton::clicked, this, &MainWindow::onlightButton_clicked);
    connect(ui->offlightButton, &QPushButton::clicked, this, &MainWindow::offlightButton_clicked);
    connect(ui->sidetoneSlider, &QSlider::sliderReleased, this, &MainWindow::sidetoneSlider_sliderReleased);
    connect(ui->voiceOnButton, &QPushButton::clicked, this, &MainWindow::voiceOnButton_clicked);
    connect(ui->voiceOffButton, &QPushButton::clicked, this, &MainWindow::voiceOffButton_clicked);
    connect(ui->notification0Button, &QPushButton::clicked, this, &MainWindow::notification0Button_clicked);
    connect(ui->notification1Button, &QPushButton::clicked, this, &MainWindow::notification1Button_clicked);
    connect(ui->inactivitySlider, &QSlider::sliderReleased, this, &MainWindow::inactivitySlider_sliderReleased);

    // Equalizer Section
    connect(ui->equalizerPresetcomboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::equalizerPresetcomboBox_currentIndexChanged);
    connect(ui->applyEqualizer, &QPushButton::clicked, this, &MainWindow::applyEqualizer_clicked);
    connect(ui->volumelimiterOffButton, &QPushButton::clicked, this, &MainWindow::volumelimiterOffButton_clicked);
    connect(ui->volumelimiterOnButton, &QPushButton::clicked, this, &MainWindow::volumelimiterOnButton_clicked);

    // Microphone Section
    connect(ui->muteledbrightnessSlider, &QSlider::sliderReleased, this, &MainWindow::muteledbrightnessSlider_sliderReleased);
    connect(ui->micvolumeSlider, &QSlider::sliderReleased, this, &MainWindow::micvolumeSlider_sliderReleased);
    connect(ui->rotateOn, &QPushButton::clicked, this, &MainWindow::rotateOn_clicked);
    connect(ui->rotateOff, &QPushButton::clicked, this, &MainWindow::rotateOff_clicked);

    // Bluetooth Section
    connect(ui->btwhenonOffButton, &QPushButton::clicked, this, &MainWindow::btwhenonOffButton_clicked);
    connect(ui->btwhenonOnButton, &QPushButton::clicked, this, &MainWindow::btwhenonOnButton_clicked);
    connect(ui->btbothRadioButton, &QRadioButton::clicked, this, &MainWindow::btbothRadioButton_clicked);
    connect(ui->btpcdbRadioButton, &QRadioButton::clicked, this, &MainWindow::btpcdbRadioButton_clicked);
    connect(ui->btonlyRadioButton, &QRadioButton::clicked, this, &MainWindow::btonlyRadioButton_clicked);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::Trigger)
    {
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
        }
    }
}

bool MainWindow::isOsDarkMode(){
    // Check if the application is using a dark palette
    QPalette palette = QApplication::palette();
    QColor textColor = palette.color(QPalette::WindowText);
    QColor backgroundColor = palette.color(QPalette::Window);

    // If text is brighter than background, it's likely a dark theme
    return textColor.lightness() > backgroundColor.lightness();
}

void MainWindow::updateIcons(){
    QString inv = "";
    if(darkMode){
        inv = "-inv";
        trayIconPath.replace(".png", "-inv.png");
    }
    else{
        trayIconPath.replace("-inv.png", ".png");
    }

    this->setWindowIcon(QIcon(":/icons/headphones"+inv+".png"));
    tray->setIcon(QIcon(trayIconPath));
}

void MainWindow::disableFrames(){
    ledOn->setEnabled(false);
    ledOff->setEnabled(false);

    ui->notSupportedFrame->setHidden(false);

    ui->deviceinfoFrame->setHidden(true);
    ui->batteryFrame->setHidden(true);

    ui->tabWidget->hide();
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
    QList<Device*> c=getConnectedDevices(), s=deserializeDevices(FILE_DEVICES_SETTINGS);
    n_connected=c.length(); n_saved=s.length();
    deviceList = mergeDevices(c, s);
}

void MainWindow::loadDevice(int deviceIndex){
    disableFrames();

    if(deviceIndex<0){
        selectedDevice=nullptr;
        return;
    }

    selectedDevice=deviceList.value(deviceIndex);
    QSet<QString>& capabilities=selectedDevice->capabilities;

    if(timerGUI!=nullptr){
        timerGUI->stop();
        timerGUI=nullptr;
    }
    timerGUI = new QTimer(this);
    connect(timerGUI, SIGNAL(timeout()), this, SLOT(updateDevice()));
    connect(timerGUI, SIGNAL(timeout()), this, SLOT(saveDevicesSettings()));
    connect(timerGUI, SIGNAL(timeout()), this, SLOT(updateGUI()));
    timerGUI->start(settings.msecUpdateIntervalTime);

    ui->notSupportedFrame->setHidden(true);

    //Info section
    ui->deviceinfovalueLabel->setText(selectedDevice->device+"\n"+selectedDevice->vendor+"\n"+selectedDevice->product);
    ui->deviceinfoFrame->setHidden(false);
    if (capabilities.contains("CAP_BATTERY_STATUS")){
        ui->batteryFrame->setHidden(false);
        this->setBatteryStatus();
        qDebug() << "Battery percentage supported";
    }

    ui->tabWidget->show();
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
    if (capabilities.contains("CAP_EQUALIZER_PRESET") && !selectedDevice->presets_list.empty()){
        ui->equalizerpresetFrame->setHidden(false);
        ui->tabWidget->setTabEnabled(1, true);
        qDebug() << "Eqaulizer preset supported";
    }
    if (capabilities.contains("CAP_EQUALIZER") && selectedDevice->equalizer.bands_number>0){
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
    this->setMaximumHeight(this->minimumHeight());
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
    if(selectedDevice->equalizer.bands_number>0){
        int i;
        for (i = 0; i < selectedDevice->equalizer.bands_number; ++i) {
            QLabel *l = new QLabel(QString::number(i));
            l->setAlignment(Qt::AlignHCenter);

            QSlider *s = new QSlider(Qt::Vertical);
            s->setMaximum(selectedDevice->equalizer.band_max/selectedDevice->equalizer.band_step);
            s->setMinimum(selectedDevice->equalizer.band_min/selectedDevice->equalizer.band_step);
            s->setSingleStep(1);
            s->setTickInterval(1/selectedDevice->equalizer.band_step);
            s->setTickPosition(QSlider::TicksBothSides);
            if(selectedDevice->equalizer_curve.size()==selectedDevice->equalizer.bands_number){
                s->setValue(selectedDevice->equalizer_curve.value(i));
            } else{
                s->setValue(selectedDevice->equalizer.band_baseline);
            }

            QVBoxLayout *lb = new QVBoxLayout();
            lb->addWidget(l);
            lb->addWidget(s);

            slidersEq.append(s);
            equalizerLayout->addLayout(lb);
        }
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

void MainWindow::saveDevicesSettings(){
    if(!savedDevices){
        serializeDevices(deviceList, FILE_DEVICES_SETTINGS);
    }
}

void MainWindow::updateDevice(){
    //serializeDevices(deviceList, "devices.json");
    if(selectedDevice!=nullptr){
        QList<Device*> newDl=getConnectedDevices();
        selectedDevice->updateDevice(newDl);
    }
}

void MainWindow::updateGUI(){
    setBatteryStatus();
    setChatmixStatus();
}

//Info Section Events
void MainWindow::setBatteryStatus()
{
    QString status = selectedDevice->battery.status;
    int batteryLevel = selectedDevice->battery.level;
    QString level=QString::number(batteryLevel);

    if(batteryLevel>=0){
        ui->batteryProgressBar->show();
        ui->batteryProgressBar->setValue(batteryLevel);
    }
    else{
        ui->batteryProgressBar->hide();
    }

    if (status == "BATTERY_UNAVAILABLE"){
        ui->batteryPercentage->setText(tr("Headset Off"));
        tray->setToolTip(tr("HeadsetControl \r\nHeadset Off"));
        trayIconPath =":/icons/headphones-inv.png";
    }
    else if (status == "BATTERY_CHARGING") {
        ui->batteryPercentage->setText(level+tr("% - Charging"));
        tray->setToolTip(tr("HeadsetControl \r\nBattery Charging"));
        trayIconPath = ":/icons/battery-charging-inv.png";
    }
    else if(status == "BATTERY_AVAILABLE"){
        ui->batteryPercentage->setText(level+tr("% - Descharging"));
        tray->setToolTip(tr("HeadsetControl \r\nBattery: ") + level + "%");
        if (level.toInt() > 75){
            trayIconPath = ":/icons/battery-level-full-inv.png";
            notified = false;
        }
        else if (level.toInt() > settings.batteryLowThreshold) {
            trayIconPath = ":/icons/battery-medium-inv.png";
            notified = false;
        }
        else {
            trayIconPath = ":/icons/battery-low-inv.png";
            if (!notified){
                tray->showMessage(tr("Battery Alert!"), tr("The battery of your headset is running low"), QIcon(":/icons/battery-low-inv.png"));
                notified = true;
            }
        }
    } else{
        ui->batteryPercentage->setText(tr("No battery info"));
        tray->setToolTip("HeadsetControl");
        trayIconPath = ":/icons/headphones-inv.png";
    }

    if(!darkMode){
        trayIconPath.replace("-inv", "");
    }
    tray->setIcon(QIcon(trayIconPath));
}

//Other Section Events
void MainWindow::onlightButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->lights=1;
        savedDevices=false;
    }
}

void MainWindow::offlightButton_clicked()
{
    QStringList args=QStringList() << QString("--light") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->lights=0;
        savedDevices=false;
    }
}

void MainWindow::sidetoneSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--sidetone") << QString::number(ui->sidetoneSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->sidetone=ui->sidetoneSlider->value();
        savedDevices=false;
    }
}

void MainWindow::voiceOnButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->voice_prompts=1;
        savedDevices=false;
    }
}

void MainWindow::voiceOffButton_clicked()
{
    QStringList args=QStringList() << QString("--voice-prompt") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->voice_prompts=0;
        savedDevices=false;
    }
}

void MainWindow::notification0Button_clicked()
{
    QStringList args=QStringList() << QString("--notificate") << QString("0");
    Action s=sendAction(args);
    if(s.status!="success"){

    }
}

void MainWindow::notification1Button_clicked()
{
    QStringList args=QStringList() << QString("--notificate") << QString("1");
    Action s=sendAction(args);
    if(s.status!="success"){

    }
}

void MainWindow::rotateOn_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->rotate_to_mute=1;
        savedDevices=false;
    }
}

void MainWindow::rotateOff_clicked()
{
    QStringList args=QStringList() << QString("--rotate-to-mute") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->rotate_to_mute=0;
        savedDevices=false;
    }
}

void MainWindow::inactivitySlider_sliderReleased(){
    QStringList args=QStringList() << QString("--inactive-time") << QString::number(ui->inactivitySlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->inactive_time=ui->inactivitySlider->value();
        savedDevices=false;
    }
}

void MainWindow::setChatmixStatus(){
    int chatmix = selectedDevice->chatmix;
    QString chatmixValue = QString::number(chatmix);
    QString chatmixStatus;
    if(chatmix<65)chatmixStatus=tr("Game");
    else if(chatmix>65)chatmixStatus=tr("Chat");
    ui->chatmixvalueLabel->setText(chatmixValue);
    ui->chatmixstatusLabel->setText(chatmixStatus);
}

//Equalizer Section Events
void MainWindow::equalizerPresetcomboBox_currentIndexChanged(){
    int preset=ui->equalizerPresetcomboBox->currentIndex();
    if(preset==0){
        //setSliders(selectedDevice->equalizer.band_baseline);
    } else if(preset>=1 && preset<=selectedDevice->presets_list.length()){
        this->setSliders(selectedDevice->presets_list.value(preset-1).values);
        QStringList args=QStringList() << QString("--equalizer-preset") << QString::number(preset-1);
        Action s=sendAction(args);
        if(s.status=="success"){
            selectedDevice->equalizer_preset=ui->equalizerPresetcomboBox->currentIndex();
            savedDevices=false;
        }
    }
}

void MainWindow::applyEqualizer_clicked(){
    ui->equalizerPresetcomboBox->setCurrentIndex(0);
    QString eq_string="";
    QList<int> values;
    for (QSlider* slider : slidersEq) {
        eq_string+= QString::number(slider->value())+",";
        values.append(slider->value()/selectedDevice->equalizer.band_step);
    }
    eq_string.removeLast();
    QStringList args=QStringList() << QString("--equalizer") << eq_string;
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->equalizer_curve=values;
        selectedDevice->equalizer_preset=-1;
        savedDevices=false;
    }
}

void MainWindow::setSliders(int value){
    for (QSlider* slider : slidersEq) {
        slider->setValue(value/selectedDevice->equalizer.band_step);
    }
}

void MainWindow::setSliders(QList<double> values){
    int i=0;
    if(values.length()==selectedDevice->equalizer.bands_number){
        for (QSlider* slider : slidersEq) {
            slider->setValue(values[i++]/selectedDevice->equalizer.band_step);
        }
    }
    else{
        qDebug() << "ERROR: Bad Equalizer Preset";
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

void MainWindow::volumelimiterOffButton_clicked(){
    QStringList args=QStringList() << QString("--volume-limiter") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->volume_limiter=0;
        savedDevices=false;
    }
}

void MainWindow::volumelimiterOnButton_clicked(){
    QStringList args=QStringList() << QString("--volume-limiter") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->volume_limiter=1;
        savedDevices=false;
    }
}

//Microphone Section Events
void MainWindow::muteledbrightnessSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--microphone-mute-led-brightness") << QString::number(ui->muteledbrightnessSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->mic_mute_led_brightness=ui->muteledbrightnessSlider->value();
        savedDevices=false;
    }
}

void MainWindow::micvolumeSlider_sliderReleased(){
    QStringList args=QStringList() << QString("--microphone-volume") << QString::number(ui->micvolumeSlider->sliderPosition());
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->mic_volume=ui->micvolumeSlider->value();
        savedDevices=false;
    }
}

//Bluetooth Section Events
void MainWindow::btwhenonOffButton_clicked(){
    QStringList args=QStringList() << QString("--bt-when-powered-on") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->bt_when_powered_on=0;
        savedDevices=false;
    }
}

void MainWindow::btwhenonOnButton_clicked(){
    QStringList args=QStringList() << QString("--bt-when-powered-on") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->bt_when_powered_on=1;
        savedDevices=false;
    }
}

void MainWindow::btbothRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("0");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->bt_call_volume=0;
        savedDevices=false;
    }
}

void MainWindow::btpcdbRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("1");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->bt_call_volume=1;
        savedDevices=false;
    }
}

void MainWindow::btonlyRadioButton_clicked(){
    QStringList args=QStringList() << QString("--bt-call-volume") << QString("2");
    Action s=sendAction(args);
    if(s.status=="success"){
        selectedDevice->bt_call_volume=2;
        savedDevices=false;
    }
}

//Tool Bar Events
void MainWindow::editProgramSetting(){
    SettingsWindow* settingsW=new SettingsWindow(settings, this);
    if (settingsW->exec() == QDialog::Accepted) {
        settings=settingsW->getSettings();
        saveSettingstoFile(settings, PROGRAM_SETTINGS_FILENAME);
        timerGUI->setInterval(settings.msecUpdateIntervalTime);
    }
}

void MainWindow::selectDevice(){
    this->loadDevices();

    QStringList devices=QStringList();
    for (Device* device : deviceList){
        if(device->connected){
            devices<<device->device;
        }
    }

    LoaddeviceWindow* loadDevWindow=new LoaddeviceWindow(devices, this);
    if (loadDevWindow->exec() == QDialog::Accepted) {
        int index = loadDevWindow->getDeviceIndex();
        this->disableFrames();
        if (index>=0) {
            if(index==0){
                ui->tabWidget->setDisabled(false);
            }
            else {
                ui->tabWidget->setDisabled(true);
            }
            this->loadDevice(index);
        }
    }
}

void MainWindow::checkForUpdates(){
    DialogInfo* dialogWindow=new DialogInfo(this);
    dialogWindow->setTitle(tr("Check for updates"));

    const QVersionNumber& local_hc=getHCVersion();
    const QVersionNumber local_gui=QVersionNumber::fromString(qApp->applicationVersion());
    QString v1 = getLatestGitHubReleaseVersion("Sapd","HeadsetControl");
    QString v2 = getLatestGitHubReleaseVersion("nicola02nb","HeadsetControl-GUI");
    QVersionNumber remote_hc =QVersionNumber::fromString(v1);
    QVersionNumber remote_gui =QVersionNumber::fromString(v2);
    QString s1 = tr("up-to date v")+local_hc.toString();
    QString s2 = tr("up-to date v")+local_gui.toString();
    if(!(v1=="") && remote_hc>local_hc){
        s1=tr("Newer version")+" -><a href=\"https://github.com/Sapd/HeadsetControl/releases/latest\">"+remote_hc.toString()+"</a>";
    }
    if(!(v2=="") && remote_gui>local_gui){
        s2=tr("Newer version")+" -><a href=\"https://github.com/nicola02nb/HeadsetControl-GUI/releases/latest\">"+remote_gui.toString()+"</a>";
    }

    QString text = "HeadesetControl: "+s1+"<br>HeadesetControl-GUI: "+s2;
    dialogWindow->setLabel(text);

    dialogWindow->show();
}

void MainWindow::showAbout(){
    DialogInfo* dialogWindow=new DialogInfo(this);
    dialogWindow->setTitle(tr("About this program"));
    QString text = tr("<a href='https://github.com/nicola02nb/HeadsetControl-GUI'>This</a> is a forked version of <a href='https://github.com/LeoKlaus/HeadsetControl-GUI'>HeadsetControl-GUI</a>."
                   "<br>Made by <a href='https://github.com/nicola02nb/HeadsetControl-GUI'>nicola02nb</a>"
                      "<br>Version: ")+qApp->applicationVersion();
    dialogWindow->setLabel(text);

    dialogWindow->show();
}

void MainWindow::showCredits(){
    DialogInfo* dialogWindow=new DialogInfo(this);
    dialogWindow->setTitle(tr("Credits"));
    QString text = tr("Big shout-out to:"
                   "<br> - Sapd for <a href='https://github.com/Sapd/HeadsetControl'>HeadsetCoontrol</a>"
                      "<br> - LeoKlaus for <a href='https://github.com/LeoKlaus/HeadsetControl-GUI'>HeadsetControl-GUI</a>");
    dialogWindow->setLabel(text);

    dialogWindow->show();
}


void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type()){
    case QEvent::PaletteChange:
        darkMode = isOsDarkMode();
        updateIcons();
        break;
    case QEvent::WindowStateChange:
        if (this->windowState()==Qt::WindowMinimized){
            this->hide();
        }
    default:
        break;
    }

    QMainWindow::changeEvent(e);
}
