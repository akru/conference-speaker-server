import QtQuick 1.1

Rectangle {
    width: 360
    height: 360
    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("QUIT");
            Qt.quit();
        }

        Image {
            id: micImage
            x: 130
            y: 130
            width: 100
            height: 100
            source: "qrc:/images/microphone.png"
        }
    }
}
