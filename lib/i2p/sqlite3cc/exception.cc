/*
 * exception.cc
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

#include <sqlite3cc/exception.h>
#include <sqlite3cc/connection.h>
#include <boost/assign/list_of.hpp>
#include <map>
#include <string>


sqlite::sqlite_error::sqlite_error(
	connection &connection,
	int code )
	:
	_code( code ),
	_message( get_message( code ) + ": " + sqlite3_errmsg( connection._handle ) )
{
}


sqlite::sqlite_error::sqlite_error(
	int code )
	:
	_code( code ),
	_message( get_message( code ) )
{
}


sqlite::sqlite_error::sqlite_error(
	const std::string &message,
	int code )
	:
	_code( code ),
	_message( message )
{
}


sqlite::sqlite_error::~sqlite_error() throw( )
{
}


int sqlite::sqlite_error::get_code() const
{
	return _code;
}


const char* sqlite::sqlite_error::what() const throw( )
{
	return _message.c_str();
}


const std::string &sqlite::sqlite_error::get_message(
	int code )
{
	static const std::map< int, std::string > messages =
		boost::assign::map_list_of
		( SQLITE_OK, "Successful result" )
		( SQLITE_ERROR, "SQL error or missing database" )
		( SQLITE_INTERNAL, "Internal logic error in SQLite" )
		( SQLITE_PERM, "Access permission denied" )
		( SQLITE_ABORT, "Callback routine requested an abort" )
		( SQLITE_BUSY, "The database file is locked" )
		( SQLITE_LOCKED, "A table in the database is locked" )
		( SQLITE_NOMEM, "A malloc() failed" )
		( SQLITE_READONLY, "Attempt to write a readonly database" )
		( SQLITE_INTERRUPT, "Operation terminated by sqlite3_interrupt()" )
		( SQLITE_IOERR, "Some kind of disk I/O error occurred" )
		( SQLITE_CORRUPT, "The database disk image is malformed" )
		( SQLITE_NOTFOUND, "NOT USED. Table or record not found" )
		( SQLITE_FULL, "Insertion failed because database is full" )
		( SQLITE_CANTOPEN, "Unable to open the database file" )
		( SQLITE_PROTOCOL, "NOT USED. Database lock protocol error" )
		( SQLITE_EMPTY, "Database is empty" )
		( SQLITE_SCHEMA, "The database schema changed" )
		( SQLITE_TOOBIG, "String or BLOB exceeds size limit" )
		( SQLITE_CONSTRAINT, "Abort due to constraint violation" )
		( SQLITE_MISMATCH, "Data type mismatch" )
		( SQLITE_MISUSE, "Library used incorrectly" )
		( SQLITE_NOLFS, "Uses OS features not supported on host" )
		( SQLITE_AUTH, "Authorization denied" )
		( SQLITE_FORMAT, "Auxiliary database format error" )
		( SQLITE_RANGE, "2nd parameter to sqlite3_bind out of range" )
		( SQLITE_NOTADB, "File opened that is not a database file" )
		( SQLITE_ROW, "sqlite3_step() has another row ready" )
		( SQLITE_DONE, "sqlite3_step() has finished executing" );

	std::map< int, std::string >::const_iterator i =
		messages.find( code );
	if( i == messages.end() )
		throw std::range_error( "bad sqlite error code" );
	return i->second;
}
