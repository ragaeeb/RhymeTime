#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QStringList>

#include "Persistance.h"
#include "WordDatabase.h"

namespace bb {
	namespace cascades {
		class Application;
	}
}

class QRunnable;

namespace rhymetime {

using namespace bb::cascades;
using namespace canadainc;

class ApplicationUI : public QObject
{
    Q_OBJECT

	Persistance m_persistance;
    WordDatabase db;
    QObject* m_root;
    bool m_loaded;

    ApplicationUI(Application* app);
    void startThread(QRunnable* qr);

private slots:
	void progress(int progress);
	void databaseLoaded();

public:
	static void create(Application* app);
    virtual ~ApplicationUI();

    Q_INVOKABLE QStringList getRhymesFor(QString const& word);
};

} // rhymetime

#endif /* ApplicationUI_HPP_ */
