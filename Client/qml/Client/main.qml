import QtQuick 1.1

Rectangle {
    id: main
    width: 300
    height: 300
    color: "black"

    Text {
        id: noticeLabel
        height: 34
        color: "white"
        text: qsTr("Please mute while you are silent.")
        wrapMode: Text.NoWrap
        font.bold: true
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignHCenter
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        font.pointSize: 12
    }

    Rectangle {
        id: micImageContainer
        color: parent.color
        anchors.topMargin: noticeLabel.height
        anchors.bottomMargin: transmitButtonContainer.height
        anchors.fill:parent
        Image {
            id: micImage
            source: "qrc:images/microphone.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id: transmitButtonContainer
        color: parent.color
        height: 100
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        ToggleButton {
            id: transmitButton
            height: 50
            width: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            offImageSource: "qrc:images/microphone.png"
            onImageSource: "qrc:images/microphone.png"
            onPushed: {
                console.log("transmit button pushed")
            }
        }
    }
}
