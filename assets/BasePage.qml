import bb.cascades 1.0

Page {
    property alias contentContainer: contentContainer.controls

    Container {
		Container {
		    id: titleBar
		    layout: DockLayout {}
		
		    horizontalAlignment: HorizontalAlignment.Fill
		    verticalAlignment: VerticalAlignment.Top
		    
		    background: Color.create("#3b4b62")
		
		    ImageView {
		        imageSource: "asset:///images/title_bg.png"
		        topMargin: 0
		        leftMargin: 0
		        rightMargin: 0
		        bottomMargin: 0
		
		        horizontalAlignment: HorizontalAlignment.Fill
		        verticalAlignment: VerticalAlignment.Fill
		        
		        animations: [
		            TranslateTransition {
		                id: translate
		                toY: 0
		                fromY: -100
		                duration: 1000
		            }
		        ]
		        
		        onCreationCompleted:
		        {
		            if ( persist.getValueFor("animations") == 1 ) {
		                translate.play()
		            }
		        }
		    }
		
		    ImageView {
		        imageSource: "asset:///images/logo.png"
		        topMargin: 0
		        leftMargin: 0
		        rightMargin: 0
		        bottomMargin: 0
		
		        horizontalAlignment: HorizontalAlignment.Center
		        verticalAlignment: VerticalAlignment.Center
		
		        animations: [
		            ParallelAnimation {
		                id: fadeTranslate
		                
			            FadeTransition {
			                duration: 1000
			                easingCurve: StockCurve.CubicIn
			                fromOpacity: 0
			                toOpacity: 1
			            }

			            TranslateTransition {
			                toY: 0
			                fromY: -100
			                duration: 1000
			            }
                    }
		        ]
		        
		        onCreationCompleted:
		        {
		            if ( persist.getValueFor("animations") == 1 ) {
		                fadeTranslate.play()
		            }
		        }
		    }
		}

        Container // This container is replaced
        {
            layout: DockLayout {
                
            }
            
            id: contentContainer
            objectName: "contentContainer"
            background: titleBar.background
            
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }

            ImageView {
                imageSource: "asset:///images/bottomDropShadow.png"
                topMargin: 0
                leftMargin: 0
                rightMargin: 0
                bottomMargin: 0

                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
            }
        }
    }
}