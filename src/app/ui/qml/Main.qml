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
    minimumWidth: 960
    minimumHeight: 600
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
                implicitHeight: 28
                color: Theme.surfaceAlt
                Label {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 12
                    text: applicationVm.status
                    color: Theme.textSecondary
                    font.pixelSize: 12
                }
                Label {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 12
                    text: "Vaulkeeper " + appVersion
                    color: Theme.textSecondary
                    font.pixelSize: 12
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
