import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Vaulkeeper
import Vaulkeeper.Theme 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 800
    minimumWidth: 1024
    minimumHeight: 640
    title: qsTr("Vaulkeeper")
    color: Theme.surface

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            Layout.fillHeight: true
            currentPage: applicationVm.currentPage
            onPageRequested: applicationVm.currentPage = page
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            TopBar {
                Layout.fillWidth: true
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: indexForPage(applicationVm.currentPage)

                HealthPage {}
                ScanPage {}
                DuplicatesPage {}
                UninstallPage {}
                RulesPage {}
                QuarantinePage {}
                HealthReportPage {}
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 30
                color: Theme.surfaceAlt

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    spacing: 16

                    Label {
                        text: applicationVm.status
                        color: Theme.textSecondary
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    Label {
                        text: "Vaulkeeper " + appVersion
                        color: Theme.textSecondary
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    LanguagePicker {
        id: languagePicker
        anchors.centerIn: parent
    }

    Component.onCompleted: {
        if (applicationVm.firstRun) {
            languagePicker.open();
        }
    }

    function indexForPage(page) {
        var map = { "health": 0, "scan": 1, "duplicates": 2, "uninstall": 3, "rules": 4, "quarantine": 5, "report": 6 };
        return map[page] !== undefined ? map[page] : 0;
    }
}
