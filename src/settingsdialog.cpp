#include "settingsdialog.h"
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Scrcpy Settings");
    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    tabWidget = new QTabWidget(this);

    // --- General Tab ---
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);
    
    alwaysOnTopCheck = new QCheckBox("Always on top (--always-on-top)");
    noControlCheck = new QCheckBox("No control (read-only) (--no-control)");
    stayAwakeCheck = new QCheckBox("Stay awake (--stay-awake)");
    turnScreenOffCheck = new QCheckBox("Turn screen off (--turn-screen-off)");
    noVdDestroyContentCheck = new QCheckBox("No virtual display content destruction (--no-vd-destroy-content)");
    showTouchesCheck = new QCheckBox("Show touches (--show-touches)");
    disableScreensaverCheck = new QCheckBox("Disable screensaver (--disable-screensaver)");

    generalLayout->addWidget(alwaysOnTopCheck);
    generalLayout->addWidget(noControlCheck);
    generalLayout->addWidget(stayAwakeCheck);
    generalLayout->addWidget(turnScreenOffCheck);
    generalLayout->addWidget(noVdDestroyContentCheck);
    generalLayout->addWidget(showTouchesCheck);
    generalLayout->addWidget(disableScreensaverCheck);
    generalLayout->addStretch();
    tabWidget->addTab(generalTab, "General");

    // --- Video Tab ---
    QWidget *videoTab = new QWidget();
    QFormLayout *videoLayout = new QFormLayout(videoTab);

    maxSizeSpin = new QSpinBox();
    maxSizeSpin->setRange(0, 8000);
    maxSizeSpin->setSpecialValueText("Unlimited");
    maxSizeSpin->setSingleStep(100);
    maxSizeSpin->setSuffix(" px");

    bitRateCombo = new QComboBox();
    bitRateCombo->addItems({"Default (8M)", "2M", "4M", "8M", "10M", "12M", "16M", "20M"});

    maxFpsSpin = new QSpinBox();
    maxFpsSpin->setRange(0, 120);
    maxFpsSpin->setSpecialValueText("Unlimited");

    videoCodecCombo = new QComboBox();
    videoCodecCombo->addItems({"Default (h264)", "h264", "h265", "av1"});

    rotationCombo = new QComboBox();
    rotationCombo->addItems({"0", "1 (-90)", "2 (180)", "3 (90)"});

    noMipmapsCheck = new QCheckBox("Disable mipmaps (--no-mipmaps)");

    videoLayout->addRow("Max Size:", maxSizeSpin);
    videoLayout->addRow("Bit Rate:", bitRateCombo);
    videoLayout->addRow("Max FPS:", maxFpsSpin);
    videoLayout->addRow("Video Codec:", videoCodecCombo);
    videoLayout->addRow("Rotation:", rotationCombo);
    videoLayout->addRow("", noMipmapsCheck);
    tabWidget->addTab(videoTab, "Video");

    // --- Audio Tab ---
    QWidget *audioTab = new QWidget();
    QFormLayout *audioLayout = new QFormLayout(audioTab);

    noAudioCheck = new QCheckBox("Disable Audio (--no-audio)");
    
    audioCodecCombo = new QComboBox();
    audioCodecCombo->addItems({"Default (opus)", "opus", "aac", "raw"});

    audioBitRateCombo = new QComboBox();
    audioBitRateCombo->addItems({"Default (128K)", "64K", "128K", "192K", "256K", "320K"});

    audioLayout->addRow("", noAudioCheck);
    audioLayout->addRow("Audio Codec:", audioCodecCombo);
    audioLayout->addRow("Audio Bit Rate:", audioBitRateCombo);
    tabWidget->addTab(audioTab, "Audio");

    // --- Window Tab ---
    QWidget *windowTab = new QWidget();
    QVBoxLayout *windowLayout = new QVBoxLayout(windowTab);

    fullscreenCheck = new QCheckBox("Fullscreen (--fullscreen)");
    borderlessCheck = new QCheckBox("Borderless (--window-borderless)");
    
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel("Window Title:"));
    windowTitleEdit = new QLineEdit();
    windowTitleEdit->setPlaceholderText("Default");
    titleLayout->addWidget(windowTitleEdit);

    windowLayout->addWidget(fullscreenCheck);
    windowLayout->addWidget(borderlessCheck);
    windowLayout->addLayout(titleLayout);
    windowLayout->addStretch();
    tabWidget->addTab(windowTab, "Window");

    // --- Advanced Tab ---
    QWidget *advancedTab = new QWidget();
    QVBoxLayout *advancedLayout = new QVBoxLayout(advancedTab);

    advancedLayout->addWidget(new QLabel("Custom Arguments (space separated):"));
    customArgsEdit = new QLineEdit();
    customArgsEdit->setPlaceholderText("e.g. --render-driver=opengl");
    advancedLayout->addWidget(customArgsEdit);
    advancedLayout->addStretch();
    tabWidget->addTab(advancedTab, "Advanced");

    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::loadSettings()
{
    QSettings settings("ScrcpyGUI", "Settings");

    // General
    alwaysOnTopCheck->setChecked(settings.value("always-on-top", false).toBool());
    noControlCheck->setChecked(settings.value("no-control", false).toBool());
    stayAwakeCheck->setChecked(settings.value("stay-awake", false).toBool());
    turnScreenOffCheck->setChecked(settings.value("turn-screen-off", false).toBool());
    noVdDestroyContentCheck->setChecked(settings.value("no-vd-destroy-content", false).toBool());
    showTouchesCheck->setChecked(settings.value("show-touches", false).toBool());
    disableScreensaverCheck->setChecked(settings.value("disable-screensaver", false).toBool());

    // Video
    maxSizeSpin->setValue(settings.value("max-size", 0).toInt());
    bitRateCombo->setCurrentText(settings.value("bit-rate", "Default (8M)").toString());
    maxFpsSpin->setValue(settings.value("max-fps", 0).toInt());
    videoCodecCombo->setCurrentText(settings.value("video-codec", "Default (h264)").toString());
    rotationCombo->setCurrentIndex(settings.value("rotation", 0).toInt());
    noMipmapsCheck->setChecked(settings.value("no-mipmaps", false).toBool());

    // Audio
    noAudioCheck->setChecked(settings.value("no-audio", false).toBool());
    audioCodecCombo->setCurrentText(settings.value("audio-codec", "Default (opus)").toString());
    audioBitRateCombo->setCurrentText(settings.value("audio-bit-rate", "Default (128K)").toString());

    // Window
    fullscreenCheck->setChecked(settings.value("fullscreen", false).toBool());
    borderlessCheck->setChecked(settings.value("window-borderless", false).toBool());
    windowTitleEdit->setText(settings.value("window-title", "").toString());

    // Advanced
    customArgsEdit->setText(settings.value("custom-args", "").toString());
}

void SettingsDialog::saveSettings()
{
    QSettings settings("ScrcpyGUI", "Settings");

    // General
    settings.setValue("always-on-top", alwaysOnTopCheck->isChecked());
    settings.setValue("no-control", noControlCheck->isChecked());
    settings.setValue("stay-awake", stayAwakeCheck->isChecked());
    settings.setValue("turn-screen-off", turnScreenOffCheck->isChecked());
    settings.setValue("no-vd-destroy-content", noVdDestroyContentCheck->isChecked());
    settings.setValue("show-touches", showTouchesCheck->isChecked());
    settings.setValue("disable-screensaver", disableScreensaverCheck->isChecked());

    // Video
    settings.setValue("max-size", maxSizeSpin->value());
    settings.setValue("bit-rate", bitRateCombo->currentText());
    settings.setValue("max-fps", maxFpsSpin->value());
    settings.setValue("video-codec", videoCodecCombo->currentText());
    settings.setValue("rotation", rotationCombo->currentIndex());
    settings.setValue("no-mipmaps", noMipmapsCheck->isChecked());

    // Audio
    settings.setValue("no-audio", noAudioCheck->isChecked());
    settings.setValue("audio-codec", audioCodecCombo->currentText());
    settings.setValue("audio-bit-rate", audioBitRateCombo->currentText());

    // Window
    settings.setValue("fullscreen", fullscreenCheck->isChecked());
    settings.setValue("window-borderless", borderlessCheck->isChecked());
    settings.setValue("window-title", windowTitleEdit->text());

    // Advanced
    settings.setValue("custom-args", customArgsEdit->text());
}
