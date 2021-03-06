import QtQuick 2.11
import QtQuick.Window 2.11
import QtPositioning 5.8
import QtLocation 5.9
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

//library name comes from main.cpp
import gpslocation.backend 2.1

//Colours:
//Invalid: #f40606
//Valid: #13f406

Window {
    id: window
    visible: true
    width: 450
    height: 600
    title: qsTr("Juzahach Edge")

    BackEnd {
        id: backEnd

        onServerAccessCodeChanged: {
            if(backEnd.serverAccessCodeStatus) {
                accessCodeFlag.color = "#13f406"
            }
            else {
                accessCodeFlag.color = "#f40606"
            }
        }

        onNetworkConnectionStatusChanged: {
            if(backEnd.networkConnectionStatus) {
                networkConnectionFlag.color = "#13f406"
            }
            else {
                networkConnectionFlag.color = "#f40606"
            }
        }

        onLocationDataSent: {
            if(backEnd.locationDataSentStatus) {
                console.log("\n[SUCCESS] Sent location data to server")
                gpsLocationFlag.color = "#13f406"
            }
            else {
                console.log("\n[FAILURE] Could not send location data to server, stored in local database")
                gpsLocationFlag.color = "#f40606"
            }
        }
    }

    //activate Geolocation and attempt to get the current location
    PositionSource {
        id: positionSource
        updateInterval: 6000 // 6 seconds
        onPositionChanged: {
            //display current location components
            if((position.coordinate + "") == "") {
                //no location, show an error to the user
                latitudeValue.text = "Unavailable"
                longitudeValue.text = "Unavailable"
                timestampValue.text = "Unavailable"
            }
            else {
                if(checkBoxTrackLocation.checkState == 2) {
                    //send the data to the backend [C++]
                    //EPSG:4326 standard states that coordinate order should be latitude, longitude

                    backEnd.locationData = position.coordinate.latitude + " " + position.coordinate.longitude + "|" + position.timestamp

                    //update and show the new coordinates to the user
                    latitudeValue.text = position.coordinate.latitude
                    longitudeValue.text = position.coordinate.longitude
                    timestampValue.text = position.timestamp
                }
            }
        }
    }

    ColumnLayout {
        anchors.rightMargin: 25
        anchors.leftMargin: 25
        anchors.bottomMargin: 25
        anchors.topMargin: 25
        anchors.fill: parent
        spacing: 2

        Column {
            id: column
            width: 200
            height: 400
            spacing: 10
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            TextField {
                font.weight: Font.Bold
                font.pixelSize: 22
                width: window.width
                id: textField
                placeholderText: qsTr("Code")

                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true
            }

            Button {
                id: buttonConnectDevice
                padding: 25
                onClicked: {
                    backEnd.serverAccessCode = textField.text
                    textField.text = ""
                }

                text: qsTr("Register Device")
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true
                font.weight: Font.Bold
                font.pixelSize: 22
            }
        }

        Column {
            height: 400
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillHeight: false
            Layout.fillWidth: true

            CheckBox {
                id: checkBoxTrackLocation
                text: qsTr("Send GPS Data")
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                font.pixelSize: 22
                display: AbstractButton.TextBesideIcon
                checkState: {
                    //Qt::CheckState == 0: is unchecked
                    //Qt::CheckState == 1: is partial
                    //Qt::CheckState == 2: check box is checked
                    checkBoxTrackLocation.checkState = backEnd.checkboxStatus

                    if(checkBoxTrackLocation.checkState == 0) {
                        positionSource.active = false
                        gpsLocationFlag.color = "#f40606"

                        console.log("Location Tracking is turned off")
                    }
                    else if(checkBoxTrackLocation.checkState == 2) {
                        positionSource.active = true
                        console.log("Location Tracking is turned on")
                    }
                }
                onCheckStateChanged: {
                    //Qt::CheckState == 0: is unchecked
                    //Qt::CheckState == 1: is partial
                    //Qt::CheckState == 2: check box is checked
                    backEnd.checkboxStatus = checkBoxTrackLocation.checkState

                    //turn location tracking off or on depending on the checkbox status
                    if(checkBoxTrackLocation.checkState == 0) {
                        positionSource.active = false
                        gpsLocationFlag.color = "#f40606"

                        console.log("Location Tracking is turned off")
                    }
                    else if(checkBoxTrackLocation.checkState == 2) {
                        positionSource.active = true
                        console.log("Location Tracking is turned on")
                    }
                }
            }

            Text {
                text: "No location data will be sent or stored when this checkbox is unselected"
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                wrapMode: Text.WordWrap
                bottomPadding: 0
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                font.pixelSize: 18
                font.bold: false
                verticalAlignment: Text.AlignTop
            }
        }



        Column {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillWidth: true
            spacing: -10

            Text {
                text: qsTr("Status")
                bottomPadding: 5
                font.bold: true
                verticalAlignment: Text.AlignTop
                font.pixelSize: 22
            }

            ColumnLayout {
                height: 50
                RowLayout {
                    width: 100
                    height: 100
                    Layout.fillHeight: true
                    Rectangle {
                        id: accessCodeFlag
                        width: 100
                        height: 25
                        color: {
                            if(backEnd.serverAccessCodeStatus) {
                                accessCodeFlag.color = "#13f406"
                            }
                            else {
                                accessCodeFlag.color = "#f40606"
                            }
                        }
                        Layout.fillHeight: false
                        Layout.fillWidth: false
                    }

                    Text {
                        text: qsTr("Authenticated")
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }
                    spacing: 5
                    Layout.fillWidth: true
                }
                spacing: 5
            }

            ColumnLayout {
                height: 50
                RowLayout {
                    width: 100
                    height: 100
                    Layout.fillHeight: true
                    Rectangle {
                        id: networkConnectionFlag
                        width: 100
                        height: 25
                        color: {
                            if(backEnd.networkConnectionStatus) {
                                networkConnectionFlag.color = "#13f406"
                            }
                            else {
                                networkConnectionFlag.color = "#f40606"
                            }
                        }
                        Layout.fillHeight: false
                        Layout.fillWidth: false
                    }

                    Text {
                        text: qsTr("Cloud Connection")
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }
                    spacing: 5
                    Layout.fillWidth: true
                }
                spacing: 5
            }

            ColumnLayout {
                height: 50
                spacing: 5

                RowLayout {
                    width: 100
                    height: 100
                    spacing: 5
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Rectangle {
                        id: gpsLocationFlag
                        width: 100
                        height: 25
                        color: "#ffffff"
                        Layout.fillWidth: false
                        Layout.fillHeight: false
                    }

                    Text {
                        text: qsTr("Sending Location")
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                        font.pixelSize: 22
                    }
                }

            }

        }

        Column {
            id: column1
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillWidth: true

            Text {
                text: qsTr("GPS Coordinates")
                bottomPadding: 10
                font.pixelSize: 22
                font.bold: true
                verticalAlignment: Text.AlignTop
            }

            ColumnLayout {
                height: 50
                RowLayout {
                    width: 100
                    height: 100
                    Layout.fillHeight: true

                    Text {
                        text: qsTr("Latitude:")
                        rightPadding: 31
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }

                    Text {
                        id: latitudeValue
                        text: qsTr("Value")
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }
                    spacing: 5
                    Layout.fillWidth: true
                }
                spacing: 5
            }

            ColumnLayout {
                height: 50

                RowLayout {
                    width: 100
                    height: 100
                    Layout.fillHeight: true

                    Text {
                        text: qsTr("Longitude:")
                        rightPadding: 11
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }

                    Text {
                        id: longitudeValue
                        text: qsTr("Value")
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }
                    spacing: 5
                    Layout.fillWidth: true
                }
                spacing: 5
            }

            ColumnLayout {
                height: 50
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0

                RowLayout {
                    width: 100
                    height: 100
                    Layout.fillHeight: true

                    Text {
                        text: qsTr("Timestamp:")
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: false
                        verticalAlignment: Text.AlignVCenter
                    }

                    Text {
                        id: timestampValue
                        text: qsTr("Value")
                        wrapMode: Text.WordWrap
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 22
                        Layout.fillWidth: true
                        verticalAlignment: Text.AlignVCenter
                    }
                    spacing: 5
                    Layout.fillWidth: true
                }
                spacing: 5
            }
            spacing: -15
        }

    }
}

/*##^## Designer {
    D{i:33;anchors_height:50}
}
 ##^##*/
