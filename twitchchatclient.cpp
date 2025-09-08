#include "twitchchatclient.h"
#include <QDebug>
#include <QRegularExpression>
#include <QCoreApplication>

TwitchChatClient* TwitchChatClient::s_instance = nullptr;

TwitchChatClient* TwitchChatClient::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    if (!s_instance) {
        s_instance = new TwitchChatClient();
    }
    return s_instance;
}

TwitchChatClient* TwitchChatClient::instance()
{
    if (!s_instance) {
        s_instance = new TwitchChatClient();
    }
    return s_instance;
}

TwitchChatClient::TwitchChatClient(QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_pingTimer(new QTimer(this))
    , m_connected(false)
{
    connect(m_socket, &QTcpSocket::connected, this, &TwitchChatClient::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TwitchChatClient::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TwitchChatClient::onSocketError);
    connect(m_socket, &QTcpSocket::readyRead, this, &TwitchChatClient::onDataReceived);

    m_pingTimer->setInterval(60000); // Ping every minute
    connect(m_pingTimer, &QTimer::timeout, this, &TwitchChatClient::sendPing);

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        emit aboutToQuit();
    });
}

bool TwitchChatClient::isConnected() const
{
    return m_connected;
}

QString TwitchChatClient::currentChannel() const
{
    return m_channel;
}

void TwitchChatClient::connectToChannel(const QString& channel, const QString& token)
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }

    // Handle channel name - remove # if present
    QString cleanChannel = channel.trimmed();
    if (cleanChannel.startsWith("#")) {
        cleanChannel = cleanChannel.mid(1); // Remove the #
    }
    m_channel = cleanChannel.toLower();

    // Handle OAuth token - add oauth: prefix if missing
    QString cleanToken = token.trimmed();
    if (!cleanToken.startsWith("oauth:")) {
        cleanToken = "oauth:" + cleanToken;
    }
    m_token = cleanToken;

    qDebug() << "Connecting to Twitch IRC...";
    qDebug() << "Channel:" << m_channel;
    qDebug() << "Token starts with oauth:" << m_token.startsWith("oauth:");

    m_socket->connectToHost("irc.chat.twitch.tv", 6667);
}

void TwitchChatClient::disconnect()
{
    m_pingTimer->stop();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void TwitchChatClient::onSocketConnected()
{
    qDebug() << "Connected to Twitch IRC";

    // Send authentication
    sendRawMessage("CAP REQ :twitch.tv/tags twitch.tv/commands");
    sendRawMessage(QString("PASS oauth:%1").arg(m_token));
    sendRawMessage("NICK justinfan12345"); // Anonymous username
    sendRawMessage(QString("JOIN #%1").arg(m_channel));

    m_connected = true;
    m_pingTimer->start();

    emit connectedChanged();
    emit currentChannelChanged();
}

void TwitchChatClient::onSocketDisconnected()
{
    qDebug() << "Disconnected from Twitch IRC";
    m_connected = false;
    m_pingTimer->stop();
    emit connectedChanged();
}

void TwitchChatClient::onSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error:" << error << m_socket->errorString();
    emit connectionError(m_socket->errorString());
}

void TwitchChatClient::onDataReceived()
{
    QByteArray data = m_socket->readAll();
    QString message = QString::fromUtf8(data);

    QStringList lines = message.split("\r\n", Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        parseIrcMessage(line);
    }
}

void TwitchChatClient::sendPing()
{
    sendRawMessage("PING :tmi.twitch.tv");
}

void TwitchChatClient::parseIrcMessage(const QString& message)
{
    qDebug() << "Raw IRC:" << message;

    if (message.startsWith("PING")) {
        sendRawMessage("PONG :tmi.twitch.tv");
        return;
    }

    if (message.contains("PRIVMSG")) {
        qDebug() << "Found PRIVMSG, parsing...";

        // Extract display-name from tags
        QString displayName;
        QRegularExpression displayNameRe("display-name=([^;]*)");
        QRegularExpressionMatch displayNameMatch = displayNameRe.match(message);
        if (displayNameMatch.hasMatch() && !displayNameMatch.captured(1).isEmpty()) {
            displayName = displayNameMatch.captured(1);
        }

        // Find the part after tags (starts with :username!)
        int userStart = message.indexOf(" :");
        if (userStart == -1) return;

        QString remainder = message.mid(userStart + 2); // Skip " :"

        // Parse :username!...@... PRIVMSG #channel :message
        QRegularExpression msgRe("^([^!]+)![^\\s]*\\s+PRIVMSG\\s+[^\\s]+\\s+:(.*)$");
        QRegularExpressionMatch msgMatch = msgRe.match(remainder);

        if (msgMatch.hasMatch()) {
            QString username = msgMatch.captured(1);
            QString text = msgMatch.captured(2);

            // Use display-name if available, otherwise fallback to username
            QString finalUsername = displayName.isEmpty() ? username : displayName;

            // Extract color from tags
            QString color = "#FFFFFF"; // Default fallback
            QRegularExpression colorRe("color=([^;]*)");
            QRegularExpressionMatch colorMatch = colorRe.match(message);
            if (colorMatch.hasMatch() && !colorMatch.captured(1).isEmpty()) {
                color = colorMatch.captured(1);
            } else {
                // Use Twitch's actual default color algorithm
                color = getTwitchDefaultColor(username);
            }

            qDebug() << "Parsed - Username:" << finalUsername << "Message:" << text << "Color:" << color;
            emit messageReceived(finalUsername, text, color);
        } else {
            qDebug() << "Failed to parse message format:" << remainder;
        }
    }
}

QString TwitchChatClient::getTwitchDefaultColor(const QString& username)
{
    // Twitch's actual default colors (corrected based on reference)
    QStringList twitchColors = {
        "#FF0000", "#0000FF", "#00FF00", "#B22222", "#FF7F50",
        "#9ACD32", "#FF4500", "#2E8B57", "#DAA520", "#D2691E",
        "#5F9EA0", "#1E90FF", "#FF69B4", "#8A2BE2", "#00FF7F"
    };

    // Use Twitch's actual algorithm: first char + last char
    QString lowerUsername = username.toLower();
    if (lowerUsername.isEmpty()) {
        return twitchColors[0]; // fallback to red
    }

    uint32_t n = lowerUsername.at(0).unicode() + lowerUsername.at(lowerUsername.length() - 1).unicode();
    return twitchColors[n % twitchColors.size()];
}

void TwitchChatClient::sendRawMessage(const QString& message)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write((message + "\r\n").toUtf8());
    }
}
