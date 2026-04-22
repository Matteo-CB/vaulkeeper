import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 28
        anchors.rightMargin: 28
        anchors.topMargin: 28
        anchors.bottomMargin: 28
        spacing: 18

        Label {
            text: qsTr("Audit report")
            color: Theme.textPrimary
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        Label {
            text: qsTr("Export a PDF or single file HTML audit report.")
            color: Theme.textSecondary
            font.pixelSize: 13
            wrapMode: Label.WordWrap
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 12
            Layout.topMargin: 4
            PrimaryButton { text: qsTr("Export PDF") }
            Button { text: qsTr("Export HTML") }
            Button { text: qsTr("Copy JSON") }
        }

        Item { Layout.fillHeight: true }
    }
}
