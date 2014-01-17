/*
 * row.cc
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

#include <sqlite3cc/row.h>
#include <sqlite3cc/manipulator.h>
#include <cassert>


sqlite::row::row(
	sqlite3_stmt *handle,
	unsigned long long row_number )
	:
	_handle( handle ),
	_column_index( 0 ),
	_row_number( row_number )
{
}


sqlite::row::row()
	:
	_handle( NULL )
{
}


sqlite::row::operator bool()
	const
{
	return _handle? true : false;
}


int sqlite::row::column_type(
	unsigned int index )
{
	assert( index <
		static_cast< unsigned int >( sqlite3_column_count( _handle ) ) );
	return sqlite3_column_type( _handle, index );
}


unsigned int sqlite::row::column_bytes(
	unsigned int index )
{
	return sqlite3_column_bytes( _handle, index );
}


sqlite::row &sqlite::row::operator >>(
	sqlite::detail::set_index_t t )
{
	_column_index = t._index;
	return *this;
}


bool sqlite::row::operator ==(
	const sqlite::row &other )
	const
{
	return !_handle? !other._handle :
		_handle == other._handle && _row_number == other._row_number;
}


template< >
sqlite::row &sqlite::row::operator >> < sqlite::detail::null_t >(
	sqlite::detail::null_t & )
{
	assert( _column_index <
		static_cast< unsigned int >( sqlite3_column_count( _handle ) ) );
	_column_index++;
	return *this;
}
