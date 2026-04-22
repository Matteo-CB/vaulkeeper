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

        Label {
            text: qsTr("Audit report")
            color: Theme.textPrimary
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        Label {
            text: qsTr("Exports a snapshot of volumes, physical disks and SMART data.")
            color: Theme.textSecondary
            font.pixelSize: 13
            wrapMode: Label.WordWrap
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 12
            Layout.topMargin: 4

            PrimaryButton {
                text: qsTr("Export JSON")
                onClicked: applicationVm.promptAndExportAudit("json")
            }
            Button {
                text: qsTr("Export HTML")
                onClicked: applicationVm.promptAndExportAudit("html")
            }
            Button {
                text: qsTr("Export text")
                onClicked: applicationVm.promptAndExportAudit("text")
            }
            Button {
                text: qsTr("Refresh preview")
                onClicked: preview.text = applicationVm.buildAuditReport("json")
            }
        }

        Card {
            title: qsTr("Preview")
            subtitle: qsTr("JSON content of the audit")

            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 320
                clip: true

                TextArea {
                    id: preview
                    readOnly: true
                    wrapMode: TextArea.NoWrap
                    font.family: "JetBrains Mono"
                    font.pixelSize: 11
                    color: Theme.textSecondary
                    background: Rectangle { color: Theme.surface; radius: 6 }
                    text: applicationVm.buildAuditReport("json")
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
