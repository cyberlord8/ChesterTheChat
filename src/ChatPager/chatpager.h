#ifndef CHATPAGER_H
#define CHATPAGER_H

#include <QObject>
#include <QScrollBar>
#include <QList>
#include "../MessageStore/messagestore.h"
#include "../ChatFormatter/chatformatter.h"

class ChatPager : public QObject {
    Q_OBJECT

public:
    ChatPager(MessageStore *store, ChatFormatter *formatter, QObject *parent = nullptr);

    void loadPage(int offset);
    void handleScroll(QScrollBar *scrollBar, bool scrollingDown);
    int currentOffset() const { return m_currentOffset; }
    int messagesPerPage() const { return m_messagesPerPage; }
    int visibleOffset() const { return m_visibleOffset; }
    int visibleLimit() const { return m_visibleLimit; }

signals:
    void messagesReady(const QList<Message> &messages);
    void scrollToTopAdjustmentRequested();
    void scrollToBottomAdjustmentRequested();

private:
    int clampOffset(int requestedOffset, int totalMessages) const;

    MessageStore *m_store;
    ChatFormatter *m_formatter;

    int m_currentOffset = 0;
    int m_messagesPerPage = 20;
    bool m_isLoading = false;
    int m_lastScrollValue = -1;
    int m_visibleOffset = 0;
    qsizetype m_visibleLimit = 0;
};

#endif // CHATPAGER_H
