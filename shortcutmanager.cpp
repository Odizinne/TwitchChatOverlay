#include "shortcutmanager.h"
#include <QDebug>

ShortcutManager* ShortcutManager::s_instance = nullptr;
bool ShortcutManager::s_ctrlPressed = false;
bool ShortcutManager::s_shiftPressed = false;
bool ShortcutManager::s_altPressed = false;

ShortcutManager::ShortcutManager(QObject *parent)
    : QObject(parent)
    , m_keyboardHook(nullptr)
{
    s_instance = this;
    installHook();
}

ShortcutManager::~ShortcutManager()
{
    uninstallHook();
    s_instance = nullptr;
}

ShortcutManager* ShortcutManager::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    if (!s_instance) {
        s_instance = new ShortcutManager();
    }
    return s_instance;
}

ShortcutManager* ShortcutManager::instance()
{
    return s_instance;
}

void ShortcutManager::installHook()
{
    if (m_keyboardHook) {
        return;
    }

    m_keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        lowLevelKeyboardProc,
        GetModuleHandle(nullptr),
        0
        );

    if (!m_keyboardHook) {
        qWarning() << "Failed to install keyboard hook";
    }
}

void ShortcutManager::uninstallHook()
{
    if (m_keyboardHook) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
    }
}

LRESULT CALLBACK ShortcutManager::lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // Track modifier states
            switch (pKeyBoard->vkCode) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                s_ctrlPressed = true;
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                s_shiftPressed = true;
                break;
            case VK_LMENU:
            case VK_RMENU:
                s_altPressed = true;
                break;
            default:
                if (pKeyBoard->vkCode == 'T' && s_ctrlPressed && s_shiftPressed && !s_altPressed) {
                    if (s_instance) {
                        emit s_instance->toggleOverlay();
                    }
                    // Consume the key event by returning 1
                    return 1;
                }
                break;
            }
        } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            switch (pKeyBoard->vkCode) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                s_ctrlPressed = false;
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                s_shiftPressed = false;
                break;
            case VK_LMENU:
            case VK_RMENU:
                s_altPressed = false;
                break;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

QString ShortcutManager::getShortcutText(int modifiers, int key)
{
    QStringList parts;

    if (modifiers & Qt::ControlModifier) parts << "Ctrl";
    if (modifiers & Qt::ShiftModifier) parts << "Shift";
    if (modifiers & Qt::AltModifier) parts << "Alt";

    QString keyText;
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        keyText = QChar('A' + (key - Qt::Key_A));
    } else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        keyText = QChar('0' + (key - Qt::Key_0));
    } else {
        // Handle special characters and shifted numbers
        switch(key) {
        case Qt::Key_Space: keyText = "Space"; break;
        case Qt::Key_Return: keyText = "Return"; break;
        case Qt::Key_Enter: keyText = "Enter"; break;
        case Qt::Key_Tab: keyText = "Tab"; break;
        case Qt::Key_Escape: keyText = "Escape"; break;
        case Qt::Key_Backspace: keyText = "Backspace"; break;
        case Qt::Key_Delete: keyText = "Delete"; break;
        case Qt::Key_Insert: keyText = "Insert"; break;
        case Qt::Key_Home: keyText = "Home"; break;
        case Qt::Key_End: keyText = "End"; break;
        case Qt::Key_PageUp: keyText = "Page Up"; break;
        case Qt::Key_PageDown: keyText = "Page Down"; break;
        case Qt::Key_Left: keyText = "Left"; break;
        case Qt::Key_Right: keyText = "Right"; break;
        case Qt::Key_Up: keyText = "Up"; break;
        case Qt::Key_Down: keyText = "Down"; break;

        // Function keys
        case Qt::Key_F1: keyText = "F1"; break;
        case Qt::Key_F2: keyText = "F2"; break;
        case Qt::Key_F3: keyText = "F3"; break;
        case Qt::Key_F4: keyText = "F4"; break;
        case Qt::Key_F5: keyText = "F5"; break;
        case Qt::Key_F6: keyText = "F6"; break;
        case Qt::Key_F7: keyText = "F7"; break;
        case Qt::Key_F8: keyText = "F8"; break;
        case Qt::Key_F9: keyText = "F9"; break;
        case Qt::Key_F10: keyText = "F10"; break;
        case Qt::Key_F11: keyText = "F11"; break;
        case Qt::Key_F12: keyText = "F12"; break;

        // Special characters that might appear when using Shift
        case Qt::Key_Exclam: keyText = "1"; break;        // Shift+1 = !
        case Qt::Key_At: keyText = "2"; break;            // Shift+2 = @
        case Qt::Key_NumberSign: keyText = "3"; break;    // Shift+3 = #
        case Qt::Key_Dollar: keyText = "4"; break;        // Shift+4 = $
        case Qt::Key_Percent: keyText = "5"; break;       // Shift+5 = %
        case Qt::Key_AsciiCircum: keyText = "6"; break;   // Shift+6 = ^
        case Qt::Key_Ampersand: keyText = "7"; break;     // Shift+7 = &
        case Qt::Key_Asterisk: keyText = "8"; break;      // Shift+8 = *
        case Qt::Key_ParenLeft: keyText = "9"; break;     // Shift+9 = (
        case Qt::Key_ParenRight: keyText = "0"; break;    // Shift+0 = )

        // Other common symbols
        case Qt::Key_Minus: keyText = "-"; break;
        case Qt::Key_Underscore: keyText = "-"; break;    // Shift+- = _
        case Qt::Key_Equal: keyText = "="; break;
        case Qt::Key_Plus: keyText = "="; break;          // Shift+= = +
        case Qt::Key_BracketLeft: keyText = "["; break;
        case Qt::Key_BracketRight: keyText = "]"; break;
        case Qt::Key_BraceLeft: keyText = "["; break;     // Shift+[ = {
        case Qt::Key_BraceRight: keyText = "]"; break;    // Shift+] = }
        case Qt::Key_Backslash: keyText = "\\"; break;
        case Qt::Key_Bar: keyText = "\\"; break;          // Shift+\ = |
        case Qt::Key_Semicolon: keyText = ";"; break;
        case Qt::Key_Colon: keyText = ";"; break;         // Shift+; = :
        case Qt::Key_Apostrophe: keyText = "'"; break;
        case Qt::Key_QuoteDbl: keyText = "'"; break;      // Shift+' = "
        case Qt::Key_Comma: keyText = ","; break;
        case Qt::Key_Less: keyText = ","; break;          // Shift+, =
        case Qt::Key_Period: keyText = "."; break;
        case Qt::Key_Greater: keyText = "."; break;       // Shift+. = >
        case Qt::Key_Slash: keyText = "/"; break;
        case Qt::Key_Question: keyText = "/"; break;      // Shift+/ = ?
        case Qt::Key_QuoteLeft: keyText = "`"; break;     // Backtick
        case Qt::Key_AsciiTilde: keyText = "`"; break;    // Shift+` = ~

        default:
            // For any unknown keys, try to show something meaningful
            if (key >= 32 && key <= 126) {
                keyText = QChar(key);
            } else {
                keyText = QString("Key_%1").arg(key);
            }
            break;
        }
    }

    parts << keyText;
    return parts.join("+");
}

UINT ShortcutManager::qtKeyToVirtualKey(int qtKey)
{
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
        return 'A' + (qtKey - Qt::Key_A);
    } else if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
        return '0' + (qtKey - Qt::Key_0);
    } else {
        switch(qtKey) {
        case Qt::Key_Space: return VK_SPACE;
        case Qt::Key_Return: return VK_RETURN;
        case Qt::Key_Tab: return VK_TAB;
        case Qt::Key_Escape: return VK_ESCAPE;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Delete: return VK_DELETE;
        case Qt::Key_Insert: return VK_INSERT;
        case Qt::Key_Home: return VK_HOME;
        case Qt::Key_End: return VK_END;
        case Qt::Key_PageUp: return VK_PRIOR;
        case Qt::Key_PageDown: return VK_NEXT;
        case Qt::Key_Left: return VK_LEFT;
        case Qt::Key_Right: return VK_RIGHT;
        case Qt::Key_Up: return VK_UP;
        case Qt::Key_Down: return VK_DOWN;

        // Function keys
        case Qt::Key_F1: return VK_F1;
        case Qt::Key_F2: return VK_F2;
        case Qt::Key_F3: return VK_F3;
        case Qt::Key_F4: return VK_F4;
        case Qt::Key_F5: return VK_F5;
        case Qt::Key_F6: return VK_F6;
        case Qt::Key_F7: return VK_F7;
        case Qt::Key_F8: return VK_F8;
        case Qt::Key_F9: return VK_F9;
        case Qt::Key_F10: return VK_F10;
        case Qt::Key_F11: return VK_F11;
        case Qt::Key_F12: return VK_F12;

        // Map shifted characters back to their base keys
        case Qt::Key_Exclam: return '1';        // !
        case Qt::Key_At: return '2';            // @
        case Qt::Key_NumberSign: return '3';    // #
        case Qt::Key_Dollar: return '4';        // $
        case Qt::Key_Percent: return '5';       // %
        case Qt::Key_AsciiCircum: return '6';   // ^
        case Qt::Key_Ampersand: return '7';     // &
        case Qt::Key_Asterisk: return '8';      // *
        case Qt::Key_ParenLeft: return '9';     // (
        case Qt::Key_ParenRight: return '0';    // )

        // Other symbols
        case Qt::Key_Minus:
        case Qt::Key_Underscore: return VK_OEM_MINUS;
        case Qt::Key_Equal:
        case Qt::Key_Plus: return VK_OEM_PLUS;
        case Qt::Key_BracketLeft:
        case Qt::Key_BraceLeft: return VK_OEM_4;
        case Qt::Key_BracketRight:
        case Qt::Key_BraceRight: return VK_OEM_6;
        case Qt::Key_Backslash:
        case Qt::Key_Bar: return VK_OEM_5;
        case Qt::Key_Semicolon:
        case Qt::Key_Colon: return VK_OEM_1;
        case Qt::Key_Apostrophe:
        case Qt::Key_QuoteDbl: return VK_OEM_7;
        case Qt::Key_Comma:
        case Qt::Key_Less: return VK_OEM_COMMA;
        case Qt::Key_Period:
        case Qt::Key_Greater: return VK_OEM_PERIOD;
        case Qt::Key_Slash:
        case Qt::Key_Question: return VK_OEM_2;
        case Qt::Key_QuoteLeft:
        case Qt::Key_AsciiTilde: return VK_OEM_3;

        default: return 0;
        }
    }
}

void ShortcutManager::executeShortcut(int modifiers, int key)
{
    INPUT inputs[8];
    int inputCount = 0;

    // Press modifiers first
    if (modifiers & Qt::ControlModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_CONTROL;
        inputs[inputCount].ki.dwFlags = 0;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    if (modifiers & Qt::ShiftModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_SHIFT;
        inputs[inputCount].ki.dwFlags = 0;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    if (modifiers & Qt::AltModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_MENU;
        inputs[inputCount].ki.dwFlags = 0;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    // Press the main key
    UINT vkCode = qtKeyToVirtualKey(key);
    if (vkCode != 0) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = vkCode;
        inputs[inputCount].ki.dwFlags = 0;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;

        // Release the main key
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = vkCode;
        inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    // Release modifiers in reverse order
    if (modifiers & Qt::AltModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_MENU;
        inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    if (modifiers & Qt::ShiftModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_SHIFT;
        inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    if (modifiers & Qt::ControlModifier) {
        inputs[inputCount].type = INPUT_KEYBOARD;
        inputs[inputCount].ki.wVk = VK_CONTROL;
        inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[inputCount].ki.wScan = 0;
        inputs[inputCount].ki.time = 0;
        inputs[inputCount].ki.dwExtraInfo = 0;
        inputCount++;
    }

    // Send the input events
    SendInput(inputCount, inputs, sizeof(INPUT));

    qDebug() << "Executed shortcut:" << getShortcutText(modifiers, key);
}
