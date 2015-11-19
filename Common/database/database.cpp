#include "database.h"
#include "../log.h"

// constructor
CDatabase::CDatabase( char* server, char* username, char* password, char* database, unsigned int port )
{
	Server   = server;
	Username = username;
	Password = password;
	Database = database;
	Port     = port;
	LastPing = time( NULL );
	Timeout  = 0;

	driver = get_driver_instance( );
}

// deconstructor
CDatabase::~CDatabase( )
{
}

// disconnect from mysql
void CDatabase::Disconnect( )
{
	con->close( );
}

int CDatabase::Connect( )
{
	Log( MSG_INFO, "Connecting to MySQL" );
	try
	{
		con.reset( driver->connect( Server, Username, Password ) );
		con->setSchema( Database );
		stmt.reset( con->createStatement( ) );

		std::unique_ptr< sql::ResultSet > res( this->QStore( "SELECT @@wait_timeout AS _timeout" ) );

		if (res->next( ))
		{
			Timeout = res->getInt( "_timeout" ) - 60; // Set the timeout to 1 minute before the connection will timeout
		}
	}
	catch ( sql::SQLException& err )
	{
		Log( MSG_FATALERROR, "Error connecting to MySQL server: %s\n", err.what( ) );
		return -1;
	}

	Log( MSG_INFO, "MySQL Ping Timeout: %d seconds", Timeout );
	return 0;
}

// reconnect to mysql
int CDatabase::Reconnect( )
{
	Log( MSG_INFO, "Reconnecting to MySQL" );

	try
	{
		if ( con->reconnect( ) )
			return 0;
	}
	catch ( sql::SQLException& err )
	{
		Log( MSG_FATALERROR, "Error reconnecting to MySQL server: %s\n", err.getErrorCode( ) );
	}

	return -1;
}

// execute query
bool CDatabase::QExecute( char* Format, ... )
{
	char    query[ 2048 ];
	va_list ap;
	va_start( ap, Format );
	vsprintf_s( query, 2048, Format, ap );
	va_end( ap );

	Log( MSG_QUERY, query );
	SQLMutex.lock( );
	try
	{
		stmt.reset( con->createStatement( ) );
		stmt->execute( query );
	}
	catch ( sql::SQLException& err )
	{
		Log( MSG_FATALERROR, "Could not execute query: %s\n", err.what( ) );
		SQLMutex.unlock();
		return false;
	}

	SQLMutex.unlock( );
	return true;
}

std::unique_ptr< sql::ResultSet > CDatabase::QStore( char* Format, ... )
{
	char    query[ 1024 ];
	va_list ap;
	va_start( ap, Format );
	vsprintf_s( query, 1024, Format, ap );
	va_end( ap );

	Log( MSG_QUERY, query );

	if (pstmt != nullptr)
		pstmt->getMoreResults( );

	std::unique_ptr< sql::ResultSet > res;
	SQLMutex.lock( );
	try
	{
		stmt.reset( con->createStatement( ) );
		res.reset( stmt->executeQuery( query ) );
		res->beforeFirst( );
	}
	catch ( sql::SQLException& err )
	{
		Log( MSG_FATALERROR, "Could not execute query: %s\n", err.what( ) );
		SQLMutex.unlock();
		return nullptr;
	}
	SQLMutex.unlock( );
	return res;
}

sql::PreparedStatement* CDatabase::QPrepare( char* format )
{
	SQLPrepareMutex.lock( );
	try
	{
		pstmt.reset( con->prepareStatement( format ) );
	}
	catch ( sql::SQLException& err )
	{
		Log( MSG_FATALERROR, "Could not execute query: %s\n", err.what( ) );
		SQLPrepareMutex.unlock( );
	}
	SQLPrepareMutex.unlock();
	return pstmt.get(); // Not that safe, but as long as we don't hold on to the PTR we should be good :D
}

void CDatabase::QPrepareFree( )
{
	//SQLPrepareMutex.unlock( );
}

std::unique_ptr< sql::ResultSet > CDatabase::QUse( char* Format, ... )
{
	char    query[ 1024 ];
	va_list ap;
	va_start( ap, Format );
	vsprintf_s( query, 1024, Format, ap );
	va_end( ap );

	return QStore( query );
}

bool CDatabase::Ping( )
{
	if ( time( NULL ) > ( LastPing + Timeout ) )
	{
		//Log(MSG_INFO, "MySql Ping");
		LastPing = time( NULL );

		try
		{
			std::unique_ptr< sql::ResultSet > res( this->QStore( "SELECT @@wait_timeout AS _timeout" ) );

			while ( res->next( ) )
			{
				Timeout = res->getInt( "_timeout" ) - 60; // Set the timeout to 1 minute before the connection will timeout
			}
		}
		catch ( sql::SQLException& err )
		{
			Log( MSG_FATALERROR, "MySQL Ping Failed: %s\n", err.what( ) );
		}
		//Log( MSG_INFO, "MySql Pong, new Timeout: %d seconds", Timeout);
	}
	return true;
}

uint64_t CDatabase::GetLastUpdate( )
{
	if ( stmt == nullptr )
		return 0;

	return stmt->getUpdateCount( );
}
