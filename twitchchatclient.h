#ifndef TWITCHCHATCLIENT_H
#define TWITCHCHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QQmlEngine>
#include <qqmlregistration.h>

class TwitchChatClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString currentChannel READ currentChannel NOTIFY currentChannelChanged)

public:
    static TwitchChatClient* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    static TwitchChatClient* instance();

    bool isConnected() const;
    QString currentChannel() const;

public slots:
    void connectToChannel(const QString& channel, const QString& token);
    void disconnect();

signals:
    void messageReceived(const QString& username, const QString& message, const QString& color);
    void connectedChanged();
    void currentChannelChanged();
    void connectionError(const QString& error);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onDataReceived();
    void sendPing();

private:
    explicit TwitchChatClient(QObject* parent = nullptr);
    void parseIrcMessage(const QString& message);
    void sendRawMessage(const QString& message);
    QString getTwitchDefaultColor(const QString& username);

    static TwitchChatClient* s_instance;
    QTcpSocket* m_socket;
    QTimer* m_pingTimer;
    QString m_channel;
    QString m_token;
    bool m_connected;
};

#endif // TWITCHCHATCLIENT_H
