/*
 Rose Online Server Emulator
 Copyright (C) 2006,2007 OSRose Team http://osroseon.to.md

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 developed with Main erose/hrose source server + some change from the original
 eich source
 */
#include "charserver.h"

// Do player identification
bool CCharServer::pakDoIdentify( CCharClient* thisclient, CPacket* P )
{
	if ( thisclient->isLoggedIn )
		return false;

	thisclient->userid = GETDWORD( ( *P ), 0x00 );
	memcpy( thisclient->password, &P->Buffer[ 4 ], 32 );

	sql::PreparedStatement* prep =
	    DB->QPrepare( "SELECT username,lastsvr,accesslevel,platinum,online "
	                  "FROM accounts WHERE id=? AND password='?'" );
	prep->setInt( 1, thisclient->userid );
	prep->setString( 2, thisclient->password );

	std::unique_ptr< sql::ResultSet > result( prep->executeQuery( ) );
	DB->QPrepareFree( );

	if ( result == NULL )
		return false;

	if ( result->next( ) != true )
	{
		Log( MSG_HACK, "Someone tried to connect to char server with an "
		               "invalid account" );
		return false;
	}
	else
	{
		strncpy( thisclient->username, result->getString( 1 ).c_str( ), 16 );
		thisclient->channel     = result->getInt( 2 );
		thisclient->accesslevel = result->getInt( 3 );
		thisclient->platinum    = result->getInt( 4 ) != 0;
	}

	Log( MSG_INFO, "User '%s'(#%i) logged in", thisclient->username,
	     thisclient->userid );

	BEGINPACKET( pak, 0x070c );
	ADDBYTE( pak, 0 );
	ADDDWORD( pak, 0x87654321 );
	ADDDWORD( pak, 0x00000000 );
	thisclient->SendPacket( &pak );

	bool online = result->getInt( 5 ) != 0;
	if ( online )
		return false;

	prep = DB->QPrepare( "UPDATE accounts SET online=1 WHERE username='?'" );
	prep->setString( 1, thisclient->username );
	bool bRes = prep->execute( );
	DB->QPrepareFree( );

	if ( !bRes )
	{
		return false;
	}

	thisclient->isLoggedIn     = true;
	thisclient->returnedfromWS = false;
	return true;
}

// Get this clients character list
bool CCharServer::pakGetCharacters( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	(void)P;

	// Properly delete items, quests, friends when deleting a character
	// Hooray for joins.
	sql::PreparedStatement* prep = DB->QPrepare(
	    "DELETE FROM characters, items, list_friend, list_quest USING characters LEFT JOIN items ON items.owner = characters.id LEFT JOIN list_friend \
		ON (list_friend.id = characters.id OR list_friend.idfriend = characters.id) LEFT JOIN list_quest ON list_quest.owner = characters.id WHERE (characters.deletetime > 0 AND characters.deletetime <= %i)" );
	prep->setInt( 1, GetServerTime( ) );
	prep->execute( );
	DB->QPrepareFree( );

	CItem        items[ 10 ];
	unsigned int charnum = 0;
	CCharacter   chars[ 5 ];
	memset( chars, 0, sizeof( CCharacter ) * 5 );
	//        0       1      2    3      4      5     6      7

	prep = DB->QPrepare( "SELECT "
	                     "char_name,level,face,hairStyle,sex,classid,id,"
	                     "deletetime FROM characters WHERE account_name='?'" );
	prep->setString( 1, thisclient->username );
	std::unique_ptr< sql::ResultSet > result( prep->executeQuery( ) );
	DB->QPrepareFree( );

	if ( result->rowsCount( ) == 0 )
		return false;

	result->beforeFirst( );
	while ( result->next( ) )
	{
		strcpy( chars[ charnum ].char_name,
		        result->getString( "char_name" ).c_str( ) );
		chars[ charnum ].level      = result->getInt( "level" );
		chars[ charnum ].face       = result->getInt( "face" );
		chars[ charnum ].hairStyle  = result->getInt( "hairStyle" );
		chars[ charnum ].sex        = result->getInt( "sex" );
		chars[ charnum ].classid    = result->getInt( "classid" );
		chars[ charnum ].id         = result->getInt( "id" );
		chars[ charnum ].DeleteTime = result->getInt( "deletetime" );

		if ( chars[ charnum ].DeleteTime > 0 )
		{
			chars[ charnum ].DeleteTime =
			    chars[ charnum ].DeleteTime - GetServerTime( );
		}
		charnum++;
	}

	BEGINPACKET( pak, 0x0712 );
	ADDBYTE( pak, charnum );
	for ( unsigned k = 0; k < charnum; k++ )
	{
		for ( unsigned j = 0; j < 10; j++ )
			ClearItem( items[ j ] );
		//       0       1      2         3         4        5
		result = DB->QStore(
		    "SELECT itemnum,itemtype,refine,durability,lifespan,slotnum "
		    "FROM items WHERE owner=%i AND slotnum<10",
		    chars[ k ].id );

		if ( result->rowsCount( ) == 0 )
			return false;

		result->beforeFirst( );
		while ( result->next( ) )
		{
			unsigned itemnum            = result->getInt( "slotnum" );
			items[ itemnum ].itemnum    = result->getInt( "itemnum" );
			items[ itemnum ].itemtype   = result->getInt( "itemtype" );
			items[ itemnum ].refine     = result->getInt( "refine" );
			items[ itemnum ].durability = result->getInt( "durability" );
			items[ itemnum ].lifespan   = result->getInt( "lifespan" );
		}

		ADDSTRING( pak, chars[ k ].char_name );
		ADDBYTE( pak, 0x00 );
		ADDBYTE( pak, chars[ k ].sex );         // SEX
		ADDWORD( pak, chars[ k ].level );       // LEVEL
		ADDWORD( pak, chars[ k ].classid );     // CLASSID
		ADDDWORD( pak, chars[ k ].DeleteTime ); // DELETE TIME
		ADDBYTE( pak, 0x00 );                   // thisclient->platinum?0x01:0x00 );
		                                        // // IS PLATINUM?  00-NO;01-YES;02-YES BUT
		                                        // USER IS NOT
		ADDDWORD( pak, chars[ k ].face );       // FACE
		ADDDWORD( pak, chars[ k ].hairStyle );  // HAIR
		ADDWORD( pak, items[ 2 ].itemnum );     // CAP
		ADDWORD( pak, items[ 2 ].refine );      // CAP REFINE
		ADDWORD( pak, items[ 3 ].itemnum );     // BODY
		ADDWORD( pak, items[ 3 ].refine );      // BODY REFINE
		ADDWORD( pak, items[ 5 ].itemnum );     // GLOVES
		ADDWORD( pak, items[ 5 ].refine );      // GLOVES REFINE
		ADDWORD( pak, items[ 6 ].itemnum );     // BOOTS
		ADDWORD( pak, items[ 6 ].refine );      // BOOTS REFINE
		ADDWORD( pak, items[ 1 ].itemnum );     // FACE
		ADDWORD( pak, items[ 1 ].refine );      // FACE REFINE
		ADDWORD( pak, items[ 4 ].itemnum );     // BACK
		ADDWORD( pak, items[ 4 ].refine );      // BACK REFINE
		ADDWORD( pak, items[ 7 ].itemnum );     // WEAPON
		ADDWORD( pak, items[ 7 ].refine );      // WEAPON REFINE
		ADDWORD( pak, items[ 8 ].itemnum );     // SUBWEAPON
		ADDWORD( pak, items[ 8 ].refine );      // SUBWEAPON REFINE
	}
	thisclient->SendPacket( &pak );
	return true;
}

// Send away the world IP
bool CCharServer::pakRequestWorld( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	memcpy( thisclient->charname, &P->Buffer[ 3 ], P->Header.Size - 6 - 4 );
	thisclient->charname[ P->Header.Size - 6 - 4 ] = 0;
	Log( MSG_INFO, "User %s(%i) selected char '%s'", thisclient->username,
	     thisclient->userid, thisclient->charname );

	if ( !DB->QExecute( "UPDATE accounts SET lastchar='%s' WHERE id=%i",
	                    thisclient->charname, thisclient->userid ) )
		return false;

	std::unique_ptr< sql::ResultSet > result = DB->QStore( "SELECT host,port FROM channels WHERE id=%i",
	                                                       thisclient->channel );
	if ( result == NULL )
		return false;

	if ( result->rowsCount( ) != 1 )
	{
		Log( MSG_WARNING, "Invalid Server: %i", thisclient->channel );
		return false;
	}
	result->beforeFirst( );
	result->next( );
	BEGINPACKET( pak, 0x711 );
	ADDWORD( pak, result->getInt( "port" ) ); // PORT
	ADDDWORD( pak, thisclient->userid );
	ADDDWORD( pak, 0x87654321 );
	ADDSTRING( pak, result->getString( "host" ).c_str( ) ); // IP
	ADDBYTE( pak, 0 );
	thisclient->SendPacket( &pak );

	// CHAR INFORMATION        0       1    2    3      4
	result = DB->QStore( "SELECT clanid,clan_rank,id,level,classid FROM "
	                     "characters WHERE char_name='%s'",
	                     thisclient->charname );
	if ( result == NULL )
		return false;

	if ( result->rowsCount( ) != 1 )
	{
		Log( MSG_WARNING, "Number of user with charname '%s' is %i",
		     thisclient->charname, result->rowsCount( ) );
		return false;
	}

	thisclient->clanid    = result->getInt( "clanid" );
	thisclient->clan_rank = result->getInt( "clan_rank" );
	thisclient->charid    = result->getInt( "id" );
	thisclient->level     = result->getInt( "level" );
	thisclient->job       = result->getInt( "classid" );
	// MESSENGER INFORMATION
	//              //          0        1
	result = DB->QStore( "SELECT idfriend,namefriend FROM list_friend WHERE id=%i",
	                     thisclient->charid );

	if ( result == NULL )
		return false;

	uint8_t nfriends = result->rowsCount( ) & 0xff;

	RESETPACKET( pak, 0x7e1 ); // Friend list command
	ADDBYTE( pak, 0x06 );      // Sending Friend List
	ADDBYTE( pak, nfriends );  // # friends

	result->beforeFirst( );
	while ( result->next( ) )
	{
		CFriendList* newfriend = new CFriendList;
		assert( newfriend );
		newfriend->id = result->getInt( "idfriend" );                          // friendid
		strcpy( newfriend->name, result->getString( "namefriend" ).c_str( ) ); // friend name
		thisclient->FriendList.push_back( newfriend );

		ADDWORD( pak, newfriend->id ); // friend id
		ADDWORD( pak, 0x0000 );
		CCharClient* otherclient = (CCharClient*)GetClientByID( newfriend->id );
		if ( otherclient != NULL ) // is online??
		{
			ADDBYTE( pak, 0x07 ); // online
			ChangeMessengerStatus( thisclient, otherclient, 0x07 );
		}
		else
		{
			ADDBYTE( pak, 0x08 ); // offline
		}
		ADDSTRING( pak, newfriend->name ); // friend name
		ADDBYTE( pak, 0x00 );
	}
	thisclient->SendPacket( &pak );
	thisclient->logout = false;

	// SEND CLAN INFORMATION
	SendClanInfo( thisclient );

	return true;
}

// Create a character
bool CCharServer::pakCreateChar( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	// Get info from packet
	unsigned sex = GETBYTE( ( *P ), 0 ) % 2;
	unsigned hairstyle = GETBYTE( ( *P ), 2 );
	unsigned face = GETBYTE( ( *P ), 3 );
	string                            newname = (char*)&P->Buffer[ 7 ];
	std::unique_ptr< sql::ResultSet > result  = DB->QStore( "SELECT id FROM characters WHERE account_name='%s'",
	                                                       thisclient->username );

	if ( result == NULL )
		return false;

	if ( result->rowsCount( ) == 5 )
	{
		STARTPACKET( pak, 0x0713, 8 );
		SETWORD( pak, 0x00, 4 )
		thisclient->SendPacket( &pak );
		return true;
	}

	result = DB->QStore( "SELECT id FROM characters WHERE char_name='%s'",
	                     newname.c_str( ) );

	if ( result == NULL )
		return false;

	if ( result->rowsCount( ) > 0 )
	{
		STARTPACKET( pak, 0x0713, 8 );
		SETWORD( pak, 0x00, 2 );
		thisclient->SendPacket( &pak );
		return true;
	}

	if ( !DB->QExecute( "INSERT INTO characters (account_name, char_name, "
	                    "face, hairStyle, sex) VALUES('%s','%s',%i,%i,%i)",
	                    thisclient->username, newname.c_str( ), face, hairstyle,
	                    sex ) )
		return false;

	uint64_t mid = DB->GetLastUpdate( );

	if ( !DB->QExecute( "INSERT INTO items VALUES(%i,30,3,0,40,100,3,1,0,0,0,0)", mid ) )
		return false;

	if ( !DB->QExecute( "INSERT INTO items VALUES(%i,1,8,0,40,100,7,1,0,0,0,0)",
	                    mid ) )
		return false;

	if ( sex == 0 )
	{
		if ( !DB->QExecute(
		         "INSERT INTO items VALUES(%i,222,2,0,40,100,12,1,0,0,0,0)",
		         mid ) )
			return false;
	}
	else
	{
		if ( !DB->QExecute(
		         "INSERT INTO items VALUES(%i,221,2,0,40,100,12,1,0,0,0,0)",
		         mid ) )
			return false;
	}

	STARTPACKET( pak, 0x0713, 8 );
	SETWORD( pak, 0x00, 0 );
	thisclient->SendPacket( &pak );

	return true;
}

// Connect WorldServer
bool CCharServer::pakWSReady( CCharClient* thisclient, CPacket* P )
{
	if ( thisclient->isLoggedIn )
		return false;

	if ( GETDWORD( ( *P ), 0 ) != Config.CharPass )
	{
		Log( MSG_HACK, "WorldServer Tried to connect CharServer with wrong "
		               "password " );
		return true;
	}
	thisclient->userid = GETDWORD( ( *P ), 4 ); // our channelid
	thisclient->accesslevel = 0xffff;           // this will identify thisclient as
	                                            // channel

	// add channel
	CChanels* newchannel = new ( nothrow ) CChanels;
	if ( newchannel == NULL )
		return false;

	newchannel->id = GETDWORD( ( *P ), 4 );
	newchannel->ip   = inet_ntoa( thisclient->clientinfo.sin_addr );
	newchannel->port = GETWORD( ( *P ), 8 );
	Log( MSG_INFO, "Channel #%i connected from ip %s", newchannel->id,
	     newchannel->ip );

	// Connect to world server
	newchannel->sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( newchannel->sock == INVALID_SOCKET )
	{
		Log( MSG_WARNING, "Could not access worldserver IP: %s , Port: %i",
		     newchannel->ip, newchannel->port );
		delete newchannel;
		return false;
	}
	struct sockaddr_in ain;
	ain.sin_family      = AF_INET;
	ain.sin_addr.s_addr = inet_addr( newchannel->ip );
	ain.sin_port        = htons( newchannel->port );
	memset( &( ain.sin_zero ), '\0', 8 );

	if ( connect( newchannel->sock, (SOCKADDR*)&ain, sizeof( ain ) ) ==
	     SOCKET_ERROR )
	{
		Log( MSG_WARNING, "Could not access worldserver IP: %s , Port: %i",
		     newchannel->ip, newchannel->port );
		delete newchannel;
		return false;
	}
	ChannelList.push_back( newchannel );

	thisclient->isLoggedIn = true;
	BEGINPACKET( pak, 0x500 );
	ADDDWORD( pak, Config.WorldPass );
	// cryptPacket( (char*)&pak, NULL );
	// EncryptBuffer( CryptTable,(unsigned char*)&pak );
	Log( MSG_INFO, "Accessing worldserver IP: %s , Port: %i", newchannel->ip,
	     newchannel->port );
	send( newchannel->sock, (char*)&pak, pak.Header.Size, 0 );
	return true;
}

// Login Connected
bool CCharServer::pakLoginConnected( CCharClient* thisclient, CPacket* P )
{
	if ( thisclient->isLoggedIn )
		return false;

	if ( GETDWORD( ( *P ), 0 ) != Config.CharPass )
	{
		Log( MSG_HACK, "LoginServer Tried to connect CharServer with wrong "
		               "password " );
		return true;
	}

	thisclient->isLoggedIn = true;
	return true;
}

// Return to char select
bool CCharServer::pakWSCharSelect( CCharClient* thisclient, CPacket* P )
{
	(void)thisclient;
	// if(!thisclient->isLoggedIn) return false;
	Log( MSG_INFO, "World server requested char select" );

	unsigned int userid = GETDWORD( ( *P ), 0 );
	CCharClient* otherclient = GetClientByUserID( userid );
	if ( otherclient == NULL )
	{
		Log( MSG_WARNING, "Invalid userid: %i", userid );
		return true;
	}

	BEGINPACKET( pak, 0x71c );
	// ADDBYTE    ( pak, 0x00 );
	otherclient->SendPacket( &pak );
	Log( MSG_INFO, "Client returning to char select" );
	return true;
}

// ???
bool CCharServer::pak7e5( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	unsigned int action = GETBYTE( ( *P ), 0 );
	switch ( action )
	{
	case 0x03:
	{
		BEGINPACKET( pak, 0x7e5 );
		ADDBYTE( pak, 0x01 );
		ADDWORD( pak, 0x0000 );
		thisclient->SendPacket( &pak );
		break;
	}
	default:
		Log( MSG_WARNING, "Unknown 7e5 action %i", action );
	}
	return true;
}

// Create a chatroom?
bool CCharServer::pakChatrooms( CCharClient* thisclient, CPacket* P )
{
	(void)P;
	if ( !thisclient->isLoggedIn )
		return false;
	return true;
}

// delete/resurrect character
bool CCharServer::pakDeleteChar( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	short int action = GETBYTE( ( *P ), 1 );
	unsigned long int DeleteTime = 0;
	switch ( action )
	{
	case 0x00: // Resurrect
	{
		DeleteTime = 0;
		if ( !DB->QExecute( " UPDATE characters SET deletetime=0 WHERE "
		                    "char_name='%s'",
		                    (char*)&P->Buffer[ 2 ] ) )
			return false;
	}
	break;
	case 0x01: // Delete
	{
		DeleteTime = GetServerTime( ) + Config.DeleteTime;
		if ( !DB->QExecute( " UPDATE characters SET deletetime=%i "
		                    "WHERE char_name='%s'",
		                    DeleteTime, (char*)&P->Buffer[ 2 ] ) )
			return false;
	}
	break;
	}
	BEGINPACKET( pak, 0x714 );
	if ( DeleteTime > 0 )
	{
		ADDDWORD( pak, Config.DeleteTime );
	}
	else
	{
		ADDDWORD( pak, 0x00000000 );
	}
	ADDSTRING( pak, (char*)&P->Buffer[ 2 ] );
	ADDBYTE( pak, 0x00 );
	thisclient->SendPacket( &pak );
	return true;
}

bool CCharServer::pakUpdateLevel( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;
	(void)P;

	uint16_t charid = GETWORD( (*P), 0 );
	uint16_t level = GETWORD( (*P), 2 );
	for (uint32_t i = 0; i < ClientList.size( ); i++)
	{
		CCharClient* player = (CCharClient*)ClientList.at( i );
		if ( player->charid == charid )
		{
			player->level = level;
			break;
		}
	}
	return true;
}

// Disconnect client from char/world server
bool CCharServer::pakLoginDSClient( CCharClient* thisclient, CPacket* P )
{
	if ( !thisclient->isLoggedIn )
		return false;

	uint8_t action = GETBYTE( (*P), 0 );
	switch ( action )
	{
	case 1:
	{
		unsigned int userid = GETDWORD( ( *P ), 1 );
		CCharClient* otherclient = GetClientByUserID( userid );
		if ( otherclient == NULL )
		{
			Log( MSG_WARNING, "Userid '%u' is not online", userid );
			return true;
		}
		otherclient->isLoggedIn = false;
		otherclient->isActive   = false;
		BEGINPACKET( pak, 0x502 );
		ADDBYTE( pak, 1 );
		ADDDWORD( pak, userid );
		ADDBYTE( pak, 0x00 );

		CChanels* thischannel = GetChannelByID( otherclient->channel );
		if ( thischannel != NULL )
		{
			send( thischannel->sock, (char*)&pak, pak.Header.Size, 0 );
		}
		else
		{
			Log( MSG_ERROR, "Check serverid in worldserver.conf it should be "
			                "the same as ID in channels in the mysql database "
			                "%i",
			     thisclient->channel );
		}
	}
	break;
	}
	return true;
}
