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
                text: qsTr("Quarantine")
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Label {
                text: qsTr("Stored: %1 bytes").arg(quarantineVm.storedBytes)
                color: Theme.textSecondary
                font.pixelSize: 12
                Layout.alignment: Qt.AlignVCenter
            }
            Button {
                text: qsTr("Purge expired")
                onClicked: quarantineVm.purgeExpired()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: quarantineVm.items
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
                            text: modelData.original
                            color: Theme.textPrimary
                            font.pixelSize: 12
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Label.ElideMiddle
                        }
                        Label {
                            text: modelData.reason
                            color: Theme.textSecondary
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                    }
                    Label {
                        text: modelData.bytes + " B"
                        color: Theme.textSecondary
                        font.pixelSize: 11
                        Layout.alignment: Qt.AlignVCenter
                    }
                    Button {
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Restore")
                        onClicked: quarantineVm.restore(modelData.id)
                    }
                    Button {
                        Layout.alignment: Qt.AlignVCenter
                        text: qsTr("Discard")
                        onClicked: quarantineVm.discard(modelData.id)
                    }
                }
            }
        }
    }
}
