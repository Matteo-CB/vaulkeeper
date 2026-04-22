import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Rectangle {
    id: root
    implicitWidth: 228
    color: Theme.surfaceAlt
    property string currentPage: "health"
    signal pageRequested(string page)

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        anchors.topMargin: 20
        anchors.bottomMargin: 16
        spacing: 4

        Label {
            text: qsTr("Vaulkeeper")
            color: Theme.textPrimary
            font.pixelSize: 18
            font.bold: true
            Layout.bottomMargin: 20
            Layout.fillWidth: true
            elide: Label.ElideRight
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
                Layout.preferredHeight: 38
                radius: 6
                color: root.currentPage === modelData.id
                       ? Theme.surface
                       : (hover.containsMouse ? Qt.rgba(1, 1, 1, 0.04) : "transparent")

                Label {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData.label
                    color: root.currentPage === modelData.id ? Theme.textPrimary : Theme.textSecondary
                    font.pixelSize: 13
                    font.bold: root.currentPage === modelData.id
                    elide: Label.ElideRight
                }

                MouseArea {
                    id: hover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.pageRequested(modelData.id)
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
