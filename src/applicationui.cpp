#include "applicationui.hpp"
#include "Logger.h"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>

#include <QThreadPool>
#include <QTimer>

namespace rhymetime {

using namespace bb::cascades;
using namespace canadainc;

ApplicationUI::ApplicationUI(Application* app) :
		QObject(app), db("app/native/assets/dict.txt"), m_loaded(false)
{
	INIT_SETTING("animations", 1);

	qmlRegisterType<QTimer>("ilmtest.rhymetime", 1, 0, "QTimer");

	QmlDocument* qmlCover = QmlDocument::create("asset:///Cover.qml").parent(this);
	Control* sceneRoot = qmlCover->createRootObject<Control>();
	SceneCover* cover = SceneCover::create().content(sceneRoot);
	app->setCover(cover);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);
    qml->setContextProperty("cover", sceneRoot);
    qml->setContextProperty("persist", &m_persistance);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app->setScene(root);

    connect( &db, SIGNAL( progress(int) ), this, SLOT( progress(int) ) );
    connect( &db, SIGNAL( loadComplete() ), this, SLOT( databaseLoaded() ) );

    m_root = root;
    startThread(&db);
}


void ApplicationUI::create(Application* app) {
	new ApplicationUI(app);
}


ApplicationUI::~ApplicationUI() {
	db.close();
}


void ApplicationUI::progress(int progress) {
	m_root->setProperty("progressValue", progress);
}


void ApplicationUI::databaseLoaded()
{
	m_loaded = true;
	m_root->setProperty("showProgress", false);

	QString word = m_root->findChild<AbstractTextControl*>("textField")->text();

	if ( !word.isEmpty() )
	{
		QStringList rhymes = getRhymesFor(word);

		if ( !rhymes.isEmpty() )
		{
			ArrayDataModel* dataModel = m_root->findChild<ArrayDataModel*>("dataModel");
			dataModel->append(rhymes);
		}
	}
}


QStringList ApplicationUI::getRhymesFor(QString const& word)
{
	QMap<QString, QString> map;

	if (m_loaded)
	{
		const char* toRhyme = word.trimmed().toUtf8().constData();

		WordArray words;
		db.findRhymes(words, toRhyme, NULL);
		for( unsigned i = 0; i < words.size(); i++ ) {
			QString value = QString(words[i]->text).replace("_", " ");
			map.insert( value.toLower(), value );
		}
	}

	LOGGER( "getRhymesFor" << word << map.size() << map.values() );

	return map.values();
}


void ApplicationUI::startThread(QRunnable* qr)
{
	qr->setAutoDelete(false);

	QThreadPool *threadPool = QThreadPool::globalInstance();
	threadPool->start(qr);
}

} // rhymetime
