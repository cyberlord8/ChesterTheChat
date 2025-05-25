#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H

#include <QDebug>
#include <QDateTime>

/**
 * @brief Macro for debug logging with timestamp and function info.
 *
 * In Debug builds (QT_DEBUG defined), expands to qDebug() output with time + function name.
 * In Release builds, expands to nothing.
 */
#ifdef QT_DEBUG
#define LOG_DEBUG(msg) qDebug() << QDateTime::currentDateTime().toString("mm:ss.zz") << Q_FUNC_INFO << msg
#else
#define LOG_DEBUG(msg) do {} while(0)
#endif

/**
 * @brief Macro for *verbose* debug logging.
 *
 * Only active if ENABLE_VERBOSE_DEBUG is manually defined.
 * Useful for extremely detailed trace logs.
 */
#ifdef ENABLE_VERBOSE_DEBUG
#define LOG_VERBOSE(msg) qDebug() << "[VERBOSE]" << QDateTime::currentDateTime().toString("mm:ss.zz") << Q_FUNC_INFO << msg
#else
#define LOG_VERBOSE(msg) do {} while(0)
#endif

#define LOG_INFO(...) qInfo() << __VA_ARGS__

#endif // DEBUGMACROS_H
