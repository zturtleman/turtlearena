/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

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
//
#include "ui_local.h"

#define ART_FRAMEL			"menu/art/frame2_l"
#define ART_FRAMER			"menu/art/frame1_r"
#define ART_MODEL0			"menu/art/model_0"
#define ART_MODEL1			"menu/art/model_1"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"
#define ART_FX_BASE			"menu/art/fx_base"
#define ART_FX_BLUE			"menu/art/fx_blue"
#ifdef TA_DATA
#define ART_FX_MAGENTA		"menu/art/fx_magenta"
#else
#define ART_FX_MAGENTA		"menu/art/fx_cyan"
#endif
#define ART_FX_GREEN		"menu/art/fx_grn"
#define ART_FX_RED			"menu/art/fx_red"
#ifdef TA_DATA
#define ART_FX_CYAN			"menu/art/fx_cyan"
#else
#define ART_FX_CYAN			"menu/art/fx_teal"
#endif
#define ART_FX_WHITE		"menu/art/fx_white"
#define ART_FX_YELLOW		"menu/art/fx_yel"
#ifdef IOQ3ZTM // MORE_COLOR_EFFECTS
#define ART_FX_ORANGE		"menu/art/fx_orange"
#define ART_FX_LIME			"menu/art/fx_lime"
#define ART_FX_VIVIDGREEN	"menu/art/fx_vividgreen"
#define ART_FX_LIGHTBLUE	"menu/art/fx_lightblue"
#define ART_FX_PURPLE		"menu/art/fx_purple"
#define ART_FX_PINK			"menu/art/fx_pink"
#endif

#define ID_NAME			10
#define ID_HANDICAP		11
#define ID_EFFECTS		12
#define ID_BACK			13
#define ID_MODEL		14
#ifdef IOQ3ZTM // UI_COLOR2
#define ID_EFFECTS2		15
#endif

#define MAX_NAMELENGTH	20

#ifdef IOQ3ZTM // MORE_COLOR_EFFECTS
#define NUM_COLOR_EFFECTS 13
#else
#define NUM_COLOR_EFFECTS 7
#endif


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;
	menubitmap_s		player;

	menufield_s			name;
	menulist_s			handicap;
	menulist_s			effects;
#ifdef IOQ3ZTM // UI_COLOR2
	menulist_s			effects2;
#endif

	menubitmap_s		back;
	menubitmap_s		model;
	menubitmap_s		item_null;

	qhandle_t			fxBasePic;
	qhandle_t			fxPic[NUM_COLOR_EFFECTS];
	playerInfo_t		playerinfo;
	int					current_fx;
	char				playerModel[MAX_QPATH];
#ifdef TA_SPLITVIEW
	int					localClient;
#endif
} playersettings_t;

static playersettings_t	s_playersettings;

#ifdef IOQ3ZTM // MORE_COLOR_EFFECTS
static int gamecodetoui[] = {8,4,6,0,10,2,12,1,3,5,7,9,11};
static int uitogamecode[] = {4,8,6,9,2,10,3,11,1,12,5,13,7};
#else
static int gamecodetoui[] = {4,2,3,0,5,1,6};
static int uitogamecode[] = {4,6,2,3,1,5,7};
#endif

static const char *handicap_items[] = {
	"None",
	"95",
	"90",
	"85",
	"80",
	"75",
	"70",
	"65",
	"60",
	"55",
	"50",
	"45",
	"40",
	"35",
	"30",
	"25",
	"20",
	"15",
	"10",
	"5",
	NULL
};


/*
=================
PlayerSettings_DrawName
=================
*/
static void PlayerSettings_DrawName( void *self ) {
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			*txt;
	char			c;
	float			*color;
	int				n;
	int				basex, x, y;
	char			name[32];
	int				i;

	f = (menufield_s*)self;
	basex = f->generic.x;
	y = f->generic.y;
	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
#ifndef TURTLEARENA
		color = text_color_highlight;
#endif
	}

	UI_DrawProportionalString( basex, y, "Name", style, color );

	// draw the actual name
	basex += 64;
	y += PROP_HEIGHT;
	txt = f->field.buffer;
	color = g_color_table[ColorIndex(COLOR_WHITE)];
	x = basex;
	for (i = 0; i < strlen(f->field.buffer)+1; i++) {
		if ( (c = *txt) != 0 ) {
			if ( !focus && Q_IsColorString( txt ) ) {
				n = ColorIndex( *(txt+1) );
				if( n == 0 ) {
					n = 7;
				}
				color = g_color_table[n];
				txt += 2;
				continue;
			}
			x += UI_DrawChar( x, y, c, style, color );
			txt++;
		}
#ifdef IOQ3ZTM // FONT_REWITE
		// draw cursor if we have focus
		if( focus && f->field.cursor == i) {
			if ( trap_Key_GetOverstrikeMode() ) {
				c = 11;
			} else {
				c = 10;
			}

			UI_DrawChar( basex, y, c, (style & ~UI_PULSE) | UI_BLINK, color_white );
		}
		basex = x;
#endif
	}

#ifndef IOQ3ZTM // FONT_REWITE
	// draw cursor if we have focus
	if( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}
#endif

	// draw at bottom also using proportional font
	Q_strncpyz( name, f->field.buffer, sizeof(name) );
	Q_CleanStr( name );
	UI_DrawProportionalString( 320, 440, name, UI_CENTER|UI_BIGFONT, text_color_normal );
}


/*
=================
PlayerSettings_DrawHandicap
=================
*/
static void PlayerSettings_DrawHandicap( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
#ifndef TURTLEARENA
		color = text_color_highlight;
#endif
	}

	UI_DrawProportionalString( item->generic.x, item->generic.y, "Handicap", style, color );
	UI_DrawProportionalString( item->generic.x + 64, item->generic.y + PROP_HEIGHT, handicap_items[item->curvalue], style, color );
}


/*
=================
PlayerSettings_DrawEffects
=================
*/
static void PlayerSettings_DrawEffects( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus
#ifdef IOQ3ZTM // UI_COLOR2
		// ZTM: Hacky: Have effects focus if effects2 is selected (they share the same header)
		|| (item->generic.id == ID_EFFECTS && ((menulist_s *)item->generic.parent->items[item->generic.parent->cursor])->generic.id == ID_EFFECTS2)
#endif
		)
	{
		style |= UI_PULSE;
#ifndef TURTLEARENA
		color = text_color_highlight;
#endif
	}

#ifdef IOQ3ZTM // UI_COLOR2
	// Effects2 doesn't need it's own header.
	if (item->generic.id != ID_EFFECTS2)
#endif
	UI_DrawProportionalString( item->generic.x, item->generic.y, "Effects", style, color );

	UI_DrawHandlePic( item->generic.x + 64, item->generic.y + PROP_HEIGHT + 8, 128, 8, s_playersettings.fxBasePic );
#ifdef IOQ3ZTM // MORE_COLOR_EFFECTS
	UI_DrawHandlePic( item->generic.x + 64 + item->curvalue * 9 + 4, item->generic.y + PROP_HEIGHT + 6, 16, 12, s_playersettings.fxPic[item->curvalue] );
#else
	UI_DrawHandlePic( item->generic.x + 64 + item->curvalue * 16 + 8, item->generic.y + PROP_HEIGHT + 6, 16, 12, s_playersettings.fxPic[item->curvalue] );
#endif
#ifdef IOQ3ZTM // UI_COLOR2
	if (focus) {
		float color[4];

		if (item->curvalue == 1) {
			// In Turtle Arena, don't draw orange on orange. In Quake3 don't draw yellow on yellow.
			color[0] = color[1] = color[2] = 1.0f; // white
		} else {
			VectorCopy(text_color_highlight, color); // orange
		}

		color[3] = 0.5 + 0.5 * sin( uis.realtime / PULSE_DIVISOR );

		// MORE_COLOR_EFFECTS
		// UI_DrawRect, but width and height are 2.
		// top
		UI_FillRect( item->generic.x + 64 + item->curvalue * 9 + 4, item->generic.y + PROP_HEIGHT + 6, 16, 2, color );
		// bottom
		UI_FillRect( item->generic.x + 64 + item->curvalue * 9 + 4, item->generic.y + PROP_HEIGHT + 6 + 10, 16, 2, color );
		// left
		UI_FillRect( item->generic.x + 64 + item->curvalue * 9 + 4, item->generic.y + PROP_HEIGHT + 6 + 2, 2, 10, color );
		// right
		UI_FillRect( item->generic.x + 64 + item->curvalue * 9 + 4 + 14, item->generic.y + PROP_HEIGHT + 6 + 2, 2, 10, color );
	}
#endif
}


/*
=================
PlayerSettings_DrawPlayer
=================
*/
static void PlayerSettings_DrawPlayer( void *self ) {
	menubitmap_s	*b;
	vec3_t			viewangles;
	char			buf[MAX_QPATH];

#ifdef TA_SPLITVIEW // #_model
	if (s_playersettings.localClient != 0) {
		trap_Cvar_VariableStringBuffer( va("%d_model", s_playersettings.localClient+1), buf, sizeof( buf ) );
	} else
#endif
	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
	if ( strcmp( buf, s_playersettings.playerModel ) != 0 ) {
		UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, buf );
		strcpy( s_playersettings.playerModel, buf );

		viewangles[YAW]   = 180 - 30;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
#ifdef TA_WEAPSYS
		UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo,
				BG_LegsStandForWeapon(&s_playersettings.playerinfo.playercfg, s_playersettings.playerinfo.weapon),
				BG_TorsoStandForWeapon(s_playersettings.playerinfo.weapon), viewangles, vec3_origin,
				s_playersettings.playerinfo.weapon, qfalse );
#else
		UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
#endif
	}

	b = (menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_playersettings.playerinfo, uis.realtime/2 );
}


/*
=================
PlayerSettings_SaveChanges
=================
*/
static void PlayerSettings_SaveChanges( void ) {
#ifdef TA_SPLITVIEW // #_name, #_handicap, #_color1, #_color2
	if (s_playersettings.localClient != 0) {
		// name
		trap_Cvar_Set( va("%d_name", s_playersettings.localClient+1), s_playersettings.name.field.buffer );

		// handicap
		trap_Cvar_SetValue( va("%d_handicap", s_playersettings.localClient+1), 100 - s_playersettings.handicap.curvalue * 5 );

		// effects color
		trap_Cvar_SetValue( va("%d_color1", s_playersettings.localClient+1), uitogamecode[s_playersettings.effects.curvalue] );
#ifdef IOQ3ZTM // UI_COLOR2
		trap_Cvar_SetValue( va("%d_color2", s_playersettings.localClient+1), uitogamecode[s_playersettings.effects2.curvalue] );
#endif
		return;
	}
#endif

	// name
	trap_Cvar_Set( "name", s_playersettings.name.field.buffer );

	// handicap
	trap_Cvar_SetValue( "handicap", 100 - s_playersettings.handicap.curvalue * 5 );

	// effects color
	trap_Cvar_SetValue( "color1", uitogamecode[s_playersettings.effects.curvalue] );
#ifdef IOQ3ZTM // UI_COLOR2
	trap_Cvar_SetValue( "color2", uitogamecode[s_playersettings.effects2.curvalue] );
#endif
}


/*
=================
PlayerSettings_MenuKey
=================
*/
static sfxHandle_t PlayerSettings_MenuKey( int key ) {
	if(
#ifndef TA_MISC // MENU: Right Mouse button = left arrow
		key == K_MOUSE2 ||
#endif
		key == K_ESCAPE ) {
		PlayerSettings_SaveChanges();
	}
	return Menu_DefaultKey( &s_playersettings.menu, key );
}


/*
=================
PlayerSettings_SetMenuItems
=================
*/
static void PlayerSettings_SetMenuItems( void ) {
	vec3_t	viewangles;
	int		c;
	int		h;
#ifdef TA_SPLITVIEW
	char	buf[64];
#endif

	// name
#ifdef TA_SPLITVIEW // #_name
	if (s_playersettings.localClient != 0) {
		Q_snprintf(buf, sizeof (buf), "%d_name", s_playersettings.localClient+1);
		Q_strncpyz( s_playersettings.name.field.buffer, UI_Cvar_VariableString(buf), sizeof(s_playersettings.name.field.buffer) );
	} else
#endif
	Q_strncpyz( s_playersettings.name.field.buffer, UI_Cvar_VariableString("name"), sizeof(s_playersettings.name.field.buffer) );

	// effects color
#ifdef TA_SPLITVIEW // #_color1
	if (s_playersettings.localClient != 0) {
		Q_snprintf(buf, sizeof (buf), "%d_color1", s_playersettings.localClient+1);
		c = trap_Cvar_VariableValue( buf ) - 1;
	} else
#endif
	c = trap_Cvar_VariableValue( "color1" ) - 1;
	if( c < 0 || c > NUM_COLOR_EFFECTS-1 ) {
		c = NUM_COLOR_EFFECTS-1;
	}
	s_playersettings.effects.curvalue = gamecodetoui[c];

#ifdef IOQ3ZTM // UI_COLOR2
#ifdef TA_SPLITVIEW // #_color2
	if (s_playersettings.localClient != 0) {
		Q_snprintf(buf, sizeof (buf), "%d_color2", s_playersettings.localClient+1);
		c = trap_Cvar_VariableValue( buf ) - 1;
	} else
#endif
	c = trap_Cvar_VariableValue( "color2" ) - 1;
	if( c < 0 || c > NUM_COLOR_EFFECTS-1 ) {
		c = NUM_COLOR_EFFECTS-1;
	}
	s_playersettings.effects2.curvalue = gamecodetoui[c];
#endif

	// model/skin
	memset( &s_playersettings.playerinfo, 0, sizeof(playerInfo_t) );
	
	viewangles[YAW]   = 180 - 30;
	viewangles[PITCH] = 0;
	viewangles[ROLL]  = 0;

#ifdef TA_SPLITVIEW // #_model
	if (s_playersettings.localClient != 0) {
		Q_snprintf(buf, sizeof (buf), "%d_model", s_playersettings.localClient+1);
		UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, UI_Cvar_VariableString( buf ) );
	} else
#endif
	UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, UI_Cvar_VariableString( "model" ) );
#ifdef TA_WEAPSYS
	UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo,
			BG_LegsStandForWeapon(&s_playersettings.playerinfo.playercfg, s_playersettings.playerinfo.weapon),
			BG_TorsoStandForWeapon(s_playersettings.playerinfo.weapon), viewangles, vec3_origin,
			s_playersettings.playerinfo.weapon, qfalse );
#else
	UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
#endif

	// handicap
#ifdef TA_SPLITVIEW // #_handicap
	if (s_playersettings.localClient != 0) {
		Q_snprintf(buf, sizeof (buf), "%d_handicap", s_playersettings.localClient+1);
		h = Com_Clamp( 5, 100, trap_Cvar_VariableValue(buf) );
	} else
#endif
	h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
	s_playersettings.handicap.curvalue = 20 - h / 5;
}


/*
=================
PlayerSettings_MenuEvent
=================
*/
static void PlayerSettings_MenuEvent( void* ptr, int event ) {
#ifdef TA_SPLITVIEW
	char	buf[64];
#endif

	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_HANDICAP:
#ifdef TA_SPLITVIEW // #_handicap
		if (s_playersettings.localClient != 0) {
			Q_snprintf(buf, sizeof (buf), "%d_handicap", s_playersettings.localClient+1);
			trap_Cvar_Set( "buf", va( "%i", 100 - 25 * s_playersettings.handicap.curvalue ) );
		} else
#endif
		trap_Cvar_Set( "handicap", va( "%i", 100 - 25 * s_playersettings.handicap.curvalue ) );
		break;

	case ID_MODEL:
		PlayerSettings_SaveChanges();
#ifdef TA_SPLITVIEW
		UI_PlayerModelMenu(s_playersettings.localClient);
#else
		UI_PlayerModelMenu();
#endif
		break;

	case ID_BACK:
		PlayerSettings_SaveChanges();
		UI_PopMenu();
		break;
	}
}


/*
=================
PlayerSettings_MenuInit
=================
*/
#ifdef TA_SPLITVIEW
static void PlayerSettings_MenuInit( int localClient )
#else
static void PlayerSettings_MenuInit( void )
#endif
{
	int		y;

	memset(&s_playersettings,0,sizeof(playersettings_t));

#ifdef TA_SPLITVIEW
	s_playersettings.localClient = localClient;
#endif

	PlayerSettings_Cache();

	s_playersettings.menu.key        = PlayerSettings_MenuKey;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.fullscreen = qtrue;

	s_playersettings.banner.generic.type  = MTYPE_BTEXT;
	s_playersettings.banner.generic.x     = 320;
	s_playersettings.banner.generic.y     = 16;
#ifdef TA_SPLITVIEW
	switch (s_playersettings.localClient)
	{
		case 0:
			s_playersettings.banner.string        = "PLAYER 1 SETTINGS";
			break;
		case 1:
			s_playersettings.banner.string        = "PLAYER 2 SETTINGS";
			break;
		case 2:
			s_playersettings.banner.string        = "PLAYER 3 SETTINGS";
			break;
		case 3:
			s_playersettings.banner.string        = "PLAYER 4 SETTINGS";
			break;
		default:
			s_playersettings.banner.string        = "PLAYER SETTINGS";
			break;
	}
#else
	s_playersettings.banner.string        = "PLAYER SETTINGS";
#endif
	s_playersettings.banner.color         = text_banner_color;
	s_playersettings.banner.style         = UI_CENTER;

	s_playersettings.framel.generic.type  = MTYPE_BITMAP;
	s_playersettings.framel.generic.name  = ART_FRAMEL;
	s_playersettings.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framel.generic.x     = 0;
	s_playersettings.framel.generic.y     = 78;
	s_playersettings.framel.width         = 256;
	s_playersettings.framel.height        = 329;

	s_playersettings.framer.generic.type  = MTYPE_BITMAP;
	s_playersettings.framer.generic.name  = ART_FRAMER;
	s_playersettings.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framer.generic.x     = 376;
	s_playersettings.framer.generic.y     = 76;
	s_playersettings.framer.width         = 256;
	s_playersettings.framer.height        = 334;

	y = 144;
	s_playersettings.name.generic.type			= MTYPE_FIELD;
	s_playersettings.name.generic.flags			= QMF_NODEFAULTINIT;
	s_playersettings.name.generic.ownerdraw		= PlayerSettings_DrawName;
	s_playersettings.name.field.widthInChars	= MAX_NAMELENGTH;
	s_playersettings.name.field.maxchars		= MAX_NAMELENGTH;
	s_playersettings.name.generic.x				= 192;
	s_playersettings.name.generic.y				= y;
	s_playersettings.name.generic.left			= 192 - 8;
	s_playersettings.name.generic.top			= y - 8;
	s_playersettings.name.generic.right			= 192 + 200;
	s_playersettings.name.generic.bottom		= y + 2 * PROP_HEIGHT;

	y += 3 * PROP_HEIGHT;
	s_playersettings.handicap.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.handicap.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.handicap.generic.id		= ID_HANDICAP;
	s_playersettings.handicap.generic.ownerdraw	= PlayerSettings_DrawHandicap;
	s_playersettings.handicap.generic.x			= 192;
	s_playersettings.handicap.generic.y			= y;
	s_playersettings.handicap.generic.left		= 192 - 8;
	s_playersettings.handicap.generic.top		= y - 8;
	s_playersettings.handicap.generic.right		= 192 + 200;
	s_playersettings.handicap.generic.bottom	= y + 2 * PROP_HEIGHT;
	s_playersettings.handicap.numitems			= 20;

	y += 3 * PROP_HEIGHT;
	s_playersettings.effects.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects.generic.id			= ID_EFFECTS;
	s_playersettings.effects.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects.generic.x			= 192;
	s_playersettings.effects.generic.y			= y;
	s_playersettings.effects.generic.left		= 192 - 8;
	s_playersettings.effects.generic.top		= y - 8;
	s_playersettings.effects.generic.right		= 192 + 200;
	s_playersettings.effects.generic.bottom		= y + 2* PROP_HEIGHT;
	s_playersettings.effects.numitems			= NUM_COLOR_EFFECTS;

#ifdef IOQ3ZTM // UI_COLOR2
	y += 1 * PROP_HEIGHT;
	s_playersettings.effects2.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects2.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects2.generic.id		= ID_EFFECTS2;
	s_playersettings.effects2.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects2.generic.x			= 192;
	s_playersettings.effects2.generic.y			= y;
	s_playersettings.effects2.generic.left		= 192 - 8;
	s_playersettings.effects2.generic.top		= y - 8;
	s_playersettings.effects2.generic.right		= 192 + 200;
	s_playersettings.effects2.generic.bottom	= y + 2* PROP_HEIGHT;
	s_playersettings.effects2.numitems			= NUM_COLOR_EFFECTS;
#endif

	s_playersettings.model.generic.type			= MTYPE_BITMAP;
	s_playersettings.model.generic.name			= ART_MODEL0;
	s_playersettings.model.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.model.generic.id			= ID_MODEL;
	s_playersettings.model.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.model.generic.x			= 640;
	s_playersettings.model.generic.y			= 480-64;
	s_playersettings.model.width				= 128;
	s_playersettings.model.height				= 64;
	s_playersettings.model.focuspic				= ART_MODEL1;

	s_playersettings.player.generic.type		= MTYPE_BITMAP;
	s_playersettings.player.generic.flags		= QMF_INACTIVE;
	s_playersettings.player.generic.ownerdraw	= PlayerSettings_DrawPlayer;
	s_playersettings.player.generic.x			= 400;
	s_playersettings.player.generic.y			= -40;
	s_playersettings.player.width				= 32*10;
	s_playersettings.player.height				= 56*10;

	s_playersettings.back.generic.type			= MTYPE_BITMAP;
	s_playersettings.back.generic.name			= ART_BACK0;
	s_playersettings.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.back.generic.id			= ID_BACK;
	s_playersettings.back.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.back.generic.x				= 0;
	s_playersettings.back.generic.y				= 480-64;
	s_playersettings.back.width					= 128;
	s_playersettings.back.height				= 64;
	s_playersettings.back.focuspic				= ART_BACK1;

	s_playersettings.item_null.generic.type		= MTYPE_BITMAP;
	s_playersettings.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_playersettings.item_null.generic.x		= 0;
	s_playersettings.item_null.generic.y		= 0;
	s_playersettings.item_null.width			= 640;
	s_playersettings.item_null.height			= 480;

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.banner );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framel );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framer );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.name );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.handicap );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects );
#ifdef IOQ3ZTM // UI_COLOR2
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects2 );
#endif
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.model );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.back );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.player );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.item_null );

	PlayerSettings_SetMenuItems();
}


/*
=================
PlayerSettings_Cache
=================
*/
void PlayerSettings_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_MODEL0 );
	trap_R_RegisterShaderNoMip( ART_MODEL1 );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );

	s_playersettings.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
#ifdef IOQ3ZTM // MORE_COLOR_EFFECTS
	s_playersettings.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	s_playersettings.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_ORANGE );
	s_playersettings.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	s_playersettings.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_LIME );
	s_playersettings.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	s_playersettings.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_VIVIDGREEN );
	s_playersettings.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	s_playersettings.fxPic[7] = trap_R_RegisterShaderNoMip( ART_FX_LIGHTBLUE );
	s_playersettings.fxPic[8] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	s_playersettings.fxPic[9] = trap_R_RegisterShaderNoMip( ART_FX_PURPLE );
	s_playersettings.fxPic[10] = trap_R_RegisterShaderNoMip( ART_FX_MAGENTA );
	s_playersettings.fxPic[11] = trap_R_RegisterShaderNoMip( ART_FX_PINK );
	s_playersettings.fxPic[12] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
#else
	s_playersettings.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	s_playersettings.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	s_playersettings.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	s_playersettings.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	s_playersettings.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	s_playersettings.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_MAGENTA );
	s_playersettings.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
#endif
}


/*
=================
UI_PlayerSettingsMenu
=================
*/
#ifdef TA_SPLITVIEW
void UI_PlayerSettingsMenu( int localClient )
#else
void UI_PlayerSettingsMenu( void )
#endif
{
#ifdef TA_SPLITVIEW
	PlayerSettings_MenuInit(localClient);
#else
	PlayerSettings_MenuInit();
#endif
	UI_PushMenu( &s_playersettings.menu );
}
