#include <bb/cascades/Application>

#include <QTimer>

#include "applicationui.hpp"
#include "Logger.h"

using namespace bb::cascades;
using namespace rhymetime;

#ifdef DEBUG
namespace {

void redirectedMessageOutput(QtMsgType type, const char *msg) {
	Q_UNUSED(type);
	fprintf(stderr, "%s\n", msg);
}

}
#endif

Q_DECL_EXPORT int main(int argc, char **argv)
{
#ifdef DEBUG
	qInstallMsgHandler(redirectedMessageOutput);
#endif

	qmlRegisterType<QTimer>("ilmtest.rhymetime", 1, 0, "QTimer");

    Application app(argc, argv);
    ApplicationUI::create(&app);

    return Application::exec();
}
