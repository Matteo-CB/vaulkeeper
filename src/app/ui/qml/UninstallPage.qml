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

        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Label {
                text: qsTr("Uninstall")
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
                elide: Label.ElideRight
            }

            Label {
                text: qsTr("%1 apps, %2 total").arg(uninstallVm.applications.length).arg(uninstallVm.totalSize)
                color: Theme.textSecondary
                font.pixelSize: 12
                Layout.alignment: Qt.AlignVCenter
            }

            Button {
                text: qsTr("Refresh")
                onClicked: uninstallVm.refresh()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: qsTr("Sort by")
                color: Theme.textSecondary
                font.pixelSize: 12
                Layout.alignment: Qt.AlignVCenter
            }

            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 180
                Layout.preferredHeight: 34
                model: [
                    { key: "size", label: qsTr("Size") },
                    { key: "name", label: qsTr("Name") },
                    { key: "publisher", label: qsTr("Publisher") },
                    { key: "installedAt", label: qsTr("Install date") },
                    { key: "kind", label: qsTr("Kind") }
                ]
                textRole: "label"
                valueRole: "key"
                currentIndex: {
                    for (var i = 0; i < model.length; ++i) {
                        if (model[i].key === uninstallVm.sortKey) { return i; }
                    }
                    return 0;
                }
                onActivated: uninstallVm.sortKey = model[currentIndex].key
            }

            Button {
                text: uninstallVm.sortDescending ? qsTr("Descending") : qsTr("Ascending")
                onClicked: uninstallVm.sortDescending = !uninstallVm.sortDescending
            }

            Item { Layout.fillWidth: true }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: uninstallVm.applications
            clip: true
            spacing: 8
            delegate: Rectangle {
                width: ListView.view.width
                height: 82
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
                            text: {
                                var parts = [];
                                if (modelData.publisher && modelData.publisher.length > 0) { parts.push(modelData.publisher); }
                                if (modelData.version && modelData.version.length > 0) { parts.push(modelData.version); }
                                if (modelData.kind && modelData.kind.length > 0) { parts.push(modelData.kind); }
                                if (modelData.installedAt && modelData.installedAt.length > 0) { parts.push(modelData.installedAt); }
                                return parts.join("  ");
                            }
                            color: Theme.textSecondary
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                    }

                    Label {
                        text: modelData.size
                        color: Theme.textPrimary
                        font.pixelSize: 13
                        font.family: "JetBrains Mono"
                        Layout.alignment: Qt.AlignVCenter
                        horizontalAlignment: Text.AlignRight
                        Layout.preferredWidth: 96
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
            text: qsTr("No applications detected yet. Click Refresh.")
            color: Theme.textSecondary
            font.pixelSize: 12
            Layout.fillWidth: true
        }
    }
}
