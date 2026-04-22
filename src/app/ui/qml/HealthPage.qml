import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0
import Vaulkeeper

Item {
    ScrollView {
        anchors.fill: parent
        padding: 24

        ColumnLayout {
            width: parent.width - 48
            spacing: 16

            Label {
                text: qsTr("Health")
                color: Theme.textPrimary
                font.pixelSize: 22
                font.bold: true
            }

            Label {
                text: healthVm.summary
                color: Theme.textSecondary
                font.pixelSize: 13
            }

            GridLayout {
                columns: 2
                rowSpacing: 12
                columnSpacing: 12
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
                title: qsTr("Physical disks")
                subtitle: qsTr("SMART summary, wear and temperature")

                Repeater {
                    model: healthVm.disks
                    delegate: Label {
                        text: modelData.model + " (" + modelData.capacity + ")"
                        color: Theme.textSecondary
                        font.pixelSize: 12
                    }
                }

                Label {
                    visible: healthVm.disks.length === 0
                    text: qsTr("No SMART data yet")
                    color: Theme.textSecondary
                    font.pixelSize: 12
                }
            }
        }
    }
}
