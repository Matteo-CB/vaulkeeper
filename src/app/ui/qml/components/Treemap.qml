import QtQuick
import QtQuick.Controls
import Vaulkeeper.Theme 1.0

Item {
    id: root
    property var model: []
    property real totalBytes: 0

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceAlt
        radius: 8
        border.color: Theme.surface
    }

    Grid {
        anchors.fill: parent
        anchors.margins: 6
        columns: 4
        spacing: 6

        Repeater {
            model: root.model
            delegate: Rectangle {
                width: (parent.width - (3 * 6)) / 4
                height: 80 + 60 * (modelData.ratio !== undefined ? modelData.ratio : 0)
                radius: 6
                color: Qt.hsla((index * 0.13) % 1.0, 0.4, 0.45, 1.0)

                Label {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: 8
                    text: modelData.name
                    color: "white"
                    font.pixelSize: 11
                }

                Label {
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 8
                    text: Math.round((modelData.ratio !== undefined ? modelData.ratio : 0) * 100) + "%"
                    color: "white"
                    font.pixelSize: 10
                }
            }
        }
    }
}
