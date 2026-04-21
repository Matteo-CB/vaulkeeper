import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import "components"

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: qsTr("Audit report")
            color: Theme.textPrimary
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            text: qsTr("Export a PDF or single file HTML audit report.")
            color: Theme.textSecondary
            font.pixelSize: 12
        }

        RowLayout {
            spacing: 12
            Button { text: qsTr("Export PDF") }
            Button { text: qsTr("Export HTML") }
            Button { text: qsTr("Copy JSON") }
        }

        Item { Layout.fillHeight: true }
    }
}
