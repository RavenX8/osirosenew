// Props to ExJam for this code :D
#include "../worldserver.h"

void CWorldServer::ReadQSD( std::string path, uint32_t index )
{
	CRoseFile* fh = new CRoseFile( path.c_str( ), FM_READ | FM_BINARY );
	if ( fh->IsOpen( ) )
	{ // goto done;

		Log( MSG_LOAD, "Loading %s", path.c_str() );

		fh->Seek( 4, SEEK_CUR );
		uint32_t BlockCount = fh->Get< uint32_t >( );
		fh->Seek( fh->Get< uint16_t >( ), SEEK_CUR );
		for ( uint32_t i = 0; i < BlockCount; i++ )
		{
			uint32_t RecordCount = fh->Get< uint32_t >( );
			fh->Seek( fh->Get< uint16_t >( ), SEEK_CUR );
			for ( uint32_t j = 0; j < RecordCount; j++ )
			{
				CQuestTrigger* trigger = new CQuestTrigger( );
				trigger->id = ( ( index * 0x10000 ) + ( i * 0x100 ) + j );
				trigger->CheckNext      = fh->Get< uint8_t >( );
				trigger->ConditionCount = fh->Get< uint32_t >( );
				trigger->ActionCount    = fh->Get< uint32_t >( );
				uint32_t len            = fh->Get< uint16_t >( );
				char*    _TriggerName   = new char[ len + 1 ];
				fh->Read( _TriggerName, len, 1 );
				_TriggerName[ len ]  = 0;
				trigger->TriggerName = _TriggerName;

				if ( trigger->ConditionCount > 0 )
				{
					trigger->Conditions = new CQuestTrigger::SQuestDatum*[ trigger->ConditionCount ];
					for ( uint32_t k = 0; k < trigger->ConditionCount; k++ )
					{
						CQuestTrigger::SQuestDatum* data = new CQuestTrigger::SQuestDatum( );
						data->size                       = fh->Get< int >( );
						data->opcode                     = fh->Get< int >( );
						data->data                       = new uint8_t[ data->size - 8 ];
						fh->Read( data->data, data->size - 8, 1 );
						trigger->Conditions[ k ] = data;
					}
				}
				else
				{
					trigger->Conditions = NULL;
				}

				if ( trigger->ActionCount > 0 )
				{
					trigger->Actions = new CQuestTrigger::SQuestDatum*[ trigger->ActionCount ];
					for ( uint32_t k = 0; k < trigger->ActionCount; k++ )
					{
						CQuestTrigger::SQuestDatum* data = new CQuestTrigger::SQuestDatum( );
						data->size                       = fh->Get< int >( );
						data->opcode                     = fh->Get< int >( ) - 0x01000000;
						data->data                       = new uint8_t[ data->size - 8 ];
						fh->Read( data->data, data->size - 8, 1 );
						trigger->Actions[ k ] = data;
					}
				}
				else
				{
					trigger->Actions = NULL;
				}

				trigger->TriggerHash = MakeStrHash( _TriggerName );
				TriggerList.push_back( trigger );
			}
		}
	}
	else
		Log( MSG_ERROR, "QSD File: '%s'", path.c_str() );

	fh->Close( );
	delete fh;
}

void CWorldServer::LoadQuestData( )
{
	CStrStb* stbQuest = new CStrStb( "3DDATA\\STB\\LIST_QUESTDATA.STB" );

	for ( uint32_t i = 1; i < stbQuest->Rows( ); i++ )
	{
		if ( stbQuest->Data( i, 0 ) )
		{
			GServer->ReadQSD( stbQuest->Data( i, 0 ), i );
		}
	}

	Log( MSG_INFO, "Finished loading quest data                              " );

	qstCondFunc[ 0 ]  = &QUEST_COND_000;
	qstCondFunc[ 1 ]  = &QUEST_COND_001;
	qstCondFunc[ 2 ]  = &QUEST_COND_002;
	qstCondFunc[ 3 ]  = &QUEST_COND_003;
	qstCondFunc[ 4 ]  = &QUEST_COND_004;
	qstCondFunc[ 5 ]  = &QUEST_COND_005;
	qstCondFunc[ 6 ]  = &QUEST_COND_006;
	qstCondFunc[ 7 ]  = &QUEST_COND_007;
	qstCondFunc[ 8 ]  = &QUEST_COND_008;
	qstCondFunc[ 9 ]  = &QUEST_COND_009;
	qstCondFunc[ 10 ] = &QUEST_COND_010;
	qstCondFunc[ 11 ] = &QUEST_COND_011;
	qstCondFunc[ 12 ] = &QUEST_COND_012;
	qstCondFunc[ 13 ] = &QUEST_COND_013;
	qstCondFunc[ 14 ] = &QUEST_COND_014;
	qstCondFunc[ 15 ] = &QUEST_COND_015;
	qstCondFunc[ 16 ] = &QUEST_COND_016;
	qstCondFunc[ 17 ] = &QUEST_COND_017;
	qstCondFunc[ 18 ] = &QUEST_COND_018;
	qstCondFunc[ 19 ] = &QUEST_COND_019;
	qstCondFunc[ 20 ] = &QUEST_COND_020;
	qstCondFunc[ 21 ] = &QUEST_COND_021;
	qstCondFunc[ 22 ] = &QUEST_COND_022;
	qstCondFunc[ 23 ] = &QUEST_COND_023;
	qstCondFunc[ 24 ] = &QUEST_COND_024;
	qstCondFunc[ 25 ] = &QUEST_COND_025;
	qstCondFunc[ 26 ] = &QUEST_COND_026;
	qstCondFunc[ 27 ] = &QUEST_COND_027;
	qstCondFunc[ 28 ] = &QUEST_COND_028;
	qstCondFunc[ 29 ] = &QUEST_COND_029;
	qstCondFunc[ 30 ] = &QUEST_COND_030;

	qstRewdFunc[ 0 ]  = &QUEST_REWD_000;
	qstRewdFunc[ 1 ]  = &QUEST_REWD_001;
	qstRewdFunc[ 2 ]  = &QUEST_REWD_002;
	qstRewdFunc[ 3 ]  = &QUEST_REWD_003;
	qstRewdFunc[ 4 ]  = &QUEST_REWD_004;
	qstRewdFunc[ 5 ]  = &QUEST_REWD_005;
	qstRewdFunc[ 6 ]  = &QUEST_REWD_006;
	qstRewdFunc[ 7 ]  = &QUEST_REWD_007;
	qstRewdFunc[ 8 ]  = &QUEST_REWD_008;
	qstRewdFunc[ 9 ]  = &QUEST_REWD_009;
	qstRewdFunc[ 10 ] = &QUEST_REWD_010;
	qstRewdFunc[ 11 ] = &QUEST_REWD_011;
	qstRewdFunc[ 12 ] = &QUEST_REWD_012;
	qstRewdFunc[ 13 ] = &QUEST_REWD_013;
	qstRewdFunc[ 14 ] = &QUEST_REWD_014;
	qstRewdFunc[ 15 ] = &QUEST_REWD_015;
	qstRewdFunc[ 16 ] = &QUEST_REWD_016;
	qstRewdFunc[ 17 ] = &QUEST_REWD_017;
	qstRewdFunc[ 18 ] = &QUEST_REWD_018;
	qstRewdFunc[ 19 ] = &QUEST_REWD_019;
	qstRewdFunc[ 20 ] = &QUEST_REWD_020;
	qstRewdFunc[ 21 ] = &QUEST_REWD_021;
	qstRewdFunc[ 22 ] = &QUEST_REWD_022;
	qstRewdFunc[ 23 ] = &QUEST_REWD_023;
	qstRewdFunc[ 24 ] = &QUEST_REWD_024;
	qstRewdFunc[ 25 ] = &QUEST_REWD_025;
	qstRewdFunc[ 26 ] = &QUEST_REWD_026;
	qstRewdFunc[ 27 ] = &QUEST_REWD_027;
	qstRewdFunc[ 28 ] = &QUEST_REWD_028;

	delete stbQuest;
}
