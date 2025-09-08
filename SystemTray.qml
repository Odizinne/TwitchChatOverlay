import QtQuick
import Qt.labs.platform as Platform

Platform.SystemTrayIcon {
    id: systemTray
    visible: true
    icon.source: "qrc:/icons/icon.png"

    signal toggleOverlay()

    menu: Platform.Menu {
        Platform.MenuItem {
            text: qsTr("Toggle Overlay")
            onTriggered: {
                systemTray.toggleOverlay()
            }
        }

        Platform.MenuSeparator {}

        Platform.MenuItem {
            text: qsTr("Exit TwitchChatOverlay")
            onTriggered: Qt.quit()
        }
    }
}
