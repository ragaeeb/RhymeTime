import bb.cascades 1.0

BasePage
{
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
    
    contentContainer: RhymeControls
    {
        id: rhyme
        
        leftPadding: 20
        rightPadding: 20
        topPadding: 20
    }
}