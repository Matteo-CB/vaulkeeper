import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    color: Theme.surfaceAlt
    radius: 8
    property var columns: []
    property var rows: []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 0

        Row {
            Layout.fillWidth: true
            spacing: 12
            Repeater {
                model: root.columns
                delegate: Label {
                    width: 140
                    text: modelData.label !== undefined ? modelData.label : modelData
                    color: Theme.textSecondary
                    font.pixelSize: 11
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.surface
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.rows
            clip: true
            delegate: Row {
                width: ListView.view.width
                spacing: 12
                property var rowData: modelData
                Repeater {
                    model: root.columns
                    delegate: Label {
                        property var column: modelData
                        width: 140
                        elide: Label.ElideRight
                        text: {
                            var key = column.key !== undefined ? column.key : column;
                            return parent.rowData && parent.rowData[key] !== undefined ? parent.rowData[key] : "";
                        }
                        color: Theme.textPrimary
                        font.pixelSize: 12
                    }
                }
            }
        }
    }
}
