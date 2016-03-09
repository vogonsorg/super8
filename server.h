/*  Copyright (C) 1996-1997 Id Software, Inc.
    Copyright (C) 1999-2012 other authors as noted in code comments

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   */
// server.h

typedef struct
{
    int			maxclients;
    int			maxclientslimit;
    struct client_s	*clients;		// [maxclients]
    int			serverflags;		// episode completion information
    qboolean	changelevel_issued;	// cleared when at SV_SpawnServer
} server_static_t;

//=============================================================================

typedef enum {ss_loading, ss_active} server_state_t;

typedef struct
{
    qboolean	active;				// false if only a net client

    qboolean	paused;
    qboolean	loadgame;			// handle connections specially

    double		time;

    int			lastcheck;			// used by PF_checkclient
    double		lastchecktime;

    char		name[64];			// map name
    char		modelname[64];		// maps/<name>.bsp, for model_precache[0]
    struct model_s 	*worldmodel;
    char		*model_precache[MAX_MODELS];	// NULL terminated
    struct model_s	*models[MAX_MODELS];
    char		*sound_precache[MAX_SOUNDS];	// NULL terminated
    char		*lightstyles[MAX_LIGHTSTYLES];
    int			num_edicts;
    edict_t		*edicts;			// can NOT be array indexed, because
    // edict_t is variable sized, but can
    // be used to reference the world ent
    server_state_t	state;			// some actions are only valid during load

    sizebuf_t	datagram;
    byte		datagram_buf[MAX_DATAGRAM];

    sizebuf_t	reliable_datagram;	// copied to all clients at end of frame
    byte		reliable_datagram_buf[MAX_DATAGRAM];

    sizebuf_t	signon;
    byte		signon_buf[MAX_MSGLEN-2]; //qb:  johnfitz -- was 8192, now uses MAX_MSGLEN
    unsigned	protocol; //qb: johnfitz
} server_t;


#define	NUM_PING_TIMES		16
#define	NUM_SPAWN_PARMS		16

typedef struct client_s
{
    qboolean		active;				// false = client is free
    qboolean		spawned;			// false = don't send datagrams
    qboolean		dropasap;			// has been told to go to another level
    qboolean		privileged;			// can execute any host command
    qboolean		sendsignon;			// only valid before spawned

    double			last_message;		// reliable messages must be sent
    // periodically

    struct qsocket_s *netconnection;	// communications handle

    usercmd_t		cmd;				// movement
    vec3_t			wishdir;			// intended motion calced from cmd

    sizebuf_t		message;			// can be added to at any time,
    // copied and clear once per frame
    byte			msgbuf[MAX_MSGLEN];
    edict_t			*edict;				// EDICT_NUM(clientnum+1)
    char			name[32];			// for printing to other people
    int				colors;

    float			ping_times[NUM_PING_TIMES];
    int				num_pings;			// ping_times[num_pings%NUM_PING_TIMES]

// spawn parms are carried from level to level
    float			spawn_parms[NUM_SPAWN_PARMS];

// client known data for deltas
    int				old_frags;
} client_t;


//=============================================================================

// edict->movetype values
#define	MOVETYPE_NONE			0		// never moves
#define	MOVETYPE_ANGLENOCLIP	1
#define	MOVETYPE_ANGLECLIP		2
#define	MOVETYPE_WALK			3		// gravity
#define	MOVETYPE_STEP			4		// gravity, special edge handling
#define	MOVETYPE_FLY			5
#define	MOVETYPE_TOSS			6		// gravity
#define	MOVETYPE_PUSH			7		// no clip to world, push and crush
#define	MOVETYPE_NOCLIP			8
#define	MOVETYPE_FLYMISSILE		9		// extra size to monsters
#define	MOVETYPE_BOUNCE			10
//qb: LHmovetypes begin.  These were ifdef Q2, plus LH improvements.
#define MOVETYPE_BOUNCEMISSILE	11		// bounce w/o gravity
#define MOVETYPE_FOLLOW			12		// track movement of aiment
//qb: LHmovetypes end

// edict->solid values
#define	SOLID_NOT				0		// no interaction with other objects
#define	SOLID_TRIGGER			1		// touch on edge, but not blocking
#define	SOLID_BBOX				2		// touch on edge, block
#define	SOLID_SLIDEBOX			3		// touch on edge, but not an onground
#define	SOLID_BSP				4		// bsp clip, touch on edge, block

// edict->deadflag values
#define	DEAD_NO					0
#define	DEAD_DYING				1
#define	DEAD_DEAD				2

#define	DAMAGE_NO				0
#define	DAMAGE_YES				1
#define	DAMAGE_AIM				2

// edict->flags
#define	FL_FLY					1
#define	FL_SWIM					2
//#define	FL_GLIMPSE				4
//#define	FL_CONVEYOR				4  //qb: open slot
#define	FL_CLIENT				8
#define	FL_INWATER				16
#define	FL_MONSTER				32
#define	FL_GODMODE				64
#define	FL_NOTARGET				128
#define	FL_ITEM					256
#define	FL_ONGROUND				512
#define	FL_PARTIALGROUND		1024	// not all corners are valid
#define	FL_WATERJUMP			2048	// player jumping out of water
#define	FL_JUMPRELEASED			4096	// for jump debouncing
//qb: TODO how many

// entity effects
//qb: TODO clean this up, add/delete effects

#define	EF_BRIGHTFIELD			1
#define	EF_MUZZLEFLASH 			2
#define	EF_BRIGHTLIGHT 			4
#define	EF_DIMLIGHT 			8
#define	EF_NODRAW				16
#define EF_ADDITIVE				32		// LordHavoc: Additive Rendering (qb: looks ghostly!)
//#define ???                   128
#define MOD_NOLERP              256     //qb: from FQ
#define	EF_REFLECTIVE			512		// LordHavoc: shiny metal objects :)
#define EF_FULLBRIGHT			1024		// LordHavoc: fullbright
//#define EF_STARDUST			2048	// LordHavoc: showering sparks
#define EF_NOSHADOW				4096	// LordHavoc: does not cast a shadow
#define EF_DARKFIELD			8192	//qb
//#define ???   			    16384

//qb: changed these to occupy 'unused' slots based on DP standard
#define EF_SHADOW				131072	// Manoel Kasimier - EF_SHADOW
#define EF_CELSHADING			262144	// Manoel Kasimier - EF_CELSHADING

//qb: add modelflags as effects
#define EF_NOMODELFLAGS			8388608		// indicates the model's .effects should be ignored (allows overriding them)
#define EF_ROCKET				16777216	// leave a trail
#define EF_GRENADE				33554432	// leave a trail
#define EF_GIB					67108864	// leave a trail
#define EF_ROTATE				134217728	// rotate (bonus items)
#define EF_TRACER				268435456	// green split trail
#define EF_ZOMGIB				536870912	// small blood trail
#define EF_TRACER2				1073741824	// orange split trail + rotate
#define EF_TRACER3				2147483648	//0x80000000	// purple trail


#define	SPAWNFLAG_NOT_EASY			256
#define	SPAWNFLAG_NOT_MEDIUM		512
#define	SPAWNFLAG_NOT_HARD			1024
#define	SPAWNFLAG_NOT_DEATHMATCH	2048

//============================================================================

extern	cvar_t	teamplay;
extern	cvar_t	skill;
extern	cvar_t	deathmatch;
extern	cvar_t	coop;
extern	cvar_t	fraglimit;
extern	cvar_t	timelimit;

extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server

extern	client_t	*host_client;

extern	jmp_buf 	host_abortserver;

extern	double		host_time;

extern	edict_t		*sv_player;

extern float		scr_centertime_off;

extern qboolean allowcheats;

//===========================================================

void SV_Init (void);

void SV_StartParticle (vec3_t org, vec3_t dir, int color, int count);
void SV_StartSound (edict_t *entity, int channel, char *sample, byte volume,
                    float attenuation);

void SV_DropClient (qboolean crash);

void SV_SendClientMessages (void);
void SV_ClearDatagram (void);

int SV_ModelIndex (char *name);

void SV_SetIdealPitch (void);

void SV_AddUpdates (void);

void SV_ClientThink (void);
void SV_AddClientToServer (struct qsocket_s	*ret);

void SV_ClientPrintf (char *fmt, ...);
void SV_BroadcastPrintf (char *fmt, ...);

void SV_Physics (void);

qboolean SV_CheckBottom (edict_t *ent);
qboolean SV_movestep (edict_t *ent, vec3_t move, qboolean relink);

void SV_WriteClientdataToMessage (edict_t *ent, sizebuf_t *msg);
void MSG_WriteCoord (sizebuf_t *sb, float f);
void SV_MoveToGoal (void);

void SV_CheckForNewClients (void);
void SV_RunClients (void);
void SV_SaveSpawnparms ();
void SV_SpawnServer (char *server);

extern cvar_t	sv_stopspeed;
extern cvar_t	sv_friction;
extern cvar_t	sv_maxvelocity;
extern cvar_t	sv_gravity;
extern cvar_t	sv_nostep;
extern cvar_t	sv_friction;
extern cvar_t	sv_edgefriction;
extern cvar_t	sv_stopspeed;
extern cvar_t	sv_maxspeed;
extern cvar_t	sv_accelerate;
extern cvar_t	sv_idealpitchscale;
extern cvar_t	sv_aim_h; // Manoel Kasimier - horizontal autoaim
extern cvar_t	sv_aim;
extern cvar_t	sv_enable_use_button; // Manoel Kasimier - +USE fix
extern cvar_t   r_palette;
extern cvar_t	sv_novis; //qb - from FQ via Mark V
extern cvar_t	sv_freezephysics; //qb
extern cvar_t   hostname;
extern cvar_t	sv_cheats;  //qb
extern cvar_t	sv_freezephysics;  //qb

