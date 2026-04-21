import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    radius: 10
    color: Theme.surfaceAlt
    implicitHeight: 90
    Layout.fillWidth: true

    property string letter: ""
    property string label: ""
    property string total: ""
    property string free: ""
    property string used: ""
    property real freeRatio: 0.0

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Rectangle {
            width: 40
            height: 40
            radius: 8
            color: Theme.surface
            Label {
                anchors.centerIn: parent
                text: root.letter
                color: Theme.textPrimary
                font.pixelSize: 16
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Label { text: root.label; color: Theme.textPrimary; font.pixelSize: 13 }
            Label {
                text: qsTr("%1 free of %2").arg(root.free).arg(root.total)
                color: Theme.textSecondary
                font.pixelSize: 11
            }
            Rectangle {
                Layout.fillWidth: true
                height: 4
                radius: 2
                color: Theme.surface
                Rectangle {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: parent.width * (1 - root.freeRatio)
                    radius: 2
                    color: root.freeRatio < 0.15 ? Theme.danger : Theme.accent
                }
            }
        }
    }
}
