/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
// bg_public.h -- Included by server, client, and botlib (not by QVMs)
//    Instead of scattering defines/enums all over that must be the same
//        in the server, client, and botlib as in the VMs, keep all in one place.

// server only
#define	CS_WARMUP				5		// server time when the match will be restarted

#define	PERS_SCORE				0		// !!! MUST NOT CHANGE, SERVER AND
										// GAME BOTH REFERENCE !!!

#define PMF_FOLLOW			4096	// spectate following another player

// client only
#define	CS_MESSAGE				3		// from the map worldspawn's message field

#define PM_INTERMISSION 5 // bg_misc.h enum pmtype_t
#define PM_SPINTERMISSION 6 // bg_misc.h enum pmtype_t

// botlib only
//NOTE: enum entityType_t in bg_misc.h
//FIXME: these might change
enum {
	ET_GENERAL,
	ET_PLAYER,
	ET_ITEM,
	ET_MISSILE,
	ET_MOVER
};

// server and botlib

// ZTM: Server and botlib need GT_SINGLE_PLAYER and [just] botlib needs GT_TEAM
//             If TA_SP is not defined Client also needs GT_SINGLE_PLAYER and
//               Server also needs GT_FFA

//FIXME: these are game specific

#ifdef TA_SP
//GT_FFA 0						// free for all
#else
#define GT_FFA 0				// free for all
#endif
//GT_TOURNAMENT 1				// one on one tournament
#define GT_SINGLE_PLAYER 2		// single player tournament
#define GT_TEAM 3				// team deathmatch
//GT_CTF 4						// capture the flag

