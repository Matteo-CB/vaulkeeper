import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Popup {
    id: root
    width: 520
    height: 360
    anchors.centerIn: parent
    modal: true
    focus: true
    padding: 0
    background: Rectangle {
        color: Theme.surfaceAlt
        radius: 10
        border.color: Theme.surface
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TextField {
            Layout.fillWidth: true
            placeholderText: qsTr("Type a command")
            padding: 14
            color: Theme.textPrimary
            background: Rectangle {
                color: Theme.surface
                radius: 10
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            delegate: Rectangle {
                width: ListView.view.width
                height: 34
                color: "transparent"
                Label {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 14
                    text: qsTr("No commands yet")
                    color: Theme.textSecondary
                }
            }
        }
    }
}
