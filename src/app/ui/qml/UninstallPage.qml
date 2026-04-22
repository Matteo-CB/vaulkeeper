import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    Component.onCompleted: uninstallVm.refresh()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: qsTr("Uninstall")
            color: Theme.textPrimary
            font.pixelSize: 22
            font.bold: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: uninstallVm.applications
            delegate: Rectangle {
                width: ListView.view.width
                height: 64
                color: Theme.surfaceAlt
                radius: 8
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 16

                    ColumnLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.name; color: Theme.textPrimary; font.pixelSize: 13 }
                        Label {
                            text: qsTr("%1 %2").arg(modelData.publisher).arg(modelData.version)
                            color: Theme.textSecondary
                            font.pixelSize: 11
                        }
                    }

                    Button {
                        text: qsTr("Uninstall")
                        onClicked: uninstallVm.uninstall(modelData.id, true, true)
                    }
                }
            }
        }

        Label {
            visible: uninstallVm.applications.length === 0
            text: qsTr("No applications detected yet")
            color: Theme.textSecondary
        }
    }
}
