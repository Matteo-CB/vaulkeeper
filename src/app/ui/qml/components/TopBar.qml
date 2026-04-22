import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    implicitHeight: 64
    color: Theme.surface

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.surfaceAlt
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 16

        TextField {
            Layout.fillWidth: true
            Layout.maximumWidth: 520
            Layout.preferredHeight: 34
            placeholderText: qsTr("Search files, apps, commands")
            color: Theme.textPrimary
            leftPadding: 12
            rightPadding: 12
            background: Rectangle {
                color: Theme.surfaceAlt
                radius: 6
            }
        }

        Item { Layout.fillWidth: true }

        PrimaryButton {
            text: qsTr("Scan now")
            onClicked: scanVm.scanAll()
        }
    }
}
