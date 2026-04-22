import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Item {
    id: root
    property var model: []

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceAlt
        radius: 10
    }

    Label {
        anchors.centerIn: parent
        visible: !root.model || root.model.length === 0
        text: qsTr("No scan yet. Click 'Scan all drives' to start.")
        color: Theme.textSecondary
        font.pixelSize: 13
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: Math.max(1, Math.min(4, Math.floor(width / 220)))
        columnSpacing: 8
        rowSpacing: 8
        visible: root.model && root.model.length > 0

        Repeater {
            model: root.model
            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 120
                Layout.minimumHeight: 100
                radius: 8
                color: Qt.hsla((index * 0.13 + 0.55) % 1.0, 0.35, 0.42, 1.0)

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    Label {
                        text: modelData.name !== undefined ? modelData.name : ""
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                    Label {
                        text: modelData.bytes !== undefined ? modelData.bytes : ""
                        color: "white"
                        opacity: 0.85
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                    Item { Layout.fillHeight: true }
                    Label {
                        text: {
                            var files = modelData.files !== undefined ? modelData.files : 0;
                            var ratio = modelData.ratio !== undefined ? modelData.ratio : 0;
                            return qsTr("%1 files, %2%").arg(files).arg(Math.round(ratio * 100));
                        }
                        color: "white"
                        opacity: 0.75
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                }
            }
        }
    }
}
