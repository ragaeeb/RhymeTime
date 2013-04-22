import bb.cascades 1.0
import ilmtest.rhymetime 1.0

Container
{
    property alias list: listView
    
    TextField {

	    attachedObjects: [
	        QTimer {
	            id: timer
	            
	            onTimeout: {
	                textField.requestFocus()
	            }
	        }
	    ]
        
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
            if ( persist.getValueFor("animations") == 1 ) {
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
                divider.visible = true
            }
        }
    }
    
    Divider {
        id: divider
    	bottomMargin: 0
    	visible: false
    }

    ListView {
        id: listView
        
        dataModel: ArrayDataModel {
            objectName: "dataModel"
            id: dataModel
        }
        
        function copyWord(ListItemData) {
            persist.copyToClipboard(ListItemData)
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
                                    rootItem.ListItem.view.copyWord(ListItemData)
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