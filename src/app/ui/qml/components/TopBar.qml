import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    implicitHeight: 56
    color: Theme.surface

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.surfaceAlt
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        TextField {
            Layout.fillWidth: true
            Layout.maximumWidth: 520
            placeholderText: qsTr("Search files, apps, commands")
            color: Theme.textPrimary
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
