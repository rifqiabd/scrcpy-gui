import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: logPanel

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // Title
        Label {
            text: qsTr("Scrcpy Logs")
            font.pointSize: 16
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        // Status Label
        Label {
            id: scrcpyStatusLabel
            text: mainWindow ? mainWindow.scrcpyStatusText : qsTr("No scrcpy running")
            color: mainWindow ? mainWindow.scrcpyStatusColor : "gray"
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            padding: 5
        }

        // Log Text Area
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            TextArea {
                id: logTextArea
                readOnly: true
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap
                font.family: "Consolas,Monaco,monospace"
                font.pointSize: 9
                color: "#d4d4d4"
                
                background: Rectangle {
                    color: "#1e1e1e"
                    border.color: "#3e3e3e"
                    border.width: 1
                }
                
                text: mainWindow ? mainWindow.logText : ""
            }
        }

        // Control Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: qsTr("Clear Logs")
                Layout.fillWidth: true
                onClicked: mainWindow.onClearLogsClicked()
            }
            
            Button {
                id: stopScrcpyButton
                text: qsTr("Stop Scrcpy")
                Layout.fillWidth: true
                enabled: mainWindow ? mainWindow.scrcpyRunning : false
                onClicked: mainWindow.onStopScrcpyClicked()
            }
        }
    }
}
