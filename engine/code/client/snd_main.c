/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2005 Stuart Dalton (badcdev@gmail.com)

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "client.h"
#include "snd_codec.h"
#include "snd_local.h"
#include "snd_public.h"

cvar_t *s_volume;
cvar_t *s_muted;
cvar_t *s_musicVolume;
cvar_t *s_doppler;
cvar_t *s_backend;
cvar_t *s_muteWhenMinimized;
cvar_t *s_muteWhenUnfocused;

static soundInterface_t si;

/*
=================
S_ValidateInterface
=================
*/
static qboolean S_ValidSoundInterface( soundInterface_t *si )
{
	if( !si->Shutdown ) return qfalse;
	if( !si->StartSound ) return qfalse;
	if( !si->StartLocalSound ) return qfalse;
	if( !si->StartBackgroundTrack ) return qfalse;
	if( !si->StopBackgroundTrack ) return qfalse;
	if( !si->RawSamples ) return qfalse;
	if( !si->StopAllSounds ) return qfalse;
	if( !si->ClearLoopingSounds ) return qfalse;
	if( !si->AddLoopingSound ) return qfalse;
	if( !si->AddRealLoopingSound ) return qfalse;
	if( !si->StopLoopingSound ) return qfalse;
	if( !si->Respatialize ) return qfalse;
	if( !si->UpdateEntityPosition ) return qfalse;
	if( !si->Update ) return qfalse;
	if( !si->DisableSounds ) return qfalse;
	if( !si->BeginRegistration ) return qfalse;
	if( !si->RegisterSound ) return qfalse;
	if( !si->ClearSoundBuffer ) return qfalse;
	if( !si->SoundInfo ) return qfalse;
	if( !si->SoundList ) return qfalse;

#ifdef USE_VOIP
	if( !si->StartCapture ) return qfalse;
	if( !si->AvailableCaptureSamples ) return qfalse;
	if( !si->Capture ) return qfalse;
	if( !si->StopCapture ) return qfalse;
	if( !si->MasterGain ) return qfalse;
#endif

	return qtrue;
}

/*
=================
S_StartSound
=================
*/
void S_StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx )
{
	if( si.StartSound ) {
		si.StartSound( origin, entnum, entchannel, sfx );
	}
}

/*
=================
S_StartLocalSound
=================
*/
void S_StartLocalSound( sfxHandle_t sfx, int channelNum )
{
	if( si.StartLocalSound ) {
		si.StartLocalSound( sfx, channelNum );
	}
}

/*
=================
S_StartBackgroundTrack
=================
*/
void S_StartBackgroundTrack( const char *intro, const char *loop )
{
	if( si.StartBackgroundTrack ) {
		si.StartBackgroundTrack( intro, loop );
	}
}

/*
=================
S_StopBackgroundTrack
=================
*/
void S_StopBackgroundTrack( void )
{
	if( si.StopBackgroundTrack ) {
		si.StopBackgroundTrack( );
	}
}

/*
=================
S_RawSamples
=================
*/
void S_RawSamples (int stream, int samples, int rate, int width, int channels,
		   const byte *data, float volume, int entityNum)
{
	if(si.RawSamples)
		si.RawSamples(stream, samples, rate, width, channels, data, volume, entityNum);
}

/*
=================
S_StopAllSounds
=================
*/
void S_StopAllSounds( void )
{
	if( si.StopAllSounds ) {
		si.StopAllSounds( );
	}
}

/*
=================
S_ClearLoopingSounds
=================
*/
void S_ClearLoopingSounds( qboolean killall )
{
	if( si.ClearLoopingSounds ) {
		si.ClearLoopingSounds( killall );
	}
}

/*
=================
S_AddLoopingSound
=================
*/
void S_AddLoopingSound( int entityNum, const vec3_t origin,
		const vec3_t velocity, sfxHandle_t sfx )
{
	if( si.AddLoopingSound ) {
		si.AddLoopingSound( entityNum, origin, velocity, sfx );
	}
}

/*
=================
S_AddRealLoopingSound
=================
*/
void S_AddRealLoopingSound( int entityNum, const vec3_t origin,
		const vec3_t velocity, sfxHandle_t sfx )
{
	if( si.AddRealLoopingSound ) {
		si.AddRealLoopingSound( entityNum, origin, velocity, sfx );
	}
}

/*
=================
S_StopLoopingSound
=================
*/
void S_StopLoopingSound( int entityNum )
{
	if( si.StopLoopingSound ) {
		si.StopLoopingSound( entityNum );
	}
}

/*
=================
S_Respatialize
=================
*/
void S_Respatialize( int entityNum, const vec3_t origin,
		vec3_t axis[3], int inwater
#ifdef TA_SPLITVIEW
		, int listener
#endif
		)
{
	if( si.Respatialize ) {
		si.Respatialize( entityNum, origin, axis, inwater
#ifdef TA_SPLITVIEW
					, listener
#endif
					);
	}
}

/*
=================
S_UpdateEntityPosition
=================
*/
void S_UpdateEntityPosition( int entityNum, const vec3_t origin )
{
	if( si.UpdateEntityPosition ) {
		si.UpdateEntityPosition( entityNum, origin );
	}
}

/*
=================
S_Update
=================
*/
void S_Update( void )
{
	if(s_muted->integer)
	{
		if(!(s_muteWhenMinimized->integer && com_minimized->integer) &&
		   !(s_muteWhenUnfocused->integer && com_unfocused->integer))
		{
			s_muted->integer = qfalse;
			s_muted->modified = qtrue;
		}
	}
	else
	{
		if((s_muteWhenMinimized->integer && com_minimized->integer) ||
		   (s_muteWhenUnfocused->integer && com_unfocused->integer))
		{
			s_muted->integer = qtrue;
			s_muted->modified = qtrue;
		}
	}
	
	if( si.Update ) {
		si.Update( );
	}
}

/*
=================
S_DisableSounds
=================
*/
void S_DisableSounds( void )
{
	if( si.DisableSounds ) {
		si.DisableSounds( );
	}
}

/*
=================
S_BeginRegistration
=================
*/
void S_BeginRegistration( void )
{
	if( si.BeginRegistration ) {
		si.BeginRegistration( );
	}
}

/*
=================
S_RegisterSound
=================
*/
sfxHandle_t	S_RegisterSound( const char *sample, qboolean compressed )
{
	if( si.RegisterSound ) {
		return si.RegisterSound( sample, compressed );
	} else {
		return 0;
	}
}

/*
=================
S_ClearSoundBuffer
=================
*/
void S_ClearSoundBuffer( void )
{
	if( si.ClearSoundBuffer ) {
		si.ClearSoundBuffer( );
	}
}

/*
=================
S_SoundInfo
=================
*/
void S_SoundInfo( void )
{
	if( si.SoundInfo ) {
		si.SoundInfo( );
	}
}

/*
=================
S_SoundList
=================
*/
void S_SoundList( void )
{
	if( si.SoundList ) {
		si.SoundList( );
	}
}


#ifdef USE_VOIP
/*
=================
S_StartCapture
=================
*/
void S_StartCapture( void )
{
	if( si.StartCapture ) {
		si.StartCapture( );
	}
}

/*
=================
S_AvailableCaptureSamples
=================
*/
int S_AvailableCaptureSamples( void )
{
	if( si.AvailableCaptureSamples ) {
		return si.AvailableCaptureSamples( );
	}
	return 0;
}

/*
=================
S_Capture
=================
*/
void S_Capture( int samples, byte *data )
{
	if( si.Capture ) {
		si.Capture( samples, data );
	}
}

/*
=================
S_StopCapture
=================
*/
void S_StopCapture( void )
{
	if( si.StopCapture ) {
		si.StopCapture( );
	}
}

/*
=================
S_MasterGain
=================
*/
void S_MasterGain( float gain )
{
	if( si.MasterGain ) {
		si.MasterGain( gain );
	}
}
#endif

//=============================================================================

/*
=================
S_Play_f
=================
*/
void S_Play_f( void ) {
	int 		i;
	sfxHandle_t	h;
	char		name[256];

	if( !si.RegisterSound || !si.StartLocalSound ) {
		return;
	}

	i = 1;
	while ( i<Cmd_Argc() ) {
		Q_strncpyz( name, Cmd_Argv(i), sizeof(name) );
		h = si.RegisterSound( name, qfalse );

		if( h ) {
			si.StartLocalSound( h, CHAN_LOCAL_SOUND );
		}
		i++;
	}
}

/*
=================
S_Music_f
=================
*/
void S_Music_f( void ) {
	int		c;

	if( !si.StartBackgroundTrack ) {
		return;
	}

	c = Cmd_Argc();

	if ( c == 2 ) {
		si.StartBackgroundTrack( Cmd_Argv(1), NULL );
	} else if ( c == 3 ) {
		si.StartBackgroundTrack( Cmd_Argv(1), Cmd_Argv(2) );
	} else {
		Com_Printf ("music <musicfile> [loopfile]\n");
		return;
	}

}

/*
=================
S_Music_f
=================
*/
void S_StopMusic_f( void )
{
	if(!si.StopBackgroundTrack)
		return;

	si.StopBackgroundTrack();
}


//=============================================================================

#ifdef IOQ3ZTM // MUSIC_SCRIPTS
typedef struct
{
	char name[MAX_QPATH];

	char intro[MAX_QPATH];
	char loop[MAX_QPATH];

	float volume; // Multiply by normal volume

} musicDef_t;

#define MAX_MUSICDEFS 128
musicDef_t musicDefs[MAX_MUSICDEFS];

void S_GetMusicForIntro(char *intro, char *loop, float *volume)
{
	int i;
	char name[MAX_QPATH];

	if (!intro || !*intro) {
		return;
	}

	// if loop is specified don't use music script?
	//if (loop && *loop && strcmp(intro, loop) != 0) {
	//	return;
	//}

	COM_StripExtension(intro, name, sizeof(name));

	for (i = 0; i < MAX_MUSICDEFS; i++)
	{
		if (!musicDefs[i].name[0]) {
			break;
		}

		if (Q_stricmp(musicDefs[i].name, name) == 0) {
			strncpy(intro, musicDefs[i].intro, MAX_QPATH);

			if (musicDefs[i].loop[0]) {
				strncpy(loop, musicDefs[i].loop, MAX_QPATH);
			} else {
				strncpy(loop, intro, MAX_QPATH);
			}

			if (volume) {
				*volume = musicDefs[i].volume;
			}
			break;
		}
	}
}

void S_LoadMusicFile(const char *filename)
{
	char *buffer;
	char *p;
	char *token;
	musicDef_t *musicDef;
	int i;

	FS_ReadFile( filename, (void **)&buffer );

	if ( !buffer ) {
		Com_Printf("Warning: Couldn't load %s", filename );
		return;
	}

	p = buffer;

	while (1)
	{
		musicDef = NULL;
		for (i = 0; i < MAX_MUSICDEFS; i++)
		{
			if (!musicDefs[i].name[0]) {
				musicDef = &musicDefs[i];
				break;
			}
		}

		if (!musicDef) {
			Com_Printf("Warning: Out of musicDefs in %s!\n", filename);
			return;
		}

		token = COM_ParseExt( &p, qtrue );
		if ( token[0] == 0 ) {
			break;
		}

		// token is name
		COM_StripExtension(token, musicDef->name, sizeof(musicDef->name));

		token = COM_ParseExt(&p, qtrue);
		if(token[0] != '{' && token[1] != '\0')
		{
			Com_Printf("%s:%s Missing begining bracket\n", filename, musicDef->name);
			break;
		}

		// Set defaults
		musicDef->volume = 1.0f;

		// Parse keywords
		while (1)
		{
			token = COM_ParseExt( &p, qtrue );
			if ( token[0] == 0 ) {
				Com_Printf("%s:%s Missing ending bracket!\n", filename, musicDef->name);
				break;
			}
			else if(token[0] == '}' && token[1] == '\0')
			{
				// end of musicdef
				break;
			}

			if ( Q_stricmp(token, "intro") == 0 ) {
				token = COM_ParseExt( &p, qtrue );
				if ( token[0] == 0 ) {
					Com_Printf("%s:%s Missing ending bracket!\n", filename, musicDef->name);
					break;
				}

				Q_strncpyz(musicDef->intro, token, sizeof(musicDef->intro));
				continue;
			}
			else if ( Q_stricmp(token, "loop") == 0 ) {
				token = COM_ParseExt( &p, qtrue );
				if ( token[0] == 0 ) {
					Com_Printf("%s:%s Missing ending bracket!\n", filename, musicDef->name);
					break;
				}

				Q_strncpyz(musicDef->loop, token, sizeof(musicDef->loop));
				continue;
			}
			else if ( Q_stricmp(token, "volume") == 0 ) {
				token = COM_ParseExt( &p, qtrue );
				if ( token[0] == 0 ) {
					Com_Printf("%s:%s Missing ending bracket!\n", filename, musicDef->name);
					break;
				}

				// Limit to 0.1 to 3.0...
				musicDef->volume = Com_Clamp(0.1f, 3.0f, atof(token));
				continue;
			}

			Com_Printf("%s:%s Unknown token %s!\n", filename, musicDef->name, token);
		}
		Com_DPrintf("Loaded %s from %s!\n",  musicDef->name, filename);
	}
}

/*
====================
ScanAndLoadShaderFiles

Init music scripts. Finds and loads all .music files
=====================
*/
#define	MAX_MUSIC_FILES	4096
void S_InitMusicDefs(void)
{
	char **musicFiles;
	int numMusicFiles;
	int i;

	// Clean memory
	memset(musicDefs, 0, sizeof(musicDefs));

	// Load from music/musiclist.txt and music/*.music
	S_LoadMusicFile("music/musiclist.txt");

	// scan for music files
	musicFiles = FS_ListFiles( "music", ".music", &numMusicFiles );

	if ( !musicFiles || !numMusicFiles ) {
		// Music files are optional
		return;
	}

	if ( numMusicFiles > MAX_MUSIC_FILES ) {
		numMusicFiles = MAX_MUSIC_FILES;
	}

	// load and parse shader files
	for ( i = 0; i < numMusicFiles; i++ )
	{
		char filename[MAX_QPATH];

		Com_sprintf( filename, sizeof( filename ), "music/%s", musicFiles[i] );
		S_LoadMusicFile(filename);
	}
}
#endif

/*
=================
S_Init
=================
*/
void S_Init( void )
{
	cvar_t		*cv;
	qboolean	started = qfalse;

	Com_Printf( "------ Initializing Sound ------\n" );

	s_volume = Cvar_Get( "s_volume", "0.8", CVAR_ARCHIVE );
#ifdef TA_MAIN // ZTM: Increase music volume, same as missionpack now
	s_musicVolume = Cvar_Get( "s_musicvolume", "0.5", CVAR_ARCHIVE );
#else
	s_musicVolume = Cvar_Get( "s_musicvolume", "0.25", CVAR_ARCHIVE );
#endif
	s_muted = Cvar_Get("s_muted", "0", CVAR_ROM);
	s_doppler = Cvar_Get( "s_doppler", "1", CVAR_ARCHIVE );
	s_backend = Cvar_Get( "s_backend", "", CVAR_ROM );
	s_muteWhenMinimized = Cvar_Get( "s_muteWhenMinimized", "0", CVAR_ARCHIVE );
	s_muteWhenUnfocused = Cvar_Get( "s_muteWhenUnfocused", "0", CVAR_ARCHIVE );

	cv = Cvar_Get( "s_initsound", "1", 0 );
	if( !cv->integer ) {
		Com_Printf( "Sound disabled.\n" );
	} else {

		S_CodecInit( );

		Cmd_AddCommand( "play", S_Play_f );
		Cmd_AddCommand( "music", S_Music_f );
		Cmd_AddCommand( "stopmusic", S_StopMusic_f );
		Cmd_AddCommand( "s_list", S_SoundList );
		Cmd_AddCommand( "s_stop", S_StopAllSounds );
		Cmd_AddCommand( "s_info", S_SoundInfo );

		cv = Cvar_Get( "s_useOpenAL", "1", CVAR_ARCHIVE );
		if( cv->integer ) {
			//OpenAL
			started = S_AL_Init( &si );
			Cvar_Set( "s_backend", "OpenAL" );
		}

		if( !started ) {
			started = S_Base_Init( &si );
			Cvar_Set( "s_backend", "base" );
		}

		if( started ) {
			if( !S_ValidSoundInterface( &si ) ) {
				Com_Error( ERR_FATAL, "Sound interface invalid" );
			}

			S_SoundInfo( );
			Com_Printf( "Sound initialization successful.\n" );
		} else {
			Com_Printf( "Sound initialization failed.\n" );
		}

#ifdef IOQ3ZTM // MUSIC_SCRIPTS
		S_InitMusicDefs();
#endif
	}

	Com_Printf( "--------------------------------\n");
}

/*
=================
S_Shutdown
=================
*/
void S_Shutdown( void )
{
	if( si.Shutdown ) {
		si.Shutdown( );
	}

	Com_Memset( &si, 0, sizeof( soundInterface_t ) );

	Cmd_RemoveCommand( "play" );
	Cmd_RemoveCommand( "music");
	Cmd_RemoveCommand( "stopmusic");
	Cmd_RemoveCommand( "s_list" );
	Cmd_RemoveCommand( "s_stop" );
	Cmd_RemoveCommand( "s_info" );

	S_CodecShutdown( );
}

