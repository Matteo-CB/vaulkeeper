import QtQuick
import Vaulkeeper.Theme 1.0

Canvas {
    id: root
    property var model: []

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();
        ctx.clearRect(0, 0, width, height);
        var cx = width / 2;
        var cy = height / 2;
        var rMax = Math.min(width, height) / 2 - 8;
        var start = -Math.PI / 2;
        var total = 0;
        for (var i = 0; i < model.length; ++i) { total += model[i].ratio || 0; }
        if (total <= 0) { return; }

        for (var i = 0; i < model.length; ++i) {
            var arc = (model[i].ratio / total) * Math.PI * 2;
            ctx.beginPath();
            ctx.moveTo(cx, cy);
            ctx.arc(cx, cy, rMax, start, start + arc);
            ctx.closePath();
            ctx.fillStyle = Qt.hsla((i * 0.137) % 1.0, 0.4, 0.5, 1.0);
            ctx.fill();
            start += arc;
        }
    }

    onModelChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
}
