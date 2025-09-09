#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND", "software");
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Odizinne");
    app.setApplicationName("TwitchChatOverlay");
    app.setQuitOnLastWindowClosed(false);
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Odizinne.TwitchChatOverlay", "Main");

    return app.exec();
}
