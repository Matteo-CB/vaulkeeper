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
            text: qsTr("Duplicates")
            color: Theme.textPrimary
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            TextField {
                id: rootInput
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                placeholderText: qsTr("Folder to inspect")
                text: applicationVm.defaultDuplicatesRoot()
                color: Theme.textPrimary
                leftPadding: 12
                rightPadding: 12
                background: Rectangle { color: Theme.surfaceAlt; radius: 6 }
            }

            PrimaryButton {
                text: qsTr("Find duplicates")
                enabled: rootInput.text.length > 0
                onClicked: duplicatesVm.run(rootInput.text)
            }
        }

        Label {
            text: qsTr("Recoverable: %1 bytes").arg(duplicatesVm.recoverableBytes)
            color: Theme.textSecondary
            font.pixelSize: 12
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: duplicatesVm.clusters
            clip: true
            spacing: 8
            delegate: Rectangle {
                width: ListView.view.width
                height: 84
                color: Theme.surfaceAlt
                radius: 8
                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.topMargin: 14
                    anchors.bottomMargin: 14
                    spacing: 6
                    Label {
                        text: qsTr("Size %1 bytes, recoverable %2 bytes").arg(modelData.size).arg(modelData.recoverable)
                        color: Theme.textPrimary
                        font.pixelSize: 13
                        font.bold: true
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                    Label {
                        text: qsTr("Digest %1").arg(modelData.digest)
                        color: Theme.textSecondary
                        font.pixelSize: 11
                        font.family: "JetBrains Mono"
                        Layout.fillWidth: true
                        elide: Label.ElideMiddle
                    }
                }
            }
        }
    }
}
