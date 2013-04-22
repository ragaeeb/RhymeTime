import bb.cascades 1.0
import bb 1.0

BasePage
{
    attachedObjects: [
        ApplicationInfo {
            id: appInfo
        },

        PackageInfo {
            id: packageInfo
        }
    ]

    contentContainer: Container
    {
        leftPadding: 20; rightPadding: 20
        
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Fill

        ScrollView {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Fill

            Label {
                multiline: true
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                textStyle.textAlign: TextAlign.Center
                textStyle.fontSize: FontSize.Small
                textStyle.color: Color.White
                content.flags: TextContentFlag.ActiveText
                text: qsTr("\n\n(c) 2013 %1. All Rights Reserved.\n%2 %3\n\nPlease report all bugs to:\nsupport@canadainc.org\n\nOften times poets, and writers want to make their compositions rhyme. They want to introduce flow into their pieces. This app makes it easy to quickly find words that rhyme with another using efficient algorithms.\n\nUsing BB10's flow architecture, you can build much more precise multisyllabic rhymes by opening up multiple pages and finding additional rhymes to support previous words. To do this simply swipe-down from the top-bezel and choose 'New Page'.\n\n").arg(packageInfo.author).arg(appInfo.title).arg(appInfo.version)
            }
        }
    }
}
