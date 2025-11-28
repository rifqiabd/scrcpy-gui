#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSettings>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    void saveSettings();

private:
    void setupUI();
    void loadSettings();

    QTabWidget *tabWidget;
    
    // General
    QCheckBox *alwaysOnTopCheck;
    QCheckBox *noControlCheck;
    QCheckBox *stayAwakeCheck;
    QCheckBox *turnScreenOffCheck;
    QCheckBox *noVdDestroyContentCheck;
    QCheckBox *showTouchesCheck;
    QCheckBox *disableScreensaverCheck;

    // Video
    QSpinBox *maxSizeSpin;
    QComboBox *bitRateCombo;
    QSpinBox *maxFpsSpin;
    QComboBox *videoCodecCombo;
    QComboBox *rotationCombo;
    QCheckBox *noMipmapsCheck;

    // Audio
    QCheckBox *noAudioCheck;
    QComboBox *audioCodecCombo;
    QComboBox *audioBitRateCombo;

    // Window
    QCheckBox *fullscreenCheck;
    QCheckBox *borderlessCheck;
    QLineEdit *windowTitleEdit;

    // Advanced
    QLineEdit *customArgsEdit;
};

#endif // SETTINGSDIALOG_H
