import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    radius: 10
    color: Theme.surfaceAlt
    Layout.fillWidth: true
    implicitHeight: 104

    property string letter: ""
    property string label: ""
    property string total: ""
    property string free: ""
    property string used: ""
    property real freeRatio: 0.0

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        spacing: 16

        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            radius: 8
            color: Theme.surface
            Label {
                anchors.centerIn: parent
                text: root.letter
                color: Theme.textPrimary
                font.pixelSize: 17
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 6

            Label {
                text: root.label
                color: Theme.textPrimary
                font.pixelSize: 13
                font.bold: true
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Label {
                text: qsTr("%1 free of %2").arg(root.free).arg(root.total)
                color: Theme.textSecondary
                font.pixelSize: 11
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: 2
                implicitHeight: 5
                radius: 3
                color: Theme.surface
                Rectangle {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: parent.width * Math.max(0, Math.min(1, 1 - root.freeRatio))
                    radius: 3
                    color: root.freeRatio < 0.15 ? Theme.danger : Theme.accent
                }
            }
        }
    }
}
