/*
 * basic_statement.cc
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

#include <sqlite3cc/basic_statement.h>
#include <sqlite3cc/exception.h>
#include <sqlite3cc/connection.h>
#include <sqlite3cc/manipulator.h>
#include <string.h>


sqlite::detail::basic_statement::basic_statement(
	connection &connection,
	const std::string &sql )
	:
	_connection( connection ),
	_handle( NULL ),
	_bind_index( 1 )
{
	int code = prepare( sql );
	if( code != SQLITE_OK ) throw sqlite_error( connection, code );
}


sqlite::detail::basic_statement::basic_statement(
	connection &connection )
	:
	_connection( connection ),
	_handle( NULL ),
	_bind_index( 1 )
{
}


sqlite::detail::basic_statement::~basic_statement()
{
	finalize();
}


int sqlite::detail::basic_statement::prepare(
	const std::string &sql )
{
	finalize();
	return sqlite3_prepare_v2( _connection._handle, sql.c_str(),
		sql.length() + 1, &_handle, NULL );
}


int sqlite::detail::basic_statement::reset()
{
	return sqlite3_reset( _handle );
}


int sqlite::detail::basic_statement::clear_bindings()
{
	_bind_index = 1;
	return sqlite3_clear_bindings( _handle );
}


int sqlite::detail::basic_statement::bind_static(
	unsigned int index,
	const char *value,
	unsigned int value_length )
{
	return sqlite3_bind_text( _handle, index, value, value_length,
		SQLITE_STATIC );
}


int sqlite::detail::basic_statement::bind_static(
	unsigned int index,
	const char *value )
{
	return bind_static( index, value, strlen( value ) );
}


int sqlite::detail::basic_statement::bind_static(
	unsigned int index,
	const std::string &value )
{
	return bind_static( index, value.c_str(), value.length() );
}


int sqlite::detail::basic_statement::bind_null(
	unsigned int index )
{
	return sqlite3_bind_null( _handle, index );
}


int sqlite::detail::basic_statement::bind_static(
	const std::string &name,
	const char *value,
	unsigned int value_length )
{
	return bind_static( bind_parameter_index( name ), value, value_length );
}


int sqlite::detail::basic_statement::bind_static(
	const std::string &name,
	const char *value )
{
	return bind_static( name, value, ::strlen( value ) );
}


int sqlite::detail::basic_statement::bind_static(
	const std::string &name,
	const std::string &value )
{
	return bind_static( name, value.c_str(), value.length() );
}


int sqlite::detail::basic_statement::bind_null(
	const std::string &name )
{
	return bind_null( bind_parameter_index( name ) );
}


int sqlite::detail::basic_statement::finalize()
{
	int code = SQLITE_OK;

	if( _handle ) {
		code = sqlite3_finalize( _handle );
		_handle = NULL;
	}

	return code;
}


int sqlite::detail::basic_statement::bind_parameter_index(
	const std::string &name )
{
	unsigned int index = sqlite3_bind_parameter_index( _handle, name.c_str() );
	if( !index ) throw std::range_error( "named parameter not found" );
	return index;
}


int sqlite::detail::basic_statement::step()
{
	return sqlite3_step( _handle );
}



template< >
sqlite::detail::basic_statement &sqlite::detail::basic_statement::operator <<
	< sqlite::detail::null_t >(
	const sqlite::detail::null_t & )
{
	int code = bind_null( _bind_index );
	if( code != SQLITE_OK ) throw sqlite_error( _connection, code );
	_bind_index++;
	return *this;
}


template< >
sqlite::detail::basic_statement &sqlite::detail::basic_statement::operator <<
	< sqlite::detail::set_index_t >(
	const sqlite::detail::set_index_t &t )
{
	_bind_index = t._index;
	return *this;
}
