#include "applicationui.hpp"
#include "Logger.h"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/Application>
#include <bb/cascades/ListItemProvider>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>
#include <bb/cascades/TextField>

#include <bb/system/Clipboard>
#include <bb/system/SystemToast>

#include <QThreadPool>

namespace rhymetime {

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI(Application* app) :
		QObject(app), db("app/native/assets/dict.txt"), m_loaded(false)
{
	if ( getValueFor("animations").isNull() ) { // first run
		saveValueFor("animations", 1);
	}

	QmlDocument* qmlCover = QmlDocument::create("asset:///Cover.qml").parent(this);
	Control* sceneRoot = qmlCover->createRootObject<Control>();
	SceneCover* cover = SceneCover::create().content(sceneRoot);
	app->setCover(cover);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);
    qml->setContextProperty("cover", sceneRoot);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app->setScene(root);

    connect( app, SIGNAL( aboutToQuit() ), this, SLOT( onAboutToQuit() ) );

    connect( &db, SIGNAL( progress(int) ), this, SLOT( progress(int const&) ) );
    connect( &db, SIGNAL( loadComplete() ), this, SLOT( databaseLoaded() ) );

    m_root = root;
    startThread(&db);
}


void ApplicationUI::create(Application* app) {
	new ApplicationUI(app);
}


void ApplicationUI::onAboutToQuit() {
	db.close();
}


void ApplicationUI::progress(int const& progress) {
	m_root->setProperty("progressValue", progress);
}


void ApplicationUI::databaseLoaded()
{
	m_loaded = true;
	m_root->setProperty("showProgress", false);

	QString word = m_root->findChild<TextField*>("textField")->text();

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


void ApplicationUI::copyWord(QString const& word)
{
	Clipboard clipboard;
	clipboard.clear();
	clipboard.insert( "text/plain", word.toUtf8() );

	SystemToast st;
	st.setBody( tr("Copied to clipboard: %1").arg(word) );
	st.exec();
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


QVariant ApplicationUI::getValueFor(QString const &objectName)
{
    QVariant value( m_settings.value(objectName) );
	LOGGER("getValueFor()" << objectName << value);

    return value;
}


void ApplicationUI::saveValueFor(QString const& objectName, QVariant const& inputValue)
{
	LOGGER("saveValueFor()" << objectName << inputValue);
	m_settings.setValue(objectName, inputValue);
}


void ApplicationUI::startThread(QRunnable* qr)
{
	qr->setAutoDelete(false);

	QThreadPool *threadPool = QThreadPool::globalInstance();
	threadPool->start(qr);
}

} // rhymetime
