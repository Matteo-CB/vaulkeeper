import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    property real progress: 0.0
    property string currentPath: ""
    property bool running: false

    radius: 8
    color: Theme.surfaceAlt
    implicitHeight: 48
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Rectangle {
            Layout.fillWidth: true
            height: 4
            radius: 2
            color: Theme.surface
            Rectangle {
                width: parent.width * Math.min(1.0, Math.max(0.0, root.progress))
                height: parent.height
                radius: 2
                color: Theme.accent
                Behavior on width { NumberAnimation { duration: 150 } }
            }
        }

        Label {
            Layout.preferredWidth: 240
            elide: Label.ElideMiddle
            text: root.running ? root.currentPath : qsTr("Idle")
            color: Theme.textSecondary
            font.pixelSize: 11
        }
    }
}
