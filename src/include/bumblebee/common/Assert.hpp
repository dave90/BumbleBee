/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
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
#pragma once

#ifndef ASSERT_H
#define ASSERT_H

#include <cassert>
#ifdef _WIN32
#include <include/assert.h>
#endif
#include <iostream>

#define assert_action( condition, action ) \
assert( condition || ( action && false ) )

#define assert_msg( condition, msg ) \
assert_action( condition, std::cerr << "[assert_msg] " << msg << std::endl )

#endif