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

        Label {
            text: qsTr("Duplicates")
            color: Theme.textPrimary
            font.pixelSize: 22
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            TextField {
                id: rootInput
                Layout.fillWidth: true
                placeholderText: qsTr("Folder to inspect")
                text: ""
                color: Theme.textPrimary
                background: Rectangle { color: Theme.surfaceAlt; radius: 6 }
            }

            PrimaryButton {
                text: qsTr("Find duplicates")
                onClicked: duplicatesVm.run(rootInput.text)
            }
        }

        Label {
            text: qsTr("Recoverable: %1 bytes").arg(duplicatesVm.recoverableBytes)
            color: Theme.textSecondary
            font.pixelSize: 12
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: duplicatesVm.clusters
            delegate: Rectangle {
                width: ListView.view.width
                height: 80
                color: Theme.surfaceAlt
                radius: 8
                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4
                    Label {
                        text: qsTr("Size %1 bytes, recoverable %2 bytes").arg(modelData.size).arg(modelData.recoverable)
                        color: Theme.textPrimary
                        font.pixelSize: 12
                    }
                    Label {
                        text: qsTr("Digest %1").arg(modelData.digest)
                        color: Theme.textSecondary
                        font.pixelSize: 11
                    }
                }
            }
        }
    }
}
