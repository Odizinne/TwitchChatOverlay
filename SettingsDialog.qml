import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import Odizinne.TwitchChatOverlay

CustomWindow {
    id: settingsWindow
    width: 500
    height: 400
    title: "Twitch Chat Settings"

    content: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        GroupBox {
            title: "Connection"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                Text {
                    text: "Get your OAuth token from:"
                    color: "white"
                }

                Text {
                    text: "https://twitchtokengenerator.com/"
                    color: "#6441a4"
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: Qt.openUrlExternally("https://twitchtokengenerator.com/")
                    }
                }

                Text {
                    text: "Required scope: chat:read"
                    color: "#AAAAAA"
                    font.italic: true
                }

                TextField {
                    id: tokenField
                    Layout.fillWidth: true
                    placeholderText: "Paste your OAuth token here (oauth:xxxxx)"
                    echoMode: TextInput.Password
                    text: UserSettings.token
                    onTextChanged: UserSettings.token = text
                }

                TextField {
                    id: channelField
                    Layout.fillWidth: true
                    placeholderText: "Channel name (without #)"
                    text: UserSettings.channelName
                    onTextChanged: UserSettings.channelName = text
                }
            }
        }

        GroupBox {
            title: "Status"
            Layout.fillWidth: true

            Text {
                text: TwitchChatClient.connected ?
                      "✓ Connected to #" + TwitchChatClient.currentChannel :
                      "✗ Not connected"
                color: TwitchChatClient.connected ? "#00FF00" : "#FF4444"
                font.bold: true
            }
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: TwitchChatClient.connected ? "Disconnect" : "Connect"
                enabled: !TwitchChatClient.connected ?
                        (tokenField.text.length > 0 && channelField.text.length > 0) :
                        true

                onClicked: {
                    if (TwitchChatClient.connected) {
                        TwitchChatClient.disconnect()
                    } else {
                        TwitchChatClient.connectToChannel(channelField.text, tokenField.text)
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Close"
                onClicked: settingsWindow.close()
            }
        }
    }
}
