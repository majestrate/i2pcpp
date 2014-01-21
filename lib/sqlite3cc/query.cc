/*
 * query.cc
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

#include <sqlite3cc/query.h>
#include <sqlite3cc/row.h>
#include <sqlite3cc/connection.h>
#include <assert.h>


sqlite::query::query(
	connection &connection,
	const std::string &sql )
	:
	basic_statement( connection, sql ),
	_next_row_number( 0 )
{
	assert( sqlite3_column_count( _handle ) > 0 );
}


sqlite::query::query(
	connection &connection )
	:
	basic_statement( connection ),
	_next_row_number( 0 )
{
}


int sqlite::query::prepare(
	const std::string &sql )
{
	_next_row_number = 0;
	int code = basic_statement::prepare( sql );
	assert( code != SQLITE_OK || sqlite3_column_count( _handle ) > 0 );
	return code;
}


int sqlite::query::reset()
{
	_next_row_number = 0;
	return basic_statement::reset();
}


sqlite::row sqlite::query::step()
{
	connection::mutex_guard lock( _connection );

	int code = basic_statement::step();
	if( code == SQLITE_DONE ) return row();
	if( code == SQLITE_ROW ) return row( _handle, _next_row_number++ );

	throw sqlite_error( _connection, code );
}


unsigned int sqlite::query::column_count()
{
	return sqlite3_column_count( _handle );
}


const std::string sqlite::query::column_name(
	unsigned int index )
{
	assert( index <
		static_cast< unsigned int >( sqlite3_column_count( _handle ) ) );
	return sqlite3_column_name( _handle, index );
}


unsigned long long sqlite::query::num_results()
{
	reset();
	unsigned long long count = 0;
	while( step() ) count++;
	reset();
	return count;
}


sqlite::query::iterator::iterator(
	sqlite::query &query,
	bool step )
	:
	_query( query )
{
	if( step ) increment();
}


sqlite::row sqlite::query::iterator::dereference() const
{
	return _row;
}


void sqlite::query::iterator::increment()
{
	_row = _query.step();
}


bool sqlite::query::iterator::equal(
	sqlite::query::iterator const &other )
	const
{
	return _row == other._row;
}


sqlite::query::iterator sqlite::query::begin()
{
	reset();
	return sqlite::query::iterator( *this, true );
}


sqlite::query::iterator sqlite::query::end()
{
	return sqlite::query::iterator( *this, false );
}

