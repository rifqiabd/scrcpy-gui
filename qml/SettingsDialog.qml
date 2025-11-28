import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: settingsDialog
    title: qsTr("Settings")
    width: 600
    height: 500
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    onAccepted: {
        if (settingsManager) {
            settingsManager.saveAllSettings()
        }
    }

    TabBar {
        id: tabBar
        width: parent.width
        
        TabButton { text: qsTr("General") }
        TabButton { text: qsTr("Video") }
        TabButton { text: qsTr("Audio") }
        TabButton { text: qsTr("Window") }
        TabButton { text: qsTr("Advanced") }
    }

    StackLayout {
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        currentIndex: tabBar.currentIndex

        // General Tab
        ScrollView {
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                CheckBox {
                    text: qsTr("Always on top")
                    checked: settingsManager ? settingsManager.alwaysOnTop : false
                    onCheckedChanged: if (settingsManager) settingsManager.alwaysOnTop = checked
                }
                
                CheckBox {
                    text: qsTr("No control (display only)")
                    checked: settingsManager ? settingsManager.noControl : false
                    onCheckedChanged: if (settingsManager) settingsManager.noControl = checked
                }
                
                CheckBox {
                    text: qsTr("Stay awake")
                    checked: settingsManager ? settingsManager.stayAwake : false
                    onCheckedChanged: if (settingsManager) settingsManager.stayAwake = checked
                }
                
                CheckBox {
                    text: qsTr("Turn screen off")
                    checked: settingsManager ? settingsManager.turnScreenOff : false
                    onCheckedChanged: if (settingsManager) settingsManager.turnScreenOff = checked
                }
                
                CheckBox {
                    text: qsTr("No virtual display destroy content")
                    checked: settingsManager ? settingsManager.noVdDestroyContent : false
                    onCheckedChanged: if (settingsManager) settingsManager.noVdDestroyContent = checked
                }
                
                CheckBox {
                    text: qsTr("Show touches")
                    checked: settingsManager ? settingsManager.showTouches : false
                    onCheckedChanged: if (settingsManager) settingsManager.showTouches = checked
                }
                
                CheckBox {
                    text: qsTr("Disable screensaver")
                    checked: settingsManager ? settingsManager.disableScreensaver : false
                    onCheckedChanged: if (settingsManager) settingsManager.disableScreensaver = checked
                }
                
                Item { Layout.fillHeight: true }
            }
        }

        // Video Tab
        ScrollView {
            clip: true
            
            GridLayout {
                width: parent.width
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                
                Label { text: qsTr("Max size:") }
                SpinBox {
                    from: 0
                    to: 4096
                    value: settingsManager ? settingsManager.maxSize : 0
                    onValueChanged: if (settingsManager) settingsManager.maxSize = value
                }
                
                Label { text: qsTr("Bit rate:") }
                ComboBox {
                    model: ["Default (8M)", "4M", "8M", "16M", "32M"]
                    currentIndex: {
                        if (!settingsManager) return 0
                        const val = settingsManager.bitRate
                        const idx = model.indexOf(val)
                        return idx >= 0 ? idx : 0
                    }
                    onCurrentTextChanged: if (settingsManager) settingsManager.bitRate = currentText
                }
                
                Label { text: qsTr("Max FPS:") }
                SpinBox {
                    from: 0
                    to: 144
                    value: settingsManager ? settingsManager.maxFps : 0
                    onValueChanged: if (settingsManager) settingsManager.maxFps = value
                }
                
                Label { text: qsTr("Video codec:") }
                ComboBox {
                    model: ["Default (h264)", "h264", "h265", "av1"]
                    currentIndex: {
                        if (!settingsManager) return 0
                        const val = settingsManager.videoCodec
                        const idx = model.indexOf(val)
                        return idx >= 0 ? idx : 0
                    }
                    onCurrentTextChanged: if (settingsManager) settingsManager.videoCodec = currentText
                }
                
                Label { text: qsTr("Rotation:") }
                ComboBox {
                    model: ["0°", "90°", "180°", "270°"]
                    currentIndex: settingsManager ? (settingsManager.rotation / 90) : 0
                    onCurrentIndexChanged: if (settingsManager) settingsManager.rotation = currentIndex * 90
                }
                
                CheckBox {
                    Layout.columnSpan: 2
                    text: qsTr("No mipmaps")
                    checked: settingsManager ? settingsManager.noMipmaps : false
                    onCheckedChanged: if (settingsManager) settingsManager.noMipmaps = checked
                }
                
                Item { Layout.fillHeight: true; Layout.columnSpan: 2 }
            }
        }

        // Audio Tab
        ScrollView {
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                CheckBox {
                    text: qsTr("No audio")
                    checked: settingsManager ? settingsManager.noAudio : false
                    onCheckedChanged: if (settingsManager) settingsManager.noAudio = checked
                }
                
                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 10
                    rowSpacing: 10
                    
                    Label { text: qsTr("Audio codec:") }
                    ComboBox {
                        model: ["Default (opus)", "opus", "aac", "flac", "raw"]
                        currentIndex: {
                            if (!settingsManager) return 0
                            const val = settingsManager.audioCodec
                            const idx = model.indexOf(val)
                            return idx >= 0 ? idx : 0
                        }
                        onCurrentTextChanged: if (settingsManager) settingsManager.audioCodec = currentText
                    }
                    
                    Label { text: qsTr("Audio bit rate:") }
                    ComboBox {
                        model: ["Default (128K)", "64K", "96K", "128K", "192K", "256K"]
                        currentIndex: {
                            if (!settingsManager) return 0
                            const val = settingsManager.audioBitRate
                            const idx = model.indexOf(val)
                            return idx >= 0 ? idx : 0
                        }
                        onCurrentTextChanged: if (settingsManager) settingsManager.audioBitRate = currentText
                    }
                }
                
                Item { Layout.fillHeight: true }
            }
        }

        // Window Tab
        ScrollView {
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                CheckBox {
                    text: qsTr("Fullscreen")
                    checked: settingsManager ? settingsManager.fullscreen : false
                    onCheckedChanged: if (settingsManager) settingsManager.fullscreen = checked
                }
                
                CheckBox {
                    text: qsTr("Window borderless")
                    checked: settingsManager ? settingsManager.windowBorderless : false
                    onCheckedChanged: if (settingsManager) settingsManager.windowBorderless = checked
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    Label { text: qsTr("Window title:") }
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("Custom window title")
                        text: settingsManager ? settingsManager.windowTitle : ""
                        onTextChanged: if (settingsManager) settingsManager.windowTitle = text
                    }
                }
                
                Item { Layout.fillHeight: true }
            }
        }

        // Advanced Tab
        ScrollView {
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                Label {
                    text: qsTr("Custom Arguments")
                    font.bold: true
                }
                
                Label {
                    text: qsTr("Additional scrcpy command-line arguments (space-separated):")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                
                TextField {
                    Layout.fillWidth: true
                    placeholderText: qsTr("e.g., --serial 1234567890")
                    text: settingsManager ? settingsManager.customArgs : ""
                    onTextChanged: if (settingsManager) settingsManager.customArgs = text
                }
                
                Item { Layout.fillHeight: true }
            }
        }
    }
}
