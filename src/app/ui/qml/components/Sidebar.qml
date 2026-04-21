import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    implicitWidth: 240
    color: Theme.surfaceAlt
    property string currentPage: "health"
    signal pageRequested(string page)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 4

        Label {
            text: qsTr("Vaulkeeper")
            color: Theme.textPrimary
            font.pixelSize: 18
            font.bold: true
            Layout.bottomMargin: 16
        }

        Repeater {
            model: [
                { id: "health", label: qsTr("Health") },
                { id: "scan", label: qsTr("Scan") },
                { id: "duplicates", label: qsTr("Duplicates") },
                { id: "uninstall", label: qsTr("Uninstall") },
                { id: "rules", label: qsTr("Rules") },
                { id: "quarantine", label: qsTr("Quarantine") },
                { id: "report", label: qsTr("Audit") }
            ]

            delegate: Rectangle {
                Layout.fillWidth: true
                height: 36
                radius: 6
                color: root.currentPage === modelData.id ? Theme.surface : "transparent"

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData.label
                    color: root.currentPage === modelData.id ? Theme.textPrimary : Theme.textSecondary
                    font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.pageRequested(modelData.id)
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
