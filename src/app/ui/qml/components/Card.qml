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

    Layout.fillWidth: true
    implicitHeight: inner.implicitHeight + 40

    ColumnLayout {
        id: inner
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 20
        spacing: 10

        Label {
            visible: root.title.length > 0
            text: root.title
            color: Theme.textPrimary
            font.pixelSize: 14
            font.bold: true
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        Label {
            visible: root.subtitle.length > 0
            text: root.subtitle
            color: Theme.textSecondary
            font.pixelSize: 12
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
        }

        ColumnLayout {
            id: contentColumn
            Layout.fillWidth: true
            Layout.topMargin: root.title.length > 0 || root.subtitle.length > 0 ? 4 : 0
            spacing: 8
        }

        Item {
            Layout.fillWidth: true
            implicitHeight: 20
        }
    }
}
