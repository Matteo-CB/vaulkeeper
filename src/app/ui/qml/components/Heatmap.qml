import QtQuick
import Vaulkeeper.Theme 1.0

Canvas {
    id: root
    property var data: []
    property real cellSize: 10
    property real cellSpacing: 2

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        var total = 7 * 53;
        var cols = 53;
        for (var i = 0; i < total; ++i) {
            var col = Math.floor(i / 7);
            var row = i % 7;
            var value = (i < data.length) ? data[i] : 0;
            var intensity = Math.max(0, Math.min(1, value));
            ctx.fillStyle = Qt.rgba(0.36, 0.62, 1.0, 0.15 + 0.85 * intensity);
            var x = col * (cellSize + cellSpacing);
            var y = row * (cellSize + cellSpacing);
            ctx.fillRect(x, y, cellSize, cellSize);
        }
    }

    onDataChanged: requestPaint()
    onCellSizeChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
}
