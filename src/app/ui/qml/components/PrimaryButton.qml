import QtQuick
import QtQuick.Controls
import Vaulkeeper.Theme 1.0

Button {
    id: control
    padding: 0
    leftPadding: 18
    rightPadding: 18
    implicitHeight: 36
    font.pixelSize: 13
    font.bold: true

    contentItem: Text {
        text: control.text
        color: Theme.surface
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font: control.font
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 6
        color: control.pressed
               ? Qt.darker(Theme.accent, 1.1)
               : (control.hovered ? Qt.lighter(Theme.accent, 1.1) : Theme.accent)
    }
}
