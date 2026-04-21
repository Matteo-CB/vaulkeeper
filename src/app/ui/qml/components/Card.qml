import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    radius: 12
    color: Theme.surfaceAlt
    property string title: ""
    property string subtitle: ""
    default property alias contentItems: contentColumn.data

    implicitHeight: 160
    Layout.fillWidth: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 12

        Label {
            visible: root.title.length > 0
            text: root.title
            color: Theme.textPrimary
            font.pixelSize: 14
            font.bold: true
        }

        Label {
            visible: root.subtitle.length > 0
            text: root.subtitle
            color: Theme.textSecondary
            font.pixelSize: 12
        }

        ColumnLayout {
            id: contentColumn
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
        }
    }
}
