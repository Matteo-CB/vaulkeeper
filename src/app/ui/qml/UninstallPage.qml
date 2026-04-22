import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    Component.onCompleted: uninstallVm.refresh()

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 28
        anchors.rightMargin: 28
        anchors.topMargin: 28
        anchors.bottomMargin: 28
        spacing: 18

        Label {
            text: qsTr("Uninstall")
            color: Theme.textPrimary
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: uninstallVm.applications
            clip: true
            spacing: 8
            delegate: Rectangle {
                width: ListView.view.width
                height: 72
                color: Theme.surfaceAlt
                radius: 8
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.topMargin: 12
                    anchors.bottomMargin: 12
                    spacing: 16

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 4
                        Label {
                            text: modelData.name
                            color: Theme.textPrimary
                            font.pixelSize: 13
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                        Label {
                            text: qsTr("%1 %2").arg(modelData.publisher).arg(modelData.version)
                            color: Theme.textSecondary
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                    }

                    Button {
                        text: qsTr("Uninstall")
                        Layout.alignment: Qt.AlignVCenter
                        onClicked: uninstallVm.uninstall(modelData.id, true, true)
                    }
                }
            }
        }

        Label {
            visible: uninstallVm.applications.length === 0
            text: qsTr("No applications detected yet")
            color: Theme.textSecondary
            font.pixelSize: 12
            Layout.fillWidth: true
        }
    }
}
