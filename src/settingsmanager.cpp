#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , settings("ScrcpyGUI", "Settings")
{
    loadSettings();
}

void SettingsManager::loadSettings()
{
    // Settings are loaded directly from QSettings when accessed
}

// General getters
bool SettingsManager::alwaysOnTop() const { return settings.value("always-on-top", false).toBool(); }
bool SettingsManager::noControl() const { return settings.value("no-control", false).toBool(); }
bool SettingsManager::stayAwake() const { return settings.value("stay-awake", false).toBool(); }
bool SettingsManager::turnScreenOff() const { return settings.value("turn-screen-off", false).toBool(); }
bool SettingsManager::noVdDestroyContent() const { return settings.value("no-vd-destroy-content", false).toBool(); }
bool SettingsManager::showTouches() const { return settings.value("show-touches", false).toBool(); }
bool SettingsManager::disableScreensaver() const { return settings.value("disable-screensaver", false).toBool(); }

// Video getters
int SettingsManager::maxSize() const { return settings.value("max-size", 0).toInt(); }
QString SettingsManager::bitRate() const { return settings.value("bit-rate", "Default (8M)").toString(); }
int SettingsManager::maxFps() const { return settings.value("max-fps", 0).toInt(); }
QString SettingsManager::videoCodec() const { return settings.value("video-codec", "Default (h264)").toString(); }
int SettingsManager::rotation() const { return settings.value("rotation", 0).toInt(); }
bool SettingsManager::noMipmaps() const { return settings.value("no-mipmaps", false).toBool(); }

// Audio getters
bool SettingsManager::noAudio() const { return settings.value("no-audio", false).toBool(); }
QString SettingsManager::audioCodec() const { return settings.value("audio-codec", "Default (opus)").toString(); }
QString SettingsManager::audioBitRate() const { return settings.value("audio-bit-rate", "Default (128K)").toString(); }

// Window getters
bool SettingsManager::fullscreen() const { return settings.value("fullscreen", false).toBool(); }
bool SettingsManager::windowBorderless() const { return settings.value("window-borderless", false).toBool(); }
QString SettingsManager::windowTitle() const { return settings.value("window-title", "").toString(); }

// Advanced getters
QString SettingsManager::customArgs() const { return settings.value("custom-args", "").toString(); }

// Setters
void SettingsManager::setAlwaysOnTop(bool value) { settings.setValue("always-on-top", value); }
void SettingsManager::setNoControl(bool value) { settings.setValue("no-control", value); }
void SettingsManager::setStayAwake(bool value) { settings.setValue("stay-awake", value); }
void SettingsManager::setTurnScreenOff(bool value) { settings.setValue("turn-screen-off", value); }
void SettingsManager::setNoVdDestroyContent(bool value) { settings.setValue("no-vd-destroy-content", value); }
void SettingsManager::setShowTouches(bool value) { settings.setValue("show-touches", value); }
void SettingsManager::setDisableScreensaver(bool value) { settings.setValue("disable-screensaver", value); }
void SettingsManager::setMaxSize(int value) { settings.setValue("max-size", value); }
void SettingsManager::setBitRate(const QString &value) { settings.setValue("bit-rate", value); }
void SettingsManager::setMaxFps(int value) { settings.setValue("max-fps", value); }
void SettingsManager::setVideoCodec(const QString &value) { settings.setValue("video-codec", value); }
void SettingsManager::setRotation(int value) { settings.setValue("rotation", value); }
void SettingsManager::setNoMipmaps(bool value) { settings.setValue("no-mipmaps", value); }
void SettingsManager::setNoAudio(bool value) { settings.setValue("no-audio", value); }
void SettingsManager::setAudioCodec(const QString &value) { settings.setValue("audio-codec", value); }
void SettingsManager::setAudioBitRate(const QString &value) { settings.setValue("audio-bit-rate", value); }
void SettingsManager::setFullscreen(bool value) { settings.setValue("fullscreen", value); }
void SettingsManager::setWindowBorderless(bool value) { settings.setValue("window-borderless", value); }
void SettingsManager::setWindowTitle(const QString &value) { settings.setValue("window-title", value); }
void SettingsManager::setCustomArgs(const QString &value) { settings.setValue("custom-args", value); }

void SettingsManager::saveAllSettings()
{
    settings.sync();
    emit settingsChanged();
}
