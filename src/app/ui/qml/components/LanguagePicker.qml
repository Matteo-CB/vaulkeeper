import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vaulkeeper.Theme 1.0

Popup {
    id: root
    width: 440
    height: 420
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    padding: 0
    background: Rectangle {
        color: Theme.surfaceAlt
        radius: 12
        border.color: Theme.surface
    }

    property string selectedTag: applicationVm.localeTag

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: qsTr("Choose your language")
            color: Theme.textPrimary
            font.pixelSize: 20
            font.bold: true
        }

        Label {
            text: qsTr("Language")
            color: Theme.textSecondary
            font.pixelSize: 11
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: applicationVm.availableLocales
            clip: true
            spacing: 6
            delegate: Rectangle {
                width: ListView.view.width
                height: 48
                radius: 8
                color: root.selectedTag === modelData.tag ? Theme.accent : Theme.surface
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    spacing: 12
                    Label {
                        text: modelData.native
                        color: root.selectedTag === modelData.tag ? Theme.surface : Theme.textPrimary
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Label {
                        text: modelData.english
                        color: root.selectedTag === modelData.tag ? Theme.surface : Theme.textSecondary
                        font.pixelSize: 12
                    }
                    Item { Layout.fillWidth: true }
                    Label {
                        visible: root.selectedTag === modelData.tag
                        text: "\u2713"
                        color: Theme.surface
                        font.pixelSize: 16
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.selectedTag = modelData.tag
                }
            }
        }

        PrimaryButton {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Continue")
            onClicked: {
                applicationVm.localeTag = root.selectedTag;
                applicationVm.commitFirstRunComplete();
                root.close();
            }
        }
    }
}
