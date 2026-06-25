/*
 * Chester The Chat
 * Copyright (C) 2024 - 2026 Timothy Millea
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

#include "chatpager.h"
#include "cmath"

ChatPager::ChatPager(MessageStore *store, ChatFormatter *formatter, QObject *parent)
    : QObject(parent)
    , m_store(store)
    , m_formatter(formatter)
{}

void ChatPager::loadPage(int offset)
{
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

    m_totalPages = std::ceil((double) total / (double) m_messagesPerPage);
    m_currentPage = std::ceil(((double) m_currentOffset / (double) m_messagesPerPage)) + 1;

    emit messagesReady(messages);
    m_isLoading = false;
} //loadPage

void ChatPager::handleScroll(QScrollBar *scrollBar, bool scrollingDown)
{
    if (m_isLoading)
        return;

    int sb_currValue = scrollBar->value();
    int sb_min = scrollBar->minimum();
    int sb_max = scrollBar->maximum();
    int msgCount = m_store->messageCount();
    bool scrollingUp = !scrollingDown;

    if (scrollingDown && sb_currValue == sb_max && (m_currentOffset + m_messagesPerPage) < msgCount) {
        loadPage(m_currentOffset + m_messagesPerPage);
        if (m_currentPage < m_totalPages){
            emit scrollToTopAdjustmentRequested();
        }
        else {
            emit scrollToValueAdjustmentRequested(sb_max);
        }
    }

    else if (scrollingUp && sb_currValue == sb_min && m_currentOffset > 0) {
        loadPage(m_currentOffset - m_messagesPerPage);
        if (m_currentPage > 1){
            emit scrollToBottomAdjustmentRequested();
        }
        else {
            emit scrollToValueAdjustmentRequested(sb_min);
        }
    }
} //handleScroll

int ChatPager::clampOffset(int requestedOffset, int totalMessages) const
{
    int maxOffset = qMax(0, totalMessages - m_messagesPerPage);
    return qBound(0, requestedOffset, maxOffset);
} //clampOffset
