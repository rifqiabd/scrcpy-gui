#include "appmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>

AppManager::AppManager(QObject *parent)
    : QObject(parent)
    , adbProcess(nullptr)
{
    loadCustomApps();
}

AppManager::~AppManager()
{
    saveCustomApps();
}

void AppManager::loadApps()
{
    allApps.clear();

    // First, add custom apps
    allApps.append(customApps);

    // Then query ADB for installed apps
    if (adbProcess) {
        delete adbProcess;
    }

    adbProcess = new QProcess(this);

    connect(adbProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &AppManager::onAdbFinished);
    connect(adbProcess, &QProcess::errorOccurred, this, &AppManager::onAdbError);

    QStringList arguments;
    arguments << "shell" << "pm" << "list" << "packages" << "-3";

    qDebug() << "Running ADB command:" << arguments;
    adbProcess->start("adb", arguments);
}

void AppManager::onAdbFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit loadError("ADB command failed with exit code: " + QString::number(exitCode));
        return;
    }

    QString output = adbProcess->readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    qDebug() << "ADB returned" << lines.size() << "packages";

    for (const QString &line : lines) {
        // Format: "package:com.example.app"
        if (line.startsWith("package:")) {
            QString packageName = line.mid(8).trimmed();

            // Skip if already in custom apps
            bool isCustom = false;
            for (const AppInfo &customApp : customApps) {
                if (customApp.packageName == packageName) {
                    isCustom = true;
                    break;
                }
            }

            if (!isCustom) {
                AppInfo app;
                app.packageName = packageName;
                app.name = packageToName(packageName);
                app.isCustom = false;
                allApps.append(app);
            }
        }
    }

    // Sort alphabetically by name
    std::sort(allApps.begin(), allApps.end(), 
              [](const AppInfo &a, const AppInfo &b) {
                  return a.name.toLower() < b.name.toLower();
              });

    emit appsLoaded(allApps);
}

void AppManager::onAdbError(QProcess::ProcessError error)
{
    QString errorMsg;

    if (error == QProcess::FailedToStart) {
        errorMsg = "Failed to start ADB.\n\n"
                  "Make sure ADB is installed and in your PATH.\n"
                  "You can install it as part of Android SDK Platform Tools.";
    } else {
        errorMsg = "ADB error: " + adbProcess->errorString();
    }

    qDebug() << "ADB error:" << errorMsg;

    // Still emit what we have (custom apps)
    if (!customApps.isEmpty()) {
        allApps = customApps;
        emit appsLoaded(allApps);
    } else {
        emit loadError(errorMsg);
    }
}

QString AppManager::packageToName(const QString &packageName)
{
    // Simple conversion: com.example.myapp -> My App
    QStringList parts = packageName.split('.');
    if (parts.isEmpty()) {
        return packageName;
    }

    QString appName = parts.last();

    // Capitalize first letter and add spaces before capitals
    QString result;
    for (int i = 0; i < appName.length(); ++i) {
        QChar c = appName[i];
        if (i == 0) {
            result += c.toUpper();
        } else if (c.isUpper()) {
            result += ' ' + c;
        } else {
            result += c;
        }
    }

    return result;
}

void AppManager::saveCustomApp(const AppInfo &app)
{
    // Check if already exists
    for (const AppInfo &existing : customApps) {
        if (existing.packageName == app.packageName) {
            return; // Already exists
        }
    }

    customApps.append(app);
    saveCustomApps();
}

QList<AppInfo> AppManager::getCustomApps()
{
    return customApps;
}

QString AppManager::getConfigFilePath()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return configDir + "/config.json";
}

void AppManager::loadCustomApps()
{
    QString configPath = getConfigFilePath();
    QFile file(configPath);

    if (!file.exists()) {
        qDebug() << "No config file found at:" << configPath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open config file:" << configPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON in config file";
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray appsArray = root["customApps"].toArray();

    for (const QJsonValue &value : appsArray) {
        QJsonObject appObj = value.toObject();
        AppInfo app;
        app.name = appObj["name"].toString();
        app.packageName = appObj["package"].toString();
        app.isCustom = true;
        customApps.append(app);
    }

    qDebug() << "Loaded" << customApps.size() << "custom apps from config";
}

void AppManager::saveCustomApps()
{
    QJsonArray appsArray;

    for (const AppInfo &app : customApps) {
        QJsonObject appObj;
        appObj["name"] = app.name;
        appObj["package"] = app.packageName;
        appsArray.append(appObj);
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["customApps"] = appsArray;

    QJsonDocument doc(root);

    QString configPath = getConfigFilePath();
    QFile file(configPath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save config file:" << configPath;
        return;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved config to:" << configPath;
}
