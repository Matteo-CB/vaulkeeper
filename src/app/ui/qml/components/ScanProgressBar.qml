import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    property real progress: 0.0
    property string currentPath: ""
    property bool running: false

    radius: 10
    color: Theme.surfaceAlt
    implicitHeight: 56
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        spacing: 16

        Rectangle {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: 6
            radius: 3
            color: Theme.surface
            Rectangle {
                width: parent.width * Math.min(1.0, Math.max(0.0, root.progress))
                height: parent.height
                radius: 3
                color: Theme.accent
                Behavior on width { NumberAnimation { duration: 150 } }
            }
        }

        Label {
            Layout.preferredWidth: 280
            Layout.alignment: Qt.AlignVCenter
            elide: Label.ElideMiddle
            text: root.running ? root.currentPath : qsTr("Idle")
            color: Theme.textSecondary
            font.pixelSize: 11
            font.family: "JetBrains Mono"
        }
    }
}
