import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import Odizinne.TwitchChatOverlay

CustomWindow {
    id: settingsWindow
    width: 500
    height: mainLyt.implicitHeight + 30 + 35 + 2
    title: "Twitch Chat Settings"

    content: ColumnLayout {
        id: mainLyt
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        RowLayout {
            Label {
                text: "Get your OAuth token from:"
            }

            Label {
                text: "https://twitchtokengenerator.com/"
                color: Universal.color(Universal.Indigo)
                font.underline: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: Qt.openUrlExternally("https://twitchtokengenerator.com/")
                }
            }
        }

        Label {
            text: "Required scope: chat:read"
            font.italic: true
            opacity: 0.7
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


        Label {
            text: TwitchChatClient.connected ?
                      "✓ Connected to #" + TwitchChatClient.currentChannel :
                      "✗ Not connected"
            color: TwitchChatClient.connected ? "#00FF00" : "#FF4444"
            font.bold: true
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
        }

        RowLayout {
            Label {
                text: "Chat font size"
                Layout.fillWidth: true
            }

            SpinBox {
                from: 10
                to: 20
                value: UserSettings.chatTextSize
                onValueChanged: UserSettings.chatTextSize = value
            }
        }
    }
}
