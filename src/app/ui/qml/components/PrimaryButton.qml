import QtQuick
import QtQuick.Controls
import Vaulkeeper.Theme 1.0

Button {
    id: control
    padding: 0
    leftPadding: 16
    rightPadding: 16
    implicitHeight: 34
    font.pixelSize: 13

    contentItem: Text {
        text: control.text
        color: Theme.surface
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font: control.font
    }

    background: Rectangle {
        radius: 6
        color: control.hovered ? Qt.lighter(Theme.accent, 1.1) : Theme.accent
    }
}
