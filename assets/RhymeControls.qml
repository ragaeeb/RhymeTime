import bb.cascades 1.0
import ilmtest.rhymetime 1.0

Container
{
    property alias list: listView
    
    attachedObjects: [
        QTimer {
            id: timer
            
            onTimeout: {
                textField.requestFocus()
            }
        }
    ]
    
    TextField {
        id: textField
        objectName: "textField"
        hintText: qsTr("Enter a word...")
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Center

        animations: [
            TranslateTransition {
                id: translate
                fromX: -1280
                duration: 1000
                
                onEnded: {
                    textField.requestFocus()
                }
            }
        ]

        onCreationCompleted:
        {
            if ( app.getValueFor("animations") == 1 ){
                translate.play()
            } else {
                timer.singleShot = true;
                timer.interval = 100;
                timer.start();
            }
        }

        onTextChanging: {
            cover.word = text
            var rhymes = app.getRhymesFor(text)
            dataModel.clear()
            
            if (rhymes.length > 0) {
                dataModel.append(rhymes)
            }
        }
    }

    ListView {
        property variant application: app
        id: listView
        
        dataModel: ArrayDataModel {
            objectName: "dataModel"
            id: dataModel
        }

        listItemComponents: [
            ListItemComponent {
                StandardListItem {
                    id: rootItem
                    title: ListItemData
                    
                    contextActions: [
                        ActionSet {
                            title: ListItemData
                            
                            ActionItem {
                                id: copyAction
                                title: qsTr("Copy")
                                imageSource: "asset:///images/ic_copy.png"
                                
                                onTriggered: {
                                    rootItem.ListItem.view.application.copyWord(ListItemData)
                                }
                            }
                        }
                    ]
                }
            }
        ]

        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
    }
}