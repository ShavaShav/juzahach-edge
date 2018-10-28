import QtQuick 2.11
import QtQuick.Window 2.11
import QtPositioning 5.8
import QtLocation 5.9
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

//library name comes from main.cpp
import gpslocation.backend 1.9

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("GPS Location")

    function setConnectionLabelText() {
        if(backEnd.connectionStringStatus === -1) {
            connectionLabel.text =
                qsTr("Setup Instructions") + "\n- " +
                qsTr("Go to the website") + "\n- " +
                qsTr("Create an account / Login") + "\n- " +
                qsTr("Click add new device") + "\n- " +
                qsTr("Enter the string that appears on this device")
        }
        else if(backEnd.connectionStringStatus === 0) {
            connectionLabel.text =
                qsTr("Device Not Registered") + "\n" +
                qsTr("Check the following:") + "\n- " +
                qsTr("Internet Connection") + "\n- " +
                qsTr("Server Status") + "\n- " +
                qsTr("Connectiong String: ") + backEnd.connectionString
        }
        else if(backEnd.connectionStringStatus === 1) {
            connectionLabel.text =
                qsTr("Device Registered") + "\n- " +
                qsTr("Connection String: ") + backEnd.connectionString
        }
    }

    BackEnd {
        id: backEnd
        onConnectionStringChanged: {
            setConnectionLabelText()
        }

        onGpsLocationDataSent: {
            if(backEnd.sendGpsDataStatus) {
                console.log("\n[SUCCESS] Sent location data to server")
            }
            else {
                console.log("\n[FAILURE] Could not send location data to server")
            }
        }
    }

    //activate Geolocation and update the map / mapitem with current location
    PositionSource {
        active: true
        onPreferredPositioningMethodsChanged: {

        }

        onPositionChanged: {
            //display current location or an error if not found
            if((position.coordinate + "") == "") {
                currentLocationLabel.text = qsTr("Location Unavailable")
            }
            else {
                currentLocationLabel.text = qsTr("Timestamp:\n") + position.timestamp + "\n" + qsTr("Coordinates:\n") + position.coordinate

                //send data to C++ where it will:
                //a) send to the server if theres an internet connection
                //b) be stored in a database and wait until theres an interent connection
                //EPSG:4326 standard states that coordinate order should be latitude, longitude
                backEnd.gpsLocation = position.coordinate.latitude + " " + position.coordinate.longitude + "|" + position.timestamp
            }

            setConnectionLabelText()
        }
    }

    ColumnLayout {
        height: window.height
        anchors.top: parent.top
        width: window.width

        Column {
            Layout.fillHeight: true
            Layout.fillWidth: true
            padding: 25

            //screen flashes when the keyboard is displayed, qt framework bug
            //need to focus the textfield to correct the dialogs location
            Button {
                padding: 25
                onClicked: {
                    dialog.visible = true
                    textField.focus = true
                }

                text: qsTr("Connect Device")
                font.weight: Font.Bold
                font.pixelSize: 22
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.left: parent.left
                anchors.leftMargin: 25
                Layout.alignment: Qt.AlignTop
            }

            //try and load the connection string on app start
            Label {
                id: connectionLabel
                topPadding: 25
                font.weight: Font.Bold
                font.pixelSize: 22
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.left: parent.left
                anchors.leftMargin: 25
            }

            Label {
                topPadding: 25
                text: qsTr("Current Location: ")
                font.weight: Font.Bold
                font.pixelSize: 22
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.left: parent.left
                anchors.leftMargin: 25
            }

            //will hold the current location and its timestamp
            Label {
                id: currentLocationLabel
                text: ""
                fontSizeMode: Text.HorizontalFit
                wrapMode: Label.WordWrap
                font.weight: Font.Bold
                font.pixelSize: 22
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.left: parent.left
                anchors.leftMargin: 25
            }
        }
    }

    //will open when the "Connect Device" button is clicked
    Dialog {
        id: dialog
        visible: false
        title: qsTr("Connect Device")
        height: 225
        width: window.width

        contentItem:
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "White"

                ColumnLayout {
                    width: dialog.width
                    height: dialog.height

                    Column {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Label {
                            topPadding: 25
                            width: dialog.width
                            font.weight: Font.Bold
                            font.pixelSize: 22
                            wrapMode: Label.WordWrap
                            text: qsTr("Enter the Server key to connect the device")
                            color: "Black"

                            anchors.right: parent.right
                            anchors.rightMargin: 25
                            anchors.left: parent.left
                            anchors.leftMargin: 25
                        }

                        TextField {
                            font.weight: Font.Bold
                            font.pixelSize: 22
                            width: dialog.width
                            id: textField
                            placeholderText: qsTr("Code")

                            anchors.right: parent.right
                            anchors.rightMargin: 25
                            anchors.left: parent.left
                            anchors.leftMargin: 25
                        }

                        DialogButtonBox {
                            topPadding: 25
                            bottomPadding: 25
                            position: DialogButtonBox.Footer
                            standardButtons: DialogButtonBox.Cancel | DialogButtonBox.Ok
                            font.weight: Font.Bold
                            font.pixelSize: 22

                            anchors.right: parent.right
                            anchors.rightMargin: 25
                            anchors.left: parent.left
                            anchors.leftMargin: 25

                            //will send the textbox string to C++ code
                            onAccepted: {
                                backEnd.connectionString = textField.text
                                textField.text = ""
                                dialog.visible = false
                            }
                            onRejected: {
                                textField.text = ""
                                dialog.visible = false
                            }
                        }
                    }
                }
            }
    }
}