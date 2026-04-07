#pragma once
#include <map>

///////////////////////////////////////////////////////////////////////////
// Baamageddon - A simple platform game using the PlayBuffer framework.
// Copyright 2020 Sumo Digital Limited
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

enum PlayState
{
	STATE_START = 0,
	STATE_APPEAR,
	STATE_PLAY,
	STATE_DEAD,
	STATE_WAIT
};

//-------------------------------------------------------------------------

enum SheepState
{
	STATE_IDLE = 0,
	STATE_WALKING,
	STATE_AIRBORNE
};

//-------------------------------------------------------------------------

enum SheepDirection
{
	DIRECTION_LEFT = 0,
	DIRECTION_RIGHT = 1
}; 

//-------------------------------------------------------------------------

enum GameObjectType
{
	TYPE_NOONE = -1,
	TYPE_SHEEP,
	TYPE_ISLAND,
	TYPE_CLOUD,
	TYPE_DOUGHNUT,
	TYPE_SPRINKLE,
	TYPE_SPIKES,
	TYPE_SPINNING_BLADE,
	TYPE_MARKER,
	TYPE_BOUNCY_BUSH,
	TYPE_EXIT,
	TYPE_WOLF,
	TYPE_SWINGING_BLADE,
	TYPE_SWINGING_SPIKES,
};

//-------------------------------------------------------------------------

struct Platform
{
	AABB box;
	int platform_id;
	float impactOffset = 0.0f;	// Visual-only vertical dip when player lands
};

enum WolfState
{
	WOLF_IDLE = 0,
	WOLF_WARNING,
	WOLF_POUNCING,
};

//-------------------------------------------------------------------------
// Newton's Cradle: a sorted group of swinging spike blocks sharing a pivot
// height. End blocks swing alternately; middle blocks are static platforms.
struct CradleGroup
{
	std::vector<int> blockIds; // Object IDs sorted left→right by x position
	int   activeEnd    = 0;    // 0 = left block swinging, 1 = right block swinging
	float angle        = 0.f; // Current pendulum angle from vertical (radians)
	float angVel       = 0.f; // Angular velocity (rad/frame)
	float amplitude    = 0.f; // Initial amplitude (set at group creation)
	float chainLen     = 0.f; // Pivot-to-block-centre distance in pixels
	float contactAngle = 0.f; // Angle at which outer block touches adjacent block
};

//-------------------------------------------------------------------------

struct GameState
{
	int score = 0;
	PlayState playState = STATE_START;
	SheepState sheepState = STATE_IDLE;
	SheepDirection sheepDirection = DIRECTION_RIGHT;
	std::vector< Platform > vPlatforms;
	Point2f cameraTarget{ 0.0f, 0.0f };
	std::map< int, WolfState > wolfStates;	// Keyed by wolf game object ID
	std::map< int, float >    wolfTimers;	// Warning timer per wolf ID
	std::map< int, float >    bladeAngles;	// Current pendulum angle per swinging blade ID
	std::map< int, float >    bladeAngVels;	// Angular velocity per swinging blade ID
	std::vector< CradleGroup > vCradleGroups; // Newton's Cradle groups
};


//-------------------------------------------------------------------------

void CreatePlatforms();

void DrawScene();

void UpdateDoughnuts();

void UpdateSprinkles();

void UpdateDestroyed();

void UpdateGamePlayState();

void UpdateSpikes();

void UpdateSpinningBlades();

void UpdateSwingingBlades();

void InitCradleGroups();

void UpdateNewtonsCradle();

void UpdateBouncyBushes();

void UpdateExit();

void UpdateWolves();

void SetAirborne(GameObject& obj_sheep);

void DrawCollisionBounds(GameObject& obj_sheep);

void DisplayDebugInfo(GameObject& obj_sheep);

void PrintMouseCoordinateList();

void TestAABBSegmentTest();

void RandomBaa();

void LoadLevel();

//-------------------------------------------------------------------------
