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
                text: qsTr("Scan")
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            PrimaryButton {
                text: scanVm.running ? qsTr("Cancel") : qsTr("Scan all drives")
                onClicked: scanVm.running ? scanVm.cancel() : scanVm.scanAll()
            }
        }

        ScanProgressBar {
            Layout.fillWidth: true
            progress: scanVm.progress
            currentPath: scanVm.currentPath
            running: scanVm.running
        }

        Card {
            title: qsTr("Summary")
            subtitle: qsTr("Volumes visited during the last scan")

            Label {
                text: qsTr("Files indexed: %1").arg(scanVm.filesIndexed)
                color: Theme.textSecondary
                font.pixelSize: 12
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Label {
                text: qsTr("Bytes indexed: %1").arg(scanVm.bytesIndexed)
                color: Theme.textSecondary
                font.pixelSize: 12
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
        }

        Treemap {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: scanVm.volumeSummaries
        }
    }
}
