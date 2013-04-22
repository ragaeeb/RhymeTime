import bb.cascades 1.0

BasePage
{
    contentContainer: Container
    {
        leftPadding: 20
        topPadding: 20
        rightPadding: 20
        bottomPadding: 20
        
        SettingPair {
            title: qsTr("Animations")
        	toggle.checked: persist.getValueFor("animations") == 1
    
            toggle.onCheckedChanged: {
        		persist.saveValueFor("animations", checked ? 1 : 0)
        		
        		if (checked) {
        		    infoText.text = qsTr("Controls will be animated whenever they are loaded.")
        		} else {
        		    infoText.text = qsTr("Controls will be snapped into position without animations.")
        		}
            }
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
        }
        
        Label {
            id: infoText
            multiline: true
            textStyle.fontSize: FontSize.XXSmall
            textStyle.textAlign: TextAlign.Center
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
}