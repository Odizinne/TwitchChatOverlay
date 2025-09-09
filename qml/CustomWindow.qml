import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

Rectangle {
    id: customWindow

    property alias title: titleText.text
    property alias content: contentArea.children
    property bool dragging: false
    property point dragStart
    property bool closeEnabled: true

    visible: false
    color: "#CC000000"
    border.color: "#444444"
    border.width: 1
    property color titleBarColor: "#55666666"

    // Animation properties
    scale: 0
    opacity: 0

    Universal.theme: Universal.Dark
    Universal.accent: Universal.Indigo

    function showWindow() {
        visible = true
        showAnimation.start()
    }

    function hideWindow() {
        hideAnimation.start()
    }

    function open() {
        showWindow()
        // Center in parent
        //if (parent) {
        //    x = (parent.width - width) / 2
        //    y = (parent.height - height) / 2
        //}
    }

    function close() {
        hideWindow()
    }

    // Show animation
    ParallelAnimation {
        id: showAnimation

        PropertyAnimation {
            target: customWindow
            property: "scale"
            from: 0.8
            to: 1.0
            duration: 200
            easing.type: Easing.OutBack
        }

        PropertyAnimation {
            target: customWindow
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

    // Hide animation
    ParallelAnimation {
        id: hideAnimation

        PropertyAnimation {
            target: customWindow
            property: "scale"
            from: 1.0
            to: 0.8
            duration: 150
            easing.type: Easing.InBack
        }

        PropertyAnimation {
            target: customWindow
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 150
            easing.type: Easing.InQuad
        }

        onFinished: {
            customWindow.visible = false
        }
    }

    // Title bar
    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 35
        color: customWindow.titleBarColor
        anchors.margins: 1

        MouseArea {
            id: titleBarDrag
            anchors.fill: parent
            anchors.rightMargin: 40 // Leave space for close button

            onPressed: function(mouse) {
                customWindow.dragging = true
                customWindow.dragStart = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: function(mouse) {
                if (customWindow.dragging) {
                    var newX = customWindow.x + mouse.x - customWindow.dragStart.x
                    var newY = customWindow.y + mouse.y - customWindow.dragStart.y

                    // Keep within parent bounds
                    if (customWindow.parent) {
                        newX = Math.max(0, Math.min(newX, customWindow.parent.width - customWindow.width))
                        newY = Math.max(0, Math.min(newY, customWindow.parent.height - customWindow.height))
                    }

                    customWindow.x = newX
                    customWindow.y = newY
                }
            }

            onReleased: {
                customWindow.dragging = false
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8

            Text {
                id: titleText
                Layout.fillWidth: true
                text: "Custom Window"
                color: "white"
                font.bold: true
                font.pixelSize: 12
            }

            Button {
                Layout.preferredHeight: 24
                Layout.preferredWidth: 24
                icon.source: "qrc:/icons/cross.svg"
                font.pixelSize: 12
                visible: customWindow.closeEnabled
                onClicked: customWindow.close()
            }
        }
    }

    // Content area
    Rectangle {
        id: contentArea
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 1
        color: "transparent"
    }
}
