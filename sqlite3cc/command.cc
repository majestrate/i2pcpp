/*
 * command.cc
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

#include <sqlite3cc/command.h>
#include <sqlite3cc/connection.h>
#include <sqlite3cc/manipulator.h>
#include <cassert>


sqlite::command::command(
	connection &connection,
	const std::string &sql )
	:
	detail::basic_statement( connection, sql ),
	_changes( 0 ),
	_total_changes( 0 ),
	_last_insert_rowid( 0 )
{
	assert( sqlite3_column_count( _handle ) == 0 );
}


sqlite::command::command(
	connection &connection )
	:
	detail::basic_statement( connection ),
	_changes( 0 ),
	_total_changes( 0 ),
	_last_insert_rowid( 0 )
{
}


int sqlite::command::prepare(
	const std::string &sql )
{
	int code = detail::basic_statement::prepare( sql );
	assert( code != SQLITE_OK || sqlite3_column_count( _handle ) == 0 );
	return code;
}


int sqlite::command::step()
{
	connection::mutex_guard lock( _connection );

	int code = detail::basic_statement::step();
	if( code == SQLITE_ROW ) throw sqlite_error( "command has results" );
	if( code != SQLITE_DONE ) throw sqlite_error( _connection, code );

	_changes = sqlite3_changes( _connection._handle );
	_total_changes = sqlite3_total_changes( _connection._handle );
	_last_insert_rowid = sqlite3_last_insert_rowid( _connection._handle );

	return code;
}


template< >
sqlite::detail::basic_statement &sqlite::detail::basic_statement::operator <<
	< sqlite::detail::exec_t >(
	const sqlite::detail::exec_t & )
{
	sqlite::command &that = dynamic_cast< sqlite::command & >( *this );

	int code = that.step();
	if( code != SQLITE_DONE ) {
		if( code == SQLITE_ROW )
			throw sqlite_error( "statement returned results" );
		else
			throw sqlite_error( _connection, code );
	}
	return *this;
}

