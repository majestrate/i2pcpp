/*
 * manipulator.cc
 *
 * Copyright (C) 2009 Tim Marston <tim@ed.am>
 *
 * This file is part of sqlite3cc (hereafter referred to as "this program").
 * See http://ed.am/dev/sqlite3cc for more information.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sqlite3cc/manipulator.h>


sqlite::detail::null_t sqlite::null = { };

sqlite::detail::exec_t sqlite::exec = { };


sqlite::detail::set_index_t sqlite::set_index(
	unsigned int index )
{
	detail::set_index_t t = { index };
	return t;
}
