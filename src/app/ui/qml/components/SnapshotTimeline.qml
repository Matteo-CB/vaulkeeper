import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    property var snapshots: []
    property var deltas: []

    color: Theme.surfaceAlt
    radius: 8
    implicitHeight: 220

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: qsTr("Snapshot timeline")
            color: Theme.textPrimary
            font.pixelSize: 14
            font.bold: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: ListView.Horizontal
            model: root.snapshots
            spacing: 12
            clip: true
            delegate: Rectangle {
                width: 140
                height: 120
                radius: 6
                color: Theme.surface
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 4
                    Label {
                        text: modelData.label
                        color: Theme.textPrimary
                        font.pixelSize: 12
                    }
                    Label {
                        text: modelData.when
                        color: Theme.textSecondary
                        font.pixelSize: 10
                    }
                    Item { Layout.fillHeight: true }
                    Label {
                        text: modelData.delta
                        color: modelData.delta && modelData.delta.indexOf("-") === 0 ? Theme.success : Theme.danger
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }
    }
}
