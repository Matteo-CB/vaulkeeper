import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Rules"); color: Theme.textPrimary; font.pixelSize: 22; font.bold: true }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("Reload")
                onClicked: rulesVm.reload()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: rulesVm.rules
            delegate: Rectangle {
                width: ListView.view.width
                height: 70
                color: Theme.surfaceAlt
                radius: 8
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.description; color: Theme.textPrimary; font.pixelSize: 13 }
                        Label { text: modelData.id; color: Theme.textSecondary; font.pixelSize: 11 }
                    }

                    Switch {
                        checked: modelData.enabled
                        text: qsTr("Enabled")
                    }

                    Button {
                        text: qsTr("Dry run")
                        onClicked: rulesVm.executeRule(modelData.id, true)
                    }
                }
            }
        }
    }
}
