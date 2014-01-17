/*
 * connection.cc
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

#include <sqlite3cc/connection.h>
#include <sqlite3cc/exception.h>
#include <sqlite3cc/command.h>
#include <sqlite3cc/query.h>


sqlite::connection::mutex_guard::mutex_guard(
	connection &connection )
	:
	_mutex( sqlite3_db_mutex( connection._handle ) )
{
	if( _mutex ) sqlite3_mutex_enter( _mutex );
}


sqlite::connection::mutex_guard::~mutex_guard()
{
	leave();
}

void sqlite::connection::mutex_guard::leave()
{
	if( _mutex ) {
		sqlite3_mutex_leave( _mutex );
		_mutex = NULL;
	}
}


sqlite::connection::connection(
	const std::string &filename )
	:
	_handle( NULL )
{
	int code = open( filename );
	if( code != SQLITE_OK ) throw sqlite_error( *this, code );
}


sqlite::connection::connection()
	:
	_handle( NULL )
{
}


sqlite::connection::~connection()
{
	close();
}


int sqlite::connection::open(
	const std::string &filename,
	int flags )
{
	close();
	return sqlite3_open_v2( filename.c_str(), &_handle, flags, NULL );
}


void sqlite::connection::close()
{
	if( _handle ) {
		sqlite3_close( _handle );
		_handle = NULL;
	}
}


void sqlite::connection::exec(
	const std::string &sql )
{
	int code = sqlite3_exec( _handle, sql.c_str(), NULL, NULL, NULL );
	if( code != SQLITE_OK ) throw sqlite_error( *this, code );
}


int sqlite::connection::busy_timeout(
	int duration )
{
	return sqlite3_busy_timeout( _handle, duration );
}


boost::shared_ptr< sqlite::command > sqlite::connection::make_command(
	std::string sql )
{
	return boost::shared_ptr< sqlite::command >(
		new sqlite::command( *this, sql ) );
}


boost::shared_ptr< sqlite::query > sqlite::connection::make_query(
	std::string sql )
{
	return boost::shared_ptr< sqlite::query >(
		new sqlite::query( *this, sql ) );
}
