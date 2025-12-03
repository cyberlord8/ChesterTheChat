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

#ifndef VERSION_H
#define VERSION_H

/**
 * @def EXPIRES
 * @brief Enables application expiration logic for alpha and beta builds.
 *
 * If defined, the application checks its build age and prevents execution
 * if the allowed period has passed.
 */
// #define EXPIRES

/**
 * @def VERSION
 * @brief Defines the application version type: "alpha", "beta", or "release".
 *
 * Only one version should be active at a time. The expiration period is determined
 * based on whether it is an alpha or beta build.
 */
#define VERSION "release"
// #define VERSION "alpha"
// #define VERSION "beta"

/**
 * @def ALPHA_TIME
 * @brief If EXPIRES is #defined, Number of days after build date that an alpha version remains valid.
 */
#define ALPHA_TIME 39

/**
 * @def BETA_TIME
 * @brief If EXPIRES is #defined, Number of days after build date that a beta version remains valid.
 */
#define BETA_TIME 395

#endif // VERSION_H
