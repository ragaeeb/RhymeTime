import bb.cascades 1.0

NavigationPane
{
    id: navigationPane

    property int progressValue: 0
    property bool showProgress: true

    attachedObjects: [
        ComponentDefinition {
            id: definition
        }
    ]

    Menu.definition: MenuDefinition
    {
        settingsAction: SettingsActionItem
        {
            property Page settingsPage
            
            id: settingsAction
            
            onTriggered:
            {
                if (!settingsPage) {
                    definition.source = "SettingsPage.qml"
                    settingsPage = definition.createObject()
                }
                
                navigationPane.push(settingsPage);
            }
        }
        
        actions: [
            ActionItem {
                title: qsTr("New Page")
                imageSource: "asset:///images/newPage.png"
                enabled: !showProgress && navigationPane.top != settingsAction.settingsPage && navigationPane.top != helpAction.helpPage
                
                onTriggered:
                {
                    definition.source = "RhymePage.qml"
                    var page = definition.createObject()
                    navigationPane.push(page);
                }
            }
        ]

        helpAction: HelpActionItem
        {
            property Page helpPage
            
            id: helpAction
            
            onTriggered:
            {
                definition.source = "HelpPage.qml"
                
                if (!helpPage) {
                    helpPage = definition.createObject();
                }

                navigationPane.push(helpPage);
            }
        }
    }

    onPopTransitionEnded: {
        page.destroy();
    }
    
    BasePage {

        actions: [
            ActionItem {
                title: qsTr("Jump To Top")
                imageSource: "file:///usr/share/icons/ic_go.png"

                onTriggered: {
                    rhyme.list.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Default)
                }

                ActionBar.placement: ActionBarPlacement.OnBar
            },

            ActionItem {
                title: qsTr("Jump To Bottom")
                imageSource: "asset:///images/scrollEnd.png"

                onTriggered: {
                    rhyme.list.scrollToPosition(ScrollPosition.End, ScrollAnimation.Default)
                }
                
                ActionBar.placement: ActionBarPlacement.OnBar
            }
        ]

        contentContainer: Container {
            leftPadding: 20
            rightPadding: 20
            topPadding: 20

            ProgressIndicator {
                horizontalAlignment: HorizontalAlignment.Fill
                fromValue: 0
                toValue: 110023 // known # of lines in the database file
                value: progressValue
                visible: showProgress
            }

            RhymeControls {
            	id: rhyme
            }
        }
    }
}