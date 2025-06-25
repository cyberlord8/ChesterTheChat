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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "version.h"
#include "features.h"

#define APP_NAME "Chester the Chat"
#define RELEASEDATE __DATE__
#define BUILDDATE RELEASEDATE " - " __TIME__
#define SYSTEMINFO QSysInfo::prettyProductName()
#define COPYRIGHT "Copyright © "
#define COMPANY " GRSS"
#define RIGHTS "All rights reserved"
#define WARRANTY "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
#define qt_COPYRIGHT "Copyright © The Qt Company Ltd. and other contributors"
#define APP_LICENSE "Green Radio Software Solutions (GRSS) grants anyone license to use this software for personal use."
#define qt_LICENSE "LGPLv3"


// /**
//  * @struct Message
//  * @brief Represents a single chat message entry in the system.
//  *
//  * Used for both storing messages in the database and rendering them in the UI.
//  */
// struct Message {
//     /**
//      * @brief The name of the user who sent the message.
//      */
//     QString user;

//     /**
//      * @brief The message content.
//      */
//     QString text;

//     /**
//      * @brief UTC timestamp when the message was sent or received.
//      */
//     QDateTime timestamp;

//     /**
//      * @brief Indicates whether the message was sent by the local user.
//      * True if sent by this client, false if received from another user.
//      */
//     bool isSentByMe = false;
// };




#endif // GLOBALS_H
