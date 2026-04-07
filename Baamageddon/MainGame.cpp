///////////////////////////////////////////////////////////////////////////
// Baamageddon - A simple platform game using the PlayBuffer framework.
// Copyright 2020 Sumo Digital Limited
///////////////////////////////////////////////////////////////////////////

#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER

//-------------------------------------------------------------------------

#include "Play.h"
#include "AABB.h"
#include "MainGame.h"

//-------------------------------------------------------------------------

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

constexpr float SHEEP_WALK_SPEED = 5.0f;
constexpr float SHEEP_JUMP_IMPULSE = 14.f;

constexpr const char* SHEEP_IDLE_LEFT_SPRITE_NAME = "spr_sheep1_idle_left";
constexpr const char* SHEEP_IDLE_RIGHT_SPRITE_NAME = "spr_sheep1_idle_right";
constexpr const char* SHEEP_WALK_LEFT_SPRITE_NAME = "spr_sheep1_walk_left";
constexpr const char* SHEEP_WALK_RIGHT_SPRITE_NAME = "spr_sheep1_walk_right";
constexpr const char* SHEEP_JUMP_LEFT_SPRITE_NAME = "spr_sheep1_jump_left";
constexpr const char* SHEEP_JUMP_RIGHT_SPRITE_NAME = "spr_sheep1_jump_right";

constexpr const char* ISLAND_A_SPRITE_NAME = "spr_island_A";
constexpr const char* ISLAND_B_SPRITE_NAME = "spr_island_B";
constexpr const char* ISLAND_C_SPRITE_NAME = "spr_island_C";
constexpr const char* ISLAND_D_SPRITE_NAME = "spr_island_D";

const Point2f SHEEP_COLLISION_HALFSIZE = { 40,40 };

constexpr const char* DOUGHNUT_SPRITE_NAME = "spr_doughnut_12";
constexpr const char* SPRINKLE_SPRITE_NAME = "spr_sprinkle";
constexpr const char* SCORE_TAB_SPRITE_NAME = "spr_score_tab";
constexpr const char* SPIKE_SPRITE_NAME = "spr_spikes";
constexpr const char* SPINNING_BLADE_SPRITE_NAME = "spr_spinning_blade";
constexpr const char* MARKER_SPRITE_NAME = "spr_invisible_marker";
constexpr const char* BOUNCY_BUSH_SPRITE_NAME = "spr_bouncy_bush_4";
constexpr const char* EXIT_SPRITE_NAME = "level_exit";
constexpr const char* WOLF_LEFT_SPRITE_NAME = "spr_wolf_left_3";
constexpr const char* WOLF_RIGHT_SPRITE_NAME = "spr_wolf_right_3";

constexpr float SPINNING_BLADE_SPEED = 3.0f;
constexpr float WOLF_POUNCE_SPEED = 8.0f;
constexpr float WOLF_DETECT_RADIUS = 200.0f;
constexpr float BOUNCY_BUSH_MULTIPLIER = 2.2f;

constexpr int LEFT_SCREEN_BOUND = 100;
constexpr int RIGHT_SCREEN_BOUND = DISPLAY_WIDTH - LEFT_SCREEN_BOUND;

constexpr int FLOOR_BOUND = DISPLAY_HEIGHT * 2;


//-------------------------------------------------------------------------

static GameState gameState;


//-------------------------------------------------------------------------
// The entry point for a PLay program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground( "Data\\Backgrounds\\spr_background.png" );
	Play::StartAudioLoop( "soundscape" );
	Play::ColourSprite( "64px", Play::cBlack );
	LoadLevel();
	CreatePlatforms();
	gameState.cameraTarget = Point2f( DISPLAY_WIDTH, DISPLAY_HEIGHT ) - Point2f( DISPLAY_WIDTH / 2.0f, DISPLAY_HEIGHT / 2.0f);
	Play::SetCameraPosition( gameState.cameraTarget );
}

//-------------------------------------------------------------------------
// Called by the PlayBuffer once for each frame of the game (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	static float fTotalGameTime = 0.f;
	fTotalGameTime += elapsedTime;

	Point2f cameraDiff = gameState.cameraTarget - Point2f( DISPLAY_WIDTH / 2.0f, DISPLAY_HEIGHT / 2.0f ) - Play::GetCameraPosition();
	Play::SetCameraPosition( Play::GetCameraPosition() + cameraDiff/8.0f );

	Play::BeginTimingBar( Play::cBlue );
	DrawScene();

	Play::ColourTimingBar( Play::cRed );
	UpdateGamePlayState();
	Play::ColourTimingBar( Play::cGreen );
	UpdateDoughnuts();
	UpdateSprinkles();
	UpdateSpikes();
	UpdateSpinningBlades();
	UpdateBouncyBushes();
	UpdateExit();
	UpdateWolves();

	Play::SetDrawingSpace( Play::SCREEN );
	Play::DrawSprite( Play::GetSpriteId( SCORE_TAB_SPRITE_NAME ), { DISPLAY_WIDTH / 2, 35 }, 0 );
	Play::DrawFontText("64px", "SCORE: " + std::to_string(gameState.score), { DISPLAY_WIDTH / 2, 28 }, Play::CENTRE);
	Play::SetDrawingSpace( Play::WORLD );

	Play::ColourTimingBar( Play::cWhite );
	Play::DrawTimingBar( { 5, DISPLAY_HEIGHT - 15 }, { 250, 10 } );
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

//-------------------------------------------------------------------------
// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

//-------------------------------------------------------------------------
void CreatePlatforms( void )
{
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType( TYPE_ISLAND );

	for( int id_platform : vPlatforms )
	{
		GameObject& obj_platform = Play::GetGameObject( id_platform );

		if( obj_platform.spriteId == Play::GetSpriteId( ISLAND_A_SPRITE_NAME ) )
		{
			Platform p = { { obj_platform.pos + Point2f( 24, 12 ), { 116, 15 } }, id_platform };
			gameState.vPlatforms.push_back( p );
		}

		if( obj_platform.spriteId == Play::GetSpriteId( ISLAND_B_SPRITE_NAME ) )
		{
			Platform p = { { obj_platform.pos + Point2f( 0, 10 ), { 250, 15 } }, id_platform };
			gameState.vPlatforms.push_back( p );
		}

		if( obj_platform.spriteId == Play::GetSpriteId( ISLAND_C_SPRITE_NAME ) )
		{
			Platform p = { { obj_platform.pos + Point2f( 0, 70 ), { 250, 15 } }, id_platform };
			gameState.vPlatforms.push_back( p );
		}

		if( obj_platform.spriteId == Play::GetSpriteId( ISLAND_D_SPRITE_NAME ) )
		{
			Platform p = { { obj_platform.pos + Point2f( 10, 50 ), { 200, 15 } }, id_platform };
			gameState.vPlatforms.push_back( p );
		}
	}
}

//-------------------------------------------------------------------------
void DrawObjectsOfType( GameObjectType type )
{
	for( int id : Play::CollectGameObjectIDsByType( type ) )
	{
		GameObject& obj = Play::GetGameObject( id );
		Play::DrawSprite( obj.spriteId, obj.pos, obj.frame);
	}
}

//-------------------------------------------------------------------------
void DrawScene()
{
	Play::DrawBackground();

	Play::ColourTimingBar( Play::cYellow );

	// Draw islands with their impact offset applied as a visual-only vertical dip,
	// then ease the offset back to zero over time.
	for( Platform& p : gameState.vPlatforms )
	{
		GameObject& obj = Play::GetGameObject( p.platform_id );
		Play::DrawSprite( obj.spriteId, obj.pos + Point2f( 0, p.impactOffset ), obj.frame );
		p.impactOffset *= 0.85f; // spring back towards 0
		if( p.impactOffset < 0.1f ) p.impactOffset = 0.f;
	}

	DrawObjectsOfType( TYPE_DOUGHNUT );
	DrawObjectsOfType( TYPE_SPRINKLE );
	DrawObjectsOfType( TYPE_SPIKES );
	DrawObjectsOfType( TYPE_BOUNCY_BUSH );

	// Draw spinning blades with rotation
	for( int id : Play::CollectGameObjectIDsByType( TYPE_SPINNING_BLADE ) )
	{
		GameObject& obj = Play::GetGameObject( id );
		Play::DrawObjectRotated( obj );
	}

	// Draw wolves with animation
	for( int id : Play::CollectGameObjectIDsByType( TYPE_WOLF ) )
	{
		GameObject& obj = Play::GetGameObject( id );
		Play::DrawObjectRotated( obj );
	}

	// Draw exit only when all doughnuts have been collected
	if( Play::CollectGameObjectIDsByType( TYPE_DOUGHNUT ).empty() )
	{
		for( int id : Play::CollectGameObjectIDsByType( TYPE_EXIT ) )
		{
			GameObject& obj = Play::GetGameObject( id );
			Play::DrawSprite( obj.spriteId, obj.pos, obj.frame );
		}
	}
}

//-------------------------------------------------------------------------
void HandlePlatformCollision(GameObject& obj_sheep)
{
	AABB sheepAABB = { obj_sheep.pos, SHEEP_COLLISION_HALFSIZE };

	int hitCount = 0;

	for (Platform& rPlatform : gameState.vPlatforms )
	{
		Vector2f positionOut;

		// Sweep X first
		if (obj_sheep.velocity.x != 0.0f)
		{
			if (AABBSweepTest(rPlatform.box, sheepAABB, { obj_sheep.velocity.x, 0.f }, positionOut))
			{
				positionOut.x += obj_sheep.velocity.x * -0.5f; // bounce out to avoid ticking.
				obj_sheep.pos = positionOut;
				sheepAABB.pos = positionOut;
				obj_sheep.velocity.x = 0.f;
				obj_sheep.acceleration.x = 0.f;
				++hitCount;
			}
		}

		// When Airborne we sweep Y movement.
		if (gameState.sheepState == STATE_AIRBORNE && obj_sheep.velocity.y != 0.0f)
		{
			if (AABBSweepTest(rPlatform.box, sheepAABB, { 0.f, obj_sheep.velocity.y }, positionOut))
			{
				obj_sheep.pos = positionOut;
				// Sheep heading down onto a platform
				if( obj_sheep.velocity.y > 0.0f )
				{
					obj_sheep.pos.y -= 1;
					gameState.sheepState = STATE_IDLE;
					// Island impact: dip proportional to landing speed, max 12 pixels
					rPlatform.impactOffset = std::min( obj_sheep.velocity.y * 0.6f, 12.0f );
					obj_sheep.velocity.y = 0.f;
					obj_sheep.acceleration.y = 0.f;
				}
				else // Sheep heading up into a platform
				{
					obj_sheep.pos.y += 1;
					obj_sheep.velocity.y = 0.f;
				}

				++hitCount;
			}
		}
		else if (gameState.sheepState != STATE_AIRBORNE)
		{
			// Check safe ground below us.
			if (AABBSweepTest(rPlatform.box, sheepAABB, { 0.f, 5.f }, positionOut))
			{
				++hitCount;
			}
		}

		// We processed a platform, jump out.
		if (hitCount > 0)
		{
			return;
		}
	}

	// We missed all platforms
	if (gameState.sheepState != STATE_AIRBORNE)
	{
		obj_sheep.velocity.y = 0.f;
		SetAirborne(obj_sheep);
	}
}

//-------------------------------------------------------------------------
void DrawCollisionBounds(GameObject& obj_sheep)
{
	for (const Platform& p : gameState.vPlatforms)
	{
		DrawAABB( p.box, Play::cBlue );
	}

	DrawAABB({ obj_sheep.pos, SHEEP_COLLISION_HALFSIZE }, Play::cBlue );
}

//-------------------------------------------------------------------------
void DisplayDebugInfo(GameObject& obj_sheep)
{
	Play::SetDrawingSpace( Play::SCREEN );

	std::ostringstream info;
	info.precision(2);
	info << std::fixed;
	info << " Playstate:" << gameState.playState;
	info << "\nSheepstate:" << gameState.sheepState;

	info << " p{ " << obj_sheep.pos.x << ", " << obj_sheep.pos.y << " } ";
	info << " v{ " << obj_sheep.velocity.x << ", " << obj_sheep.velocity.y << " } ";
	info << " a{ " << obj_sheep.acceleration.x << ", " << obj_sheep.acceleration.y << " } ";

	Play::DrawDebugText({ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, info.str().c_str());

	Play::SetDrawingSpace( Play::WORLD );
}

//-------------------------------------------------------------------------
void SetAirborne(GameObject& obj_sheep)
{
	gameState.sheepState = STATE_AIRBORNE;

	obj_sheep.acceleration = { 0.f, 0.5f };	// Gravity
}

//-------------------------------------------------------------------------
void UpdateSheep(GameObject& obj_sheep)
{

	switch (gameState.sheepState)
	{
	case STATE_IDLE: // fall through
	case STATE_WALKING:
	{

		if (Play::KeyDown(VK_LEFT))
		{
			obj_sheep.velocity = { -SHEEP_WALK_SPEED, 0 };
			Play::SetSprite(obj_sheep, SHEEP_WALK_LEFT_SPRITE_NAME, 1.0f);
			gameState.sheepDirection = DIRECTION_LEFT;
			gameState.sheepState = STATE_WALKING;
		}
		else if (Play::KeyDown(VK_RIGHT))
		{
			obj_sheep.velocity = { SHEEP_WALK_SPEED, 0 };
			Play::SetSprite(obj_sheep, SHEEP_WALK_RIGHT_SPRITE_NAME, 1.0f);
			gameState.sheepDirection = DIRECTION_RIGHT;
			gameState.sheepState = STATE_WALKING;
		}
		else
		{
			Play::SetSprite(obj_sheep, gameState.sheepDirection ? SHEEP_IDLE_RIGHT_SPRITE_NAME : SHEEP_IDLE_LEFT_SPRITE_NAME, 0.333f);
			obj_sheep.velocity.x *= 0.5;
			obj_sheep.acceleration = { 0, 0 };
		}

		if (Play::KeyPressed(VK_SPACE))
		{
			Play::SetSprite(obj_sheep, gameState.sheepDirection ? SHEEP_JUMP_RIGHT_SPRITE_NAME : SHEEP_JUMP_LEFT_SPRITE_NAME, 1.f);
			obj_sheep.velocity.y = -SHEEP_JUMP_IMPULSE;
			SetAirborne(obj_sheep);
			RandomBaa();
		}
	} break;

	case STATE_AIRBORNE:
	{
		if (Play::KeyDown(VK_LEFT))
		{
			obj_sheep.velocity.x = -SHEEP_WALK_SPEED;
			Play::SetSprite(obj_sheep, SHEEP_JUMP_LEFT_SPRITE_NAME, 1.0f);
		}
		else if (Play::KeyDown(VK_RIGHT))
		{
			obj_sheep.velocity.x = SHEEP_WALK_SPEED;
			Play::SetSprite(obj_sheep, SHEEP_JUMP_RIGHT_SPRITE_NAME, 1.0f);
		}

		// Hold to jump higher: reduce gravity while SPACE held and sheep still rising
		// This gives the player more control over jump height.
		if( Play::KeyDown( VK_SPACE ) && obj_sheep.velocity.y < 0.f )
		{
			obj_sheep.acceleration.y = 0.2f; // reduced gravity while holding jump
			// Somersault: rotate sprite while rising with held jump
			obj_sheep.rotSpeed = gameState.sheepDirection ? 0.15f : -0.15f;
		}
		else
		{
			obj_sheep.acceleration.y = 0.5f; // normal gravity
			obj_sheep.rotSpeed = 0.f;
			if( gameState.sheepState == STATE_AIRBORNE && obj_sheep.velocity.y >= 0.f )
				obj_sheep.rotation = 0.f; // reset rotation once falling
		}
	} break;
	};

	switch (gameState.sheepState)
	{
	case STATE_IDLE:
		obj_sheep.acceleration = { 0, 0 };
		obj_sheep.velocity = { 0, 0 };
		break;
	case STATE_WALKING:
		// check platform collision and enter idle or falling/jumping.
		break;
	case STATE_AIRBORNE:
		// if collision with platform then enter idle
		if (obj_sheep.pos.y > FLOOR_BOUND)
			gameState.playState = STATE_DEAD;

		break;
	default:
		break;
	};

	HandlePlatformCollision(obj_sheep);

	gameState.cameraTarget = obj_sheep.pos;
	
}

//-------------------------------------------------------------------------
void UpdateSprinkles()
{
	std::vector<int> vSprinkles = Play::CollectGameObjectIDsByType(TYPE_SPRINKLE);

	for (int id_sprinkle : vSprinkles)
	{
		GameObject& obj_sprinkle = Play::GetGameObject(id_sprinkle);

		Play::UpdateGameObject(obj_sprinkle);

		if (!Play::IsVisible(obj_sprinkle))
			Play::DestroyGameObject(id_sprinkle);
	}
}

//-------------------------------------------------------------------------
void UpdateDoughnuts()
{
	GameObject& obj_sheep = Play::GetGameObjectByType(TYPE_SHEEP);
	std::vector<int> vDoughnuts = Play::CollectGameObjectIDsByType(TYPE_DOUGHNUT);

	for (int id_doughnut : vDoughnuts)
	{
		GameObject& obj_doughnut = Play::GetGameObject(id_doughnut);
		bool hasCollided = false;

		if (Play::IsColliding(obj_doughnut, obj_sheep))
		{
			for (float rad{ 0.25f }; rad < 2.0f; rad += 0.25f)
			{
				int id = Play::CreateGameObject(TYPE_SPRINKLE, obj_sheep.pos, 0, SPRINKLE_SPRITE_NAME);
				GameObject& obj_sprinkle = Play::GetGameObject(id);
				obj_sprinkle.rotSpeed = 0.1f;
				obj_sprinkle.acceleration = { 0.0f, 0.5f };
				Play::SetGameObjectDirection(obj_sprinkle, 16, rad * PLAY_PI);
			}

			hasCollided = true;
			gameState.score += 500;
			Play::PlayAudio( "munch" );
		}

		Play::UpdateGameObject(obj_doughnut);

		if (hasCollided)
			Play::DestroyGameObject(id_doughnut);
	}
}


//-------------------------------------------------------------------------
void UpdateGamePlayState()
{
	GameObject& obj_sheep = Play::GetGameObjectByType(TYPE_SHEEP);

	switch (gameState.playState)
	{
	case STATE_START:

		for( int id_obj : Play::CollectAllGameObjectIDs() )
			Play::DestroyGameObject( id_obj );

		gameState.wolfStates.clear();
		gameState.wolfTimers.clear();
		gameState.vPlatforms.clear();

		LoadLevel();
		CreatePlatforms(); // Rebuild platform collision data for the reloaded level

		for( int id_doughnut : Play::CollectGameObjectIDsByType( TYPE_DOUGHNUT ) )
		{
			GameObject& obj = Play::GetGameObject( id_doughnut );
			obj.animSpeed = 0.0f;
			obj.frame = rand();
		}

		gameState.playState = STATE_APPEAR;
		return;

	case STATE_APPEAR:
		obj_sheep.velocity = { 0, 0 };
		obj_sheep.acceleration = { 0, 0.5f };
		Play::SetSprite(obj_sheep, SHEEP_JUMP_RIGHT_SPRITE_NAME, 0);
		obj_sheep.rotation = 0;
		gameState.playState = STATE_PLAY;
		gameState.sheepState = STATE_AIRBORNE;
		RandomBaa();
		break;

	case STATE_PLAY:

		UpdateSheep(obj_sheep);

		if (Play::KeyPressed(VK_DELETE))
		{
			gameState.playState = STATE_DEAD;
		}

		break;

	case STATE_DEAD:
		Play::DestroyGameObjectsByType( TYPE_DOUGHNUT );

		gameState.playState = STATE_WAIT;

		break;
	case STATE_WAIT:
		obj_sheep.rotation += 0.25f;
		obj_sheep.acceleration = { -0.3f , 0.5f };
		obj_sheep.velocity.y += 1.f;
		if (Play::KeyPressed(VK_SPACE) == true)
		{
			gameState.playState = STATE_START;
			gameState.score = 0;
		}
		break;

	} // End of switch

	Play::UpdateGameObject(obj_sheep);
	Play::DrawObjectRotated(obj_sheep);

	// Debug Visualisation
	static bool s_bEnableDebug = false;
	if (Play::KeyPressed(VK_HOME))
		s_bEnableDebug = !s_bEnableDebug;

	if (s_bEnableDebug)
	{
		DisplayDebugInfo(obj_sheep);
		DrawCollisionBounds(obj_sheep);
		TestAABBSegmentTest();
		PrintMouseCoordinateList();
	}
}

//-------------------------------------------------------------------------
void PrintMouseCoordinateList()
{
	static bool pressed = false;
	if (!pressed && Play::GetMouseButton(Play::LEFT))
	{
		char text[32];
		sprintf_s(text, 32, "{%.0f, %.0f},\n", Play::GetMousePos().x, Play::GetMousePos().y);
		OutputDebugStringA(text);
		pressed = true;
	}
	if (pressed && !Play::GetMouseButton(Play::LEFT))
	{
		pressed = false;
	}
}

//-------------------------------------------------------------------------
void TestAABBSegmentTest()
{
	static float t = 0;
	t += 0.04f;
	if (t > PLAY_PI) t -= PLAY_PI * 2.f;

	Vector2f b{ sin(t) * 200.f + 250.f, cos(t) * 200.f + 250.f };
	Vector2f a{ 400.f, 800.f };

	float t0;
	if (AABBSegmentTest(gameState.vPlatforms[0].box, a, b, t0))
	{
		Vector2f c = a + (b - a) * t0;
		Play::DrawLine(a, c, { 100,0,0 });
	}
	else
	{
		Play::DrawLine(a, b, { 100,100,0 });
	}

	AABB testBox = { a, Vector2f(20,30) };

	Vector2f constrainedPos;
	if (AABBSweepTest( gameState.vPlatforms[0].box, testBox, b - a, constrainedPos))
	{
		testBox.pos = constrainedPos;
		DrawAABB(testBox, { 100, 0, 100 });
	}

	DrawAABB(testBox, { 100, 100, 0 });

	testBox.pos = constrainedPos;
	DrawAABB(testBox, { 100, 100, 100 });
}


//-------------------------------------------------------------------------
// Loads the objects from the Baamageddon\Level.lev file
void LoadLevel( void )
{
	std::ifstream levelfile;
	levelfile.open( "Level.lev" );

	std::string sType, sX, sY, sSprite;

	std::getline( levelfile, sType );

	while( !levelfile.eof() )
	{
		std::getline( levelfile, sType );
		std::getline( levelfile, sX );
		std::getline( levelfile, sY );
		std::getline( levelfile, sSprite );

		if( sType == "TYPE_SHEEP" )
			Play::CreateGameObject( TYPE_SHEEP, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_ISLAND" )
			Play::CreateGameObject( TYPE_ISLAND, { std::stof( sX ), std::stof( sY ) }, 0, sSprite.c_str() );

		if( sType == "TYPE_DOUGHNUT" )
			Play::CreateGameObject( TYPE_DOUGHNUT, { std::stof( sX ), std::stof( sY ) }, 30, sSprite.c_str() );

		if( sType == "TYPE_SPIKES" )
			Play::CreateGameObject( TYPE_SPIKES, { std::stof( sX ), std::stof( sY ) }, 40, sSprite.c_str() );

		if( sType == "TYPE_SPINNING_BLADE" )
		{
			int id = Play::CreateGameObject( TYPE_SPINNING_BLADE, { std::stof( sX ), std::stof( sY ) }, 35, sSprite.c_str() );
			Play::GetGameObject( id ).velocity.x = SPINNING_BLADE_SPEED;
		}

		if( sType == "TYPE_MARKER" )
			Play::CreateGameObject( TYPE_MARKER, { std::stof( sX ), std::stof( sY ) }, 20, sSprite.c_str() );

		if( sType == "TYPE_BOUNCY_BUSH" )
			Play::CreateGameObject( TYPE_BOUNCY_BUSH, { std::stof( sX ), std::stof( sY ) }, 40, sSprite.c_str() );

		if( sType == "TYPE_EXIT" )
			Play::CreateGameObject( TYPE_EXIT, { std::stof( sX ), std::stof( sY ) }, 40, sSprite.c_str() );

		if( sType == "TYPE_WOLF" )
			Play::CreateGameObject( TYPE_WOLF, { std::stof( sX ), std::stof( sY ) }, 35, sSprite.c_str() );
	}

	levelfile.close();
}


//-------------------------------------------------------------------------
// Kills the player if they touch any spike object
void UpdateSpikes()
{
	if( gameState.playState != STATE_PLAY ) return;

	GameObject& obj_sheep = Play::GetGameObjectByType( TYPE_SHEEP );
	for( int id : Play::CollectGameObjectIDsByType( TYPE_SPIKES ) )
	{
		GameObject& obj_spike = Play::GetGameObject( id );
		if( Play::IsColliding( obj_sheep, obj_spike ) )
		{
			gameState.playState = STATE_DEAD;
			return;
		}
	}
}

//-------------------------------------------------------------------------
// Spinning blades move horizontally and rotate. They reverse when they hit
// a marker object or another hazard (spike or spinning blade).
void UpdateSpinningBlades()
{
	if( gameState.playState != STATE_PLAY ) return;

	GameObject& obj_sheep = Play::GetGameObjectByType( TYPE_SHEEP );
	std::vector<int> vBlades = Play::CollectGameObjectIDsByType( TYPE_SPINNING_BLADE );

	for( int id_blade : vBlades )
	{
		GameObject& obj_blade = Play::GetGameObject( id_blade );

		// Rotate continuously
		obj_blade.rotSpeed = 0.1f;

		// Check for reversal against markers
		for( int id_marker : Play::CollectGameObjectIDsByType( TYPE_MARKER ) )
		{
			if( Play::IsColliding( obj_blade, Play::GetGameObject( id_marker ) ) )
			{
				obj_blade.velocity.x = -obj_blade.velocity.x;
				break;
			}
		}

		// Check for reversal against other spinning blades
		for( int id_other : vBlades )
		{
			if( id_other != id_blade && Play::IsColliding( obj_blade, Play::GetGameObject( id_other ) ) )
			{
				obj_blade.velocity.x = -obj_blade.velocity.x;
				break;
			}
		}

		// Check for reversal against spikes
		for( int id_spike : Play::CollectGameObjectIDsByType( TYPE_SPIKES ) )
		{
			if( Play::IsColliding( obj_blade, Play::GetGameObject( id_spike ) ) )
			{
				obj_blade.velocity.x = -obj_blade.velocity.x;
				break;
			}
		}

		Play::UpdateGameObject( obj_blade );

		// Kill player on contact
		if( Play::IsColliding( obj_blade, obj_sheep ) )
			gameState.playState = STATE_DEAD;
	}
}

//-------------------------------------------------------------------------
// Bouncy bushes launch the player upward. The higher the fall speed, the
// bigger the bounce, up to a cap.
void UpdateBouncyBushes()
{
	if( gameState.playState != STATE_PLAY ) return;
	if( gameState.sheepState != STATE_AIRBORNE ) return;

	GameObject& obj_sheep = Play::GetGameObjectByType( TYPE_SHEEP );

	for( int id : Play::CollectGameObjectIDsByType( TYPE_BOUNCY_BUSH ) )
	{
		GameObject& obj_bush = Play::GetGameObject( id );
		if( Play::IsColliding( obj_sheep, obj_bush ) && obj_sheep.velocity.y > 0.f )
		{
			// Bounce with multiplier of incoming fall speed, capped at 2x normal jump
			float bounceSpeed = obj_sheep.velocity.y * BOUNCY_BUSH_MULTIPLIER;
			constexpr float MAX_BOUNCE = SHEEP_JUMP_IMPULSE * 2.5f;
			if( bounceSpeed > MAX_BOUNCE ) bounceSpeed = MAX_BOUNCE;
			obj_sheep.velocity.y = -bounceSpeed;
			obj_sheep.acceleration.y = 0.5f;
			Play::SetSprite( obj_sheep, gameState.sheepDirection ? SHEEP_JUMP_RIGHT_SPRITE_NAME : SHEEP_JUMP_LEFT_SPRITE_NAME, 1.f );
			RandomBaa();
			return;
		}
	}
}

//-------------------------------------------------------------------------
// Shows the exit when all doughnuts are collected and ends the level when
// the player reaches it. A burst of sprinkles celebrates the completion.
//
// NOTE: Play::ColourSprite is NOT called here every frame. It works by
// iterating over every pixel in the sprite sheet and blending in a new
// colour, which is a costly O(pixels) CPU operation. Calling it once at
// startup (or on a deliberate event) is fine; calling it at 60fps would
// burn unnecessary CPU cycles and could cause hitches. Instead, we re-use
// the pre-existing sprinkle sprite which already looks colourful.
void UpdateExit()
{
	if( gameState.playState != STATE_PLAY ) return;
	if( !Play::CollectGameObjectIDsByType( TYPE_DOUGHNUT ).empty() ) return;

	GameObject& obj_sheep = Play::GetGameObjectByType( TYPE_SHEEP );
	for( int id : Play::CollectGameObjectIDsByType( TYPE_EXIT ) )
	{
		if( Play::IsColliding( obj_sheep, Play::GetGameObject( id ) ) )
		{
			// Level complete: emit a sprinkle burst then restart
			for( float rad{ 0.0f }; rad < 2.0f; rad += 0.2f )
			{
				int sid = Play::CreateGameObject( TYPE_SPRINKLE, obj_sheep.pos, 0, SPRINKLE_SPRITE_NAME );
				GameObject& obj_s = Play::GetGameObject( sid );
				obj_s.rotSpeed = 0.1f;
				obj_s.acceleration = { 0.0f, 0.5f };
				Play::SetGameObjectDirection( obj_s, 20, rad * PLAY_PI );
			}
			gameState.playState = STATE_START;
			return;
		}
	}
}

//-------------------------------------------------------------------------
// Wolves are dormant until the player gets close. They then show a warning
// frame before pouncing in the direction they face. A pouncing wolf that
// falls below the death floor is destroyed.
//
// Wolf state is stored in gameState.wolfStates (keyed by object ID) rather
// than in the GameObject itself, since physics fields like velocity are needed
// for the pounce and cannot double as state storage.
void UpdateWolves()
{
	if( gameState.playState != STATE_PLAY ) return;

	GameObject& obj_sheep = Play::GetGameObjectByType( TYPE_SHEEP );

	for( int id : Play::CollectGameObjectIDsByType( TYPE_WOLF ) )
	{
		GameObject& obj_wolf = Play::GetGameObject( id );

		// Initialise state for newly loaded wolves
		if( gameState.wolfStates.find( id ) == gameState.wolfStates.end() )
		{
			gameState.wolfStates[id] = WOLF_IDLE;
			gameState.wolfTimers[id] = 0.f;
		}

		WolfState& wolfState = gameState.wolfStates[id];

		switch( wolfState )
		{
		case WOLF_IDLE:
		{
			// Face the player by choosing the matching directional sprite
			bool facingLeft = ( obj_sheep.pos.x < obj_wolf.pos.x );
			Play::SetSprite( obj_wolf, facingLeft ? WOLF_LEFT_SPRITE_NAME : WOLF_RIGHT_SPRITE_NAME, 0.0f );
			obj_wolf.frame = 0; // frozen on the standing frame

			float dx = obj_sheep.pos.x - obj_wolf.pos.x;
			float dy = obj_sheep.pos.y - obj_wolf.pos.y;
			if( ( dx * dx + dy * dy ) < WOLF_DETECT_RADIUS * WOLF_DETECT_RADIUS )
			{
				wolfState = WOLF_WARNING;
				gameState.wolfTimers[id] = 0.f;
				obj_wolf.frame = 1; // warning frame – gives the player a chance to react
			}
			break;
		}
		case WOLF_WARNING:
		{
			// Hold the warning frame for ~30 ticks, then pounce
			gameState.wolfTimers[id] += 1.f;
			if( gameState.wolfTimers[id] > 30.f )
			{
				bool facingLeft = ( Play::GetSpriteName( obj_wolf.spriteId ) == std::string( WOLF_LEFT_SPRITE_NAME ) );
				obj_wolf.velocity.x    = facingLeft ? -WOLF_POUNCE_SPEED : WOLF_POUNCE_SPEED;
				obj_wolf.velocity.y    = -4.f;          // small upward component to the pounce
				obj_wolf.acceleration  = { 0.f, 0.5f }; // gravity pulls it down
				obj_wolf.animSpeed     = 0.5f;
				obj_wolf.frame         = 2;
				wolfState = WOLF_POUNCING;
			}
			break;
		}
		case WOLF_POUNCING:
		{
			Play::UpdateGameObject( obj_wolf );

			// Kill player on contact
			if( Play::IsColliding( obj_wolf, obj_sheep ) )
			{
				gameState.playState = STATE_DEAD;
				return;
			}

			// Destroy wolf if it falls off the level
			if( obj_wolf.pos.y > FLOOR_BOUND )
			{
				gameState.wolfStates.erase( id );
				gameState.wolfTimers.erase( id );
				Play::DestroyGameObject( id );
			}

			break;
		}
		}
	}
}

void RandomBaa( void )
{
	switch( rand() % 5 )
	{
		case 0: Play::PlayAudio( "baa1" ); break;
		case 1: Play::PlayAudio( "baa2" ); break;
		case 2: Play::PlayAudio( "baa3" ); break;
		case 3: Play::PlayAudio( "baa4" ); break;
		case 4: Play::PlayAudio( "baa5" ); break;
	}
}