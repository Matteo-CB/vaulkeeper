import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 28
        anchors.rightMargin: 28
        anchors.topMargin: 28
        anchors.bottomMargin: 28
        spacing: 18

        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Label {
                text: qsTr("Rules")
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Button {
                text: qsTr("Reload")
                onClicked: rulesVm.reload()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: rulesVm.rules
            clip: true
            spacing: 8
            delegate: Rectangle {
                width: ListView.view.width
                height: 76
                color: Theme.surfaceAlt
                radius: 8
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.topMargin: 12
                    anchors.bottomMargin: 12
                    spacing: 14

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 4
                        Label {
                            text: modelData.description
                            color: Theme.textPrimary
                            font.pixelSize: 13
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                        Label {
                            text: modelData.id
                            color: Theme.textSecondary
                            font.pixelSize: 11
                            font.family: "JetBrains Mono"
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                    }

                    Switch {
                        Layout.alignment: Qt.AlignVCenter
                        checked: modelData.enabled
                        text: qsTr("Enabled")
                    }

                    Button {
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Dry run")
                        onClicked: rulesVm.executeRule(modelData.id, true)
                    }
                }
            }
        }
    }
}
