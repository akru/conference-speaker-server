import QtQuick 1.1

/* A toggle button component. */
Rectangle  {
    id: toggleButton

    signal selected()
    signal pushed()

    color: "transparent"
    state: "unPressed"
    onStateChanged:  {
        if (state == "pressed")  {
            selected()
        }
    }
    border.width: 2
    border.color: "gray"
    radius: 2; smooth: true
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#424242" }
        GradientStop { position: 1.0; color: "black" }
    }

    MouseArea  {
        id: mouseArea

        anchors.fill: parent
        onPressed:  {
            if (parent.state == "unPressed")  {
                pushed()
                parent.state = "pressed"
            }
            else
                parent.state = "unPressed"
        }
    }

    states: [
        State  {
            name: "pressed"
            PropertyChanges  {
                target: toggleButton
                scale: 0.95
                radius: 25
            }
        },
        State  {
            name: "unPressed"
            PropertyChanges  {
                target: toggleButton
                scale: 1/0.95
                radius: 2
            }
        }
    ]

    transitions: [
        Transition  {
            from: "unPressed"
            to: "pressed"
            reversible: true
            PropertyAnimation  {
                target: toggleButton
                properties: "scale"
                duration: 100
            }
        }
    ]
}
