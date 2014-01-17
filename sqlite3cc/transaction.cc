/*
 * transaction.cc
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

#include <sqlite3cc/transaction.h>
#include <sqlite3cc/connection.h>
#include <sqlite3cc/exception.h>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>


sqlite::detail::basic_transaction::basic_transaction(
	connection &connection )
	:
	_connection( connection )
{
}


void sqlite::detail::basic_transaction::begin()
{
	_connection.exec( "BEGIN" );
}


void sqlite::detail::basic_transaction::commit()
{
	// we must reset any active queries when committing
	reset_active_queries();
	_connection.exec( "COMMIT" );
}


void sqlite::detail::basic_transaction::reset_active_queries()
{
	sqlite3_stmt *old_handle = NULL;
	while( sqlite3_stmt *handle =
		sqlite3_next_stmt( _connection._handle, old_handle ) )
	{
		int code = sqlite3_reset( handle );
		if( code != SQLITE_OK ) throw sqlite_error( _connection, code );
		old_handle = handle;
	}
}


void sqlite::detail::basic_transaction::rollback()
{
	// we must reset any active queries when rolling back
	reset_active_queries();
	_connection.exec( "ROLLBACK" );
}


sqlite::immediate_transaction::immediate_transaction(
	connection &connection )
	:
	basic_transaction( connection )
{
}


void sqlite::immediate_transaction::begin()
{
	_connection.exec( "BEGIN IMMEDIATE" );
}


sqlite::exclusive_transaction::exclusive_transaction(
	connection &connection )
	:
	basic_transaction( connection )
{
}


void sqlite::exclusive_transaction::begin()
{
	_connection.exec( "BEGIN EXCLUSIVE" );
}


sqlite::recursive_transaction::recursive_transaction(
	connection &connection )
	:
	basic_transaction( connection )
{
	static unsigned long long i = 0;
	static boost::mutex mutex;
	unsigned long long my_i;
	{
		boost::lock_guard< boost::mutex > lock( mutex );
		my_i = i++;
	}
	_sp_name = "_" + boost::lexical_cast< std::string >( my_i++ );
}


void sqlite::recursive_transaction::begin()
{
	_connection.exec( "SAVEPOINT " + _sp_name );
}


void sqlite::recursive_transaction::commit()
{
	// we must reset any active queries when committing
	reset_active_queries();
	_connection.exec( "RELEASE " + _sp_name );
}


void sqlite::recursive_transaction::rollback()
{
	// we must reset any active queries when rolling back
	reset_active_queries();
	_connection.exec( "ROLLBACK TO " + _sp_name );

	// we have rolled back this transaction's savepoint, but the savepoint will
	// remain on the transaction stack unless we also release it
	_connection.exec( "RELEASE " + _sp_name );
}
