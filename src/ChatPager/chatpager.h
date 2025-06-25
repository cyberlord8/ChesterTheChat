/*
 * Chester The Chat
 * Copyright (C) 2024 Timothy Millea
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */



#ifndef CHATPAGER_H
#define CHATPAGER_H

#include <QList>
#include <QObject>
#include <QScrollBar>
#include "../ChatFormatter/chatformatter.h"
#include "../MessageStore/messagestore.h"

#define NUM_MSGS_PER_PAGE 25

/**
 * @class ChatPager
 * @brief Manages paginated loading of chat messages and scroll-driven page transitions.
 *
 * ChatPager fetches batches of messages from a MessageStore, computes page
 * boundaries, emits the loaded messages, and requests scroll adjustments
 * when the user scrolls to the top or bottom of the view.
 */
class ChatPager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a ChatPager.
     * @param store Pointer to the MessageStore providing access to stored messages.
     * @param formatter Pointer to a ChatFormatter to format fetched messages.
     * @param parent Optional QObject parent.
     */
    ChatPager(MessageStore *store, ChatFormatter *formatter, QObject *parent = nullptr);

    /**
     * @brief Loads a page of messages starting at the given offset.
     *
     * If a load is already in progress or the offset equals the current offset,
     * this call is ignored. Otherwise, it clamps the offset within valid bounds,
     * fetches up to messagesPerPage() messages, updates internal pagination state,
     * and emits messagesReady().
     *
     * @param offset Desired zero-based message index to start loading from.
     */
    void loadPage(int offset);

    /**
     * @brief Handles a scroll event to trigger loading next/previous page.
     *
     * When the user scrolls to the bottom and there are more pages, the next page
     * is loaded and a scrollToTopAdjustmentRequested() is emitted to reposition view.
     * Likewise, scrolling to the top loads the previous page and emits
     * scrollToBottomAdjustmentRequested().
     *
     * @param scrollBar The QScrollBar monitoring the chat view.
     * @param scrollingDown True if the user is scrolling down; false if up.
     */
    void handleScroll(QScrollBar *scrollBar, bool scrollingDown);

    /// Returns the current zero-based offset of the loaded page.
    int currentOffset() const { return m_currentOffset; }

    /// Returns the maximum number of messages per page.
    int messagesPerPage() const { return m_messagesPerPage; }

    /// Returns the offset of the first visible message in the last loaded page.
    int visibleOffset() const { return m_visibleOffset; }

    /// Returns the number of messages visible in the last loaded page.
    int visibleLimit() const { return m_visibleLimit; }

signals:
    /**
     * @brief Emitted when a new page of messages has been fetched.
     * @param messages List of messages ready for display.
     */
    void messagesReady(const QList<Message> &messages);

    /// Emitted after loading a next page to request scrolling the view to the top.
    void scrollToTopAdjustmentRequested();

    /// Emitted after loading a previous page to request scrolling the view to the bottom.
    void scrollToBottomAdjustmentRequested();

    /**
     * @brief Emitted to adjust the scroll bar to a specific value.
     * @param value The scroll bar position to set.
     */
    void scrollToValueAdjustmentRequested(int value);

private:
    /**
     * @brief Clamps the requested offset between 0 and the last possible offset.
     * @param requestedOffset The desired offset.
     * @param totalMessages Total number of messages available.
     * @return A valid offset within [0, maxOffset].
     */
    int clampOffset(int requestedOffset, int totalMessages) const;

    MessageStore     *m_store;           /**< Source of stored chat messages. */
    ChatFormatter    *m_formatter;       /**< Formatter for message content. */

    int   m_currentOffset  = 0;          /**< Currently loaded page start index. */
    int   m_messagesPerPage= NUM_MSGS_PER_PAGE; /**< Page size. */
    bool  m_isLoading      = false;      /**< True while a loadPage is in progress. */
    int   m_lastScrollValue= -1;         /**< Last recorded scroll bar value. */
    int   m_visibleOffset  = 0;          /**< Offset of first visible message. */
    qsizetype m_visibleLimit = 0;        /**< Number of visible messages in view. */
    int   m_currentPage    = 0;          /**< 1-based index of current page. */
    int   m_totalPages     = 0;          /**< Total number of pages available. */
};

#endif // CHATPAGER_H
