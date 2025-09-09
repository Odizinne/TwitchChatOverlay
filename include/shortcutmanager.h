#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <windows.h>

class ShortcutManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static ShortcutManager* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
    static ShortcutManager* instance();

    Q_INVOKABLE QString getShortcutText(int modifiers, int key);
    Q_INVOKABLE void executeShortcut(int modifiers, int key);

    void installHook();
    void uninstallHook();

signals:
    void toggleOverlay();

private:
    explicit ShortcutManager(QObject *parent = nullptr);
    ~ShortcutManager();

    static ShortcutManager* s_instance;
    static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    void processKeyPress(int vkCode, int modifiers);
    UINT qtKeyToVirtualKey(int qtKey);

    HHOOK m_keyboardHook;

    static bool s_ctrlPressed;
    static bool s_shiftPressed;
    static bool s_altPressed;
};

#endif // SHORTCUTMANAGER_H
