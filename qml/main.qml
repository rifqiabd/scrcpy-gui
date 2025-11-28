import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 1000
    height: 600
    minimumWidth: 800
    minimumHeight: 500
    title: "Qt GUI Scrcpy"

    // Platform-adaptive styling will be set from C++ main.cpp
    
    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            
            Action {
                text: qsTr("&Refresh")
                shortcut: "Ctrl+R"
                onTriggered: mainWindow.onRefreshClicked()
            }
            
            Action {
                text: qsTr("Manual &Add")
                shortcut: "Ctrl+N"
                onTriggered: mainWindow.onManualAddClicked()
            }
            
            MenuSeparator {}
            
            Action {
                text: qsTr("E&xit")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
        
        Menu {
            title: qsTr("&Help")
            
            Action {
                text: qsTr("&About")
                onTriggered: mainWindow.onAbout()
            }
        }

        Menu {
            title: qsTr("&Settings")
            
            Action {
                text: qsTr("&Preferences...")
                onTriggered: mainWindow.onSettings()
            }
        }
    }

    SplitView {
        anchors.fill: parent
        anchors.margins: 10
        orientation: Qt.Horizontal

        // Left Panel - App List
        AppListPanel {
            id: appListPanel
            SplitView.preferredWidth: 600
            SplitView.minimumWidth: 300
        }

        // Right Panel - Logs
        LogPanel {
            id: logPanel
            SplitView.preferredWidth: 400
            SplitView.minimumWidth: 300
        }
    }

    Component.onCompleted: {
        console.log("Main window loaded")
        if (mainWindow) {
            mainWindow.windowReady()
        }
    }
}
