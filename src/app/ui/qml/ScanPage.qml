import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import "components"

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Scan")
                color: Theme.textPrimary
                font.pixelSize: 22
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Button {
                text: scanVm.running ? qsTr("Cancel") : qsTr("Scan all drives")
                onClicked: scanVm.running ? scanVm.cancel() : scanVm.scanAll()
            }
        }

        ScanProgressBar {
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
            }
            Label {
                text: qsTr("Bytes indexed: %1").arg(scanVm.bytesIndexed)
                color: Theme.textSecondary
                font.pixelSize: 12
            }
        }

        Treemap {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: scanVm.volumeSummaries
        }
    }
}
