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
            Label { text: qsTr("Quarantine"); color: Theme.textPrimary; font.pixelSize: 22; font.bold: true }
            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("Stored: %1 bytes").arg(quarantineVm.storedBytes)
                color: Theme.textSecondary
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
                        Label { text: modelData.original; color: Theme.textPrimary; font.pixelSize: 12; elide: Label.ElideMiddle }
                        Label { text: modelData.reason; color: Theme.textSecondary; font.pixelSize: 11 }
                    }
                    Label { text: modelData.bytes + " B"; color: Theme.textSecondary; font.pixelSize: 11 }
                    Button {
                        text: qsTr("Restore")
                        onClicked: quarantineVm.restore(modelData.id)
                    }
                    Button {
                        text: qsTr("Discard")
                        onClicked: quarantineVm.discard(modelData.id)
                    }
                }
            }
        }
    }
}
