#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QSettings>
#include <QStringList>

#include "WordDatabase.h"

namespace bb {
	namespace cascades {
		class Application;
		class ArrayDataModel;
	}
}

class QRunnable;

namespace rhymetime {

using namespace bb::cascades;

class ApplicationUI : public QObject
{
    Q_OBJECT

    QSettings m_settings;
    WordDatabase db;
    QObject* m_root;
    bool m_loaded;

    ApplicationUI(Application* app);
    void startThread(QRunnable* qr);

private slots:
	void progress(int const& progress);
	void databaseLoaded();
	void onAboutToQuit();

public:
	static void create(Application* app);
    virtual ~ApplicationUI() {}

    Q_INVOKABLE void copyWord(QString const& word);
    Q_INVOKABLE QStringList getRhymesFor(QString const& word);
    Q_INVOKABLE QVariant getValueFor(QString const& objectName);
    Q_INVOKABLE void saveValueFor(QString const& objectName, QVariant const& inputValue);
};

} // rhymetime

#endif /* ApplicationUI_HPP_ */
