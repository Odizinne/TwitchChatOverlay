import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import Odizinne.TwitchChatOverlay

ApplicationWindow {
    id: mainWindow
    visible: false
    title: "Twitch Chat Overlay"
    width: Screen.width
    height: Screen.height
    flags: Qt.WindowStaysOnTopHint |
           Qt.FramelessWindowHint

    //visibility: Window.FullScreen
    color: "transparent"

    Universal.theme: Universal.Dark
    Universal.accent: Universal.Purple

    Component.onCompleted: {
        if (UserSettings.channelName !== "" && UserSettings.token !== "") {
            TwitchChatClient.connectToChannel(UserSettings.channelName, UserSettings.token)
        }
    }

    function showOverlay() {
        visible = true
        showAnimation.start()
    }

    // Hide overlay function
    function hideOverlay() {
        hideAnimation.start()
    }

    // Show animation
    PropertyAnimation {
        id: showAnimation
        target: mainWindow
        property: "opacity"
        from: 0.0
        to: 1.0
        duration: 300
        easing.type: Easing.OutQuad
    }

    // Hide animation
    PropertyAnimation {
        id: hideAnimation
        target: mainWindow
        property: "opacity"
        from: 1.0
        to: 0.0
        duration: 300
        easing.type: Easing.InQuad

        onFinished: {
            mainWindow.visible = false
        }
    }

    Connections {
        target: ShortcutManager
        function onToggleOverlay() {
            if (mainWindow.visible) {
                hideOverlay()
            } else {
                showOverlay()
            }
        }
    }

    header: ToolBar {
        height: 40
        RowLayout {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Settings"
                Layout.preferredHeight: 40
                font.pixelSize: 20
                onClicked: settingsDialog.open()
            }

            Button {
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40
                text: "×"
                font.pixelSize: 20
                onClicked: Qt.quit()
            }
        }

        Label {
            text: "Twitch chat overlay"
            font.pixelSize: 20
            anchors.centerIn: parent
            font.bold: true
        }
    }

    Rectangle {
        color: Universal.background
        anchors.fill: parent
        opacity: 0.7
    }

    // Chat Window using CustomWindow
    CustomWindow {
        id: chatWindow
        width: 400
        height: 600
        x: 0
        y: 0
        visible: true // Chat window is always visible
        closeEnabled: false
        Component.onCompleted: showWindow()

        title: TwitchChatClient.connected ?
               "#" + TwitchChatClient.currentChannel + " Chat" :
               "Twitch Chat - Not Connected"

        // Resizing properties
        property bool resizing: false
        property string resizeMode: ""
        property point resizeStart
        property real initialWidth
        property real initialHeight

        content: ScrollView {
            anchors.fill: parent
            anchors.margins: 10

            ListView {
                id: chatView
                model: chatModel
                spacing: 2

                delegate: Rectangle {
                    width: chatView.width
                    height: messageText.implicitHeight + 10
                    color: "transparent"

                    Text {
                        id: messageText
                        anchors.fill: parent
                        anchors.margins: 5
                        text: "<font color='" + model.color + "'><b>" + model.username + ":</b></font> " + model.message
                        color: "white"
                        wrapMode: Text.WordWrap
                        textFormat: Text.RichText
                    }
                }

                onCountChanged: {
                    if (count > 100) {
                        chatModel.remove(0, count - 100)
                    }
                    positionViewAtEnd()
                }
            }
        }

        // Right border resize area
        MouseArea {
            anchors.top: parent.children[0].bottom // Below title bar
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: -4
            width: 8
            cursorShape: Qt.SizeHorCursor

            onPressed: function(mouse) {
                chatWindow.resizing = true
                chatWindow.resizeMode = "right"
                chatWindow.resizeStart = mapToItem(chatWindow, mouse.x, mouse.y)
                chatWindow.initialWidth = chatWindow.width
            }

            onPositionChanged: function(mouse) {
                if (chatWindow.resizing && chatWindow.resizeMode === "right") {
                    var currentPos = mapToItem(chatWindow, mouse.x, mouse.y)
                    var deltaX = currentPos.x - chatWindow.resizeStart.x
                    var newWidth = Math.max(250, chatWindow.initialWidth + deltaX)
                    newWidth = Math.min(newWidth, mainWindow.width - chatWindow.x)
                    chatWindow.width = newWidth
                }
            }

            onReleased: {
                chatWindow.resizing = false
                chatWindow.resizeMode = ""
            }
        }

        // Bottom border resize area
        MouseArea {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -4
            height: 8
            cursorShape: Qt.SizeVerCursor

            onPressed: function(mouse) {
                chatWindow.resizing = true
                chatWindow.resizeMode = "bottom"
                chatWindow.resizeStart = mapToItem(chatWindow, mouse.x, mouse.y)
                chatWindow.initialHeight = chatWindow.height
            }

            onPositionChanged: function(mouse) {
                if (chatWindow.resizing && chatWindow.resizeMode === "bottom") {
                    var currentPos = mapToItem(chatWindow, mouse.x, mouse.y)
                    var deltaY = currentPos.y - chatWindow.resizeStart.y
                    var newHeight = Math.max(200, chatWindow.initialHeight + deltaY)
                    newHeight = Math.min(newHeight, mainWindow.height - chatWindow.y)
                    chatWindow.height = newHeight
                }
            }

            onReleased: {
                chatWindow.resizing = false
                chatWindow.resizeMode = ""
            }
        }

        // Bottom-right corner resize area
        MouseArea {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: -4
            anchors.bottomMargin: -4
            width: 12
            height: 12
            cursorShape: Qt.SizeFDiagCursor

            onPressed: function(mouse) {
                chatWindow.resizing = true
                chatWindow.resizeMode = "bottomRight"
                chatWindow.resizeStart = mapToItem(chatWindow, mouse.x, mouse.y)
                chatWindow.initialWidth = chatWindow.width
                chatWindow.initialHeight = chatWindow.height
            }

            onPositionChanged: function(mouse) {
                if (chatWindow.resizing && chatWindow.resizeMode === "bottomRight") {
                    var currentPos = mapToItem(chatWindow, mouse.x, mouse.y)
                    var deltaX = currentPos.x - chatWindow.resizeStart.x
                    var deltaY = currentPos.y - chatWindow.resizeStart.y

                    var newWidth = Math.max(250, chatWindow.initialWidth + deltaX)
                    var newHeight = Math.max(200, chatWindow.initialHeight + deltaY)

                    newWidth = Math.min(newWidth, mainWindow.width - chatWindow.x)
                    newHeight = Math.min(newHeight, mainWindow.height - chatWindow.y)

                    chatWindow.width = newWidth
                    chatWindow.height = newHeight
                }
            }

            onReleased: {
                chatWindow.resizing = false
                chatWindow.resizeMode = ""
            }
        }
    }

    ListModel {
        id: chatModel
    }

    SettingsDialog {
        id: settingsDialog
        parent: mainWindow.contentItem
    }

    Connections {
        target: TwitchChatClient

        function onMessageReceived(username, message, color) {
            chatModel.append({
                "username": username,
                "message": message,
                "color": color
            })
        }

        function onConnectionError(error) {
            chatModel.append({
                "username": "System",
                "message": "Connection error: " + error,
                "color": "#FF4444"
            })
        }
    }
}
