#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT

    // General
    Q_PROPERTY(bool alwaysOnTop READ alwaysOnTop WRITE setAlwaysOnTop NOTIFY settingsChanged)
    Q_PROPERTY(bool noControl READ noControl WRITE setNoControl NOTIFY settingsChanged)
    Q_PROPERTY(bool stayAwake READ stayAwake WRITE setStayAwake NOTIFY settingsChanged)
    Q_PROPERTY(bool turnScreenOff READ turnScreenOff WRITE setTurnScreenOff NOTIFY settingsChanged)
    Q_PROPERTY(bool noVdDestroyContent READ noVdDestroyContent WRITE setNoVdDestroyContent NOTIFY settingsChanged)
    Q_PROPERTY(bool showTouches READ showTouches WRITE setShowTouches NOTIFY settingsChanged)
    Q_PROPERTY(bool disableScreensaver READ disableScreensaver WRITE setDisableScreensaver NOTIFY settingsChanged)

    // Video
    Q_PROPERTY(int maxSize READ maxSize WRITE setMaxSize NOTIFY settingsChanged)
    Q_PROPERTY(QString bitRate READ bitRate WRITE setBitRate NOTIFY settingsChanged)
    Q_PROPERTY(int maxFps READ maxFps WRITE setMaxFps NOTIFY settingsChanged)
    Q_PROPERTY(QString videoCodec READ videoCodec WRITE setVideoCodec NOTIFY settingsChanged)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY settingsChanged)
    Q_PROPERTY(bool noMipmaps READ noMipmaps WRITE setNoMipmaps NOTIFY settingsChanged)

    // Audio
    Q_PROPERTY(bool noAudio READ noAudio WRITE setNoAudio NOTIFY settingsChanged)
    Q_PROPERTY(QString audioCodec READ audioCodec WRITE setAudioCodec NOTIFY settingsChanged)
    Q_PROPERTY(QString audioBitRate READ audioBitRate WRITE setAudioBitRate NOTIFY settingsChanged)

    // Window
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY settingsChanged)
    Q_PROPERTY(bool windowBorderless READ windowBorderless WRITE setWindowBorderless NOTIFY settingsChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle NOTIFY settingsChanged)

    // Advanced
    Q_PROPERTY(QString customArgs READ customArgs WRITE setCustomArgs NOTIFY settingsChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    // General getters
    bool alwaysOnTop() const;
    bool noControl() const;
    bool stayAwake() const;
    bool turnScreenOff() const;
    bool noVdDestroyContent() const;
    bool showTouches() const;
    bool disableScreensaver() const;

    // Video getters
    int maxSize() const;
    QString bitRate() const;
    int maxFps() const;
    QString videoCodec() const;
    int rotation() const;
    bool noMipmaps() const;

    // Audio getters
    bool noAudio() const;
    QString audioCodec() const;
    QString audioBitRate() const;

    // Window getters
    bool fullscreen() const;
    bool windowBorderless() const;
    QString windowTitle() const;

    // Advanced getters
    QString customArgs() const;

    // Setters
    void setAlwaysOnTop(bool value);
    void setNoControl(bool value);
    void setStayAwake(bool value);
    void setTurnScreenOff(bool value);
    void setNoVdDestroyContent(bool value);
    void setShowTouches(bool value);
    void setDisableScreensaver(bool value);
    void setMaxSize(int value);
    void setBitRate(const QString &value);
    void setMaxFps(int value);
    void setVideoCodec(const QString &value);
    void setRotation(int value);
    void setNoMipmaps(bool value);
    void setNoAudio(bool value);
    void setAudioCodec(const QString &value);
    void setAudioBitRate(const QString &value);
    void setFullscreen(bool value);
    void setWindowBorderless(bool value);
    void setWindowTitle(const QString &value);
    void setCustomArgs(const QString &value);

    Q_INVOKABLE void saveAllSettings();

signals:
    void settingsChanged();

private:
    void loadSettings();

    QSettings settings;
};

#endif // SETTINGSMANAGER_H
