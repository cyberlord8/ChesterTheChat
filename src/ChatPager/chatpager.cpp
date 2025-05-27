#include "chatpager.h"

ChatPager::ChatPager(MessageStore *store, ChatFormatter *formatter, QObject *parent)
    : QObject(parent), m_store(store), m_formatter(formatter)
{}

void ChatPager::loadPage(int offset) {
    if (m_isLoading || offset == m_currentOffset)
        return;

    m_isLoading = true;

    int total = m_store->messageCount();
    if (total == 0) {
        m_isLoading = false;
        return;
    }

    int clamped = clampOffset(offset, total);
    if (clamped == m_currentOffset) {
        m_isLoading = false;
        return;
    }

    QList<Message> messages = m_store->fetchMessages(clamped, m_messagesPerPage);
    m_currentOffset = clamped;
    m_visibleOffset = clamped;
    m_visibleLimit = messages.count();

    emit messagesReady(messages);
    m_isLoading = false;
}

void ChatPager::handleScroll(QScrollBar *scrollBar, bool scrollingDown) {
    if (m_isLoading)
        return;

    int value = scrollBar->value();
    int min = scrollBar->minimum();
    int max = scrollBar->maximum();
    int total = m_store->messageCount();
    bool scrollingUp = !scrollingDown;

    if (scrollingDown && value == max && (m_currentOffset + m_messagesPerPage) < total) {
        loadPage(m_currentOffset + m_messagesPerPage);
        emit scrollToTopAdjustmentRequested();
    }

    if (scrollingUp && value == min && m_currentOffset > 0) {
        loadPage(m_currentOffset - m_messagesPerPage);
        emit scrollToBottomAdjustmentRequested();
    }
}

int ChatPager::clampOffset(int requestedOffset, int totalMessages) const {
    int maxOffset = qMax(0, totalMessages - m_messagesPerPage);
    return qBound(0, requestedOffset, maxOffset);
}