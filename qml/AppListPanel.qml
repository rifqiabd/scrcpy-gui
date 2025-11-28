import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: appListPanel

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // Title
        Label {
            text: qsTr("Android Apps")
            font.pointSize: 16
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        // Filter buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Item { Layout.fillWidth: true } // Spacer
            
            RadioButton {
                id: allAppsRadio
                text: qsTr("All Apps")
                checked: true
                onClicked: mainWindow.onFilterChanged(false)
            }
            
            RadioButton {
                id: runningOnlyRadio
                text: qsTr("Running Only")
                onClicked: mainWindow.onFilterChanged(true)
            }
            
            Item { Layout.fillWidth: true } // Spacer
        }

        // App List
        ListView {
            id: appListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            model: appListModel  // Set from C++
            
            delegate: ItemDelegate {
                width: appListView.width
                height: 40
                text: model.name
                
                onClicked: {
                    appListView.currentIndex = index
                    mainWindow.onAppSelected(model.packageName, model.name)
                }
                
                background: Rectangle {
                    color: {
                        if (parent.down) return palette.midlight
                        if (parent.hovered) return palette.light
                        if (index % 2 === 0) return palette.alternateBase
                        return palette.base
                    }
                }
            }
            
            ScrollBar.vertical: ScrollBar {}
        }

        // Button Row
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: qsTr("Refresh")
                Layout.fillWidth: true
                onClicked: mainWindow.onRefreshClicked()
            }
            
            Button {
                text: qsTr("Manual Add")
                Layout.fillWidth: true
                onClicked: mainWindow.onManualAddClicked()
            }
            
            Button {
                text: qsTr("Mirror Device")
                Layout.fillWidth: true
                ToolTip.text: qsTr("Launch full device mirror")
                ToolTip.visible: hovered
                onClicked: mainWindow.onMirrorDeviceClicked()
            }
        }

        // Status Label
        Label {
            id: statusLabel
            text: mainWindow ? mainWindow.statusText : qsTr("Ready")
            color: "gray"
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            padding: 5
        }
    }
}
