import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: scroll.availableWidth
            spacing: 18

            Item { Layout.preferredHeight: 28 }

            Label {
                text: qsTr("Health")
                color: Theme.textPrimary
                font.pixelSize: 24
                font.bold: true
                Layout.leftMargin: 28
                Layout.rightMargin: 28
                Layout.fillWidth: true
                elide: Label.ElideRight
            }

            Label {
                text: healthVm.summary
                color: Theme.textSecondary
                font.pixelSize: 13
                wrapMode: Label.WordWrap
                Layout.leftMargin: 28
                Layout.rightMargin: 28
                Layout.fillWidth: true
            }

            GridLayout {
                columns: scroll.availableWidth < 760 ? 1 : 2
                rowSpacing: 14
                columnSpacing: 14
                Layout.leftMargin: 28
                Layout.rightMargin: 28
                Layout.fillWidth: true

                Repeater {
                    model: healthVm.volumes
                    delegate: VolumeCard {
                        letter: modelData.letter
                        label: modelData.label
                        total: modelData.total
                        free: modelData.free
                        used: modelData.used
                        freeRatio: modelData.freeRatio
                    }
                }
            }

            Card {
                Layout.leftMargin: 28
                Layout.rightMargin: 28
                title: qsTr("Physical disks")
                subtitle: qsTr("SMART summary, wear and temperature")

                Repeater {
                    model: healthVm.disks
                    delegate: Label {
                        text: modelData.model + " (" + modelData.capacity + ")"
                        color: Theme.textSecondary
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                }

                Label {
                    visible: healthVm.disks.length === 0
                    text: qsTr("No SMART data yet")
                    color: Theme.textSecondary
                    font.pixelSize: 12
                    Layout.fillWidth: true
                }
            }

            Item { Layout.preferredHeight: 28 }
        }
    }
}
