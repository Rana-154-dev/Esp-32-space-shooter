/*
 * ESP32 Space Shooter
 * -------------------
 * A complete space shooter game built for ESP32
 * with a 128x64 OLED display.
 *
 * Features:
 * - 3 levels
 * - 3 bosses
 * - 3 power-ups
 * - Multiple enemy types
 * - Custom audio system
 * - Joystick controls
 *
 * Hardware:
 * - ESP32
 * - 128x64 SSD1306 OLED
 * - PS2 joystick
 * - Speaker
 * - Push button
 *
 * YouTube:
 * https://youtu.be/Q0ecRh4ckHo?si=Latqo1PBueYFOt3F
 *
 * Author: Rana (SpritualEngineering)
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Pins
#define JOY_X 34
#define JOY_Y 35
#define JOY_SW 32
#define SPEAKER_PIN 25

// Ship dimensions
#define SHIP_SIZE 16

// Playable area after setRotation(3)
#define PLAY_W 64
#define PLAY_H 128

int playerX = 24;
int playerY = 100;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Bitmaps ---
const unsigned char PROGMEM playerShipUp[] = {
  0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x03, 0xc0,
  0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8,
  0x1f, 0xf8, 0x3f, 0xfc, 0x33, 0xcc, 0x73, 0xce,
  0x60, 0x06, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM enemyShipScout[] = {
  0x18, 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x1e, 0x78,
  0x0f, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe,
  0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0,
  0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00
};

const unsigned char PROGMEM powerupHealth[] = {
  0x07, 0xe0, 0x1f, 0xf8, 0x38, 0x1c, 0x70, 0x0e,
  0x66, 0x66, 0xc6, 0x63, 0xc6, 0x63, 0xcf, 0xf3,
  0xc6, 0x63, 0xc6, 0x63, 0x66, 0x66, 0x70, 0x0e,
  0x38, 0x1c, 0x1f, 0xf8, 0x07, 0xe0, 0x00, 0x00 
};

const unsigned char PROGMEM powerupShipShield[] = {
  0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0,
  0x1b, 0xd8, 0x39, 0x9c, 0x71, 0x8e, 0x7b, 0xde,
  0xef, 0xf7, 0xc7, 0xe3, 0x83, 0xc1, 0x81, 0x81,
  0xc0, 0x03, 0xe0, 0x07, 0x70, 0x0e, 0x30, 0x0c
};

const unsigned char PROGMEM ship_MissileCarrier[] = {
  0x03, 0xc0, // Forward target guidance scanner
  0x07, 0xe0, 
  0x1f, 0xf8, // Central hull frame
  0x39, 0x9c, // Open missile tube vents
  0x71, 0x8e, 
  0x63, 0xc6, // Loaded missile bay (ready to fire)
  0xe3, 0xc7, // Swept support wings
  0xff, 0xff, 
  0xdb, 0xdb, // Dual launcher pods
  0x99, 0x99, 
  0x99, 0x99, // Engine exhaust nacelles
  0x81, 0x81, 
  0xc0, 0x03, 
  0x60, 0x06, 
  0x30, 0x0c, 
  0x00, 0x00
};

const unsigned char PROGMEM ship_Hacker[] = {
  0x00, 0x00,
  0x18, 0x18,
  0x3c, 0x3c,
  0x7e, 0x7e,
  0xdb, 0xdb,
  0xff, 0xff,
  0x7e, 0x7e,
  0x3c, 0x3c,
  0x18, 0x18,
  0x3c, 0x3c,
  0x66, 0x66,
  0xc3, 0xc3,
  0x81, 0x81,
  0x00, 0x00,
  0x00, 0x00
};
const unsigned char PROGMEM bullet[] = {
  0b11000000, 0b11000000, 0b11000000, 0b11000000
};

// --- SpiritualEngineering logo for the title screen (56x48 monochrome) ---
const unsigned char PROGMEM spiritualEngineeringLogo[] = {
  0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x60, 0x78, 0x08, 0x00, 0x00, 0x00, 0x00, 0x70, 0x78, 0x3C, 0x00, 0x00, 0x00,
  0x00, 0x78, 0x78, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x78, 0x7C, 0x00, 0x00, 0x00,
  0x00, 0x7D, 0xFE, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xF0, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x1E, 0x3E, 0x39, 0xF0, 0xF0, 0x00, 0x00,
  0x3F, 0x7C, 0x10, 0x79, 0xF0, 0x00, 0x00, 0x1F, 0xF8, 0x10, 0x3F, 0xF0, 0x00, 0x00,
  0x0F, 0xFC, 0x10, 0x7F, 0xE0, 0x00, 0x00, 0x07, 0xE6, 0x30, 0xEF, 0x80, 0x00, 0x00,
  0x01, 0xC3, 0x0B, 0x87, 0x00, 0x00, 0x00, 0x03, 0xC3, 0x7D, 0x07, 0x00, 0x00, 0x00,
  0x03, 0x80, 0x7D, 0x07, 0x00, 0x00, 0x00, 0x3F, 0x80, 0xFE, 0x03, 0xF8, 0x00, 0x00,
  0xFF, 0xDF, 0xFA, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFC, 0xFE, 0xFF, 0xFE, 0x00, 0x00,
  0x7F, 0xC3, 0xFE, 0x03, 0xFC, 0x0C, 0x00, 0x03, 0x80, 0xF6, 0x03, 0x90, 0x1E, 0x00,
  0x03, 0x83, 0x79, 0x07, 0x3C, 0x1E, 0x00, 0x01, 0xC3, 0x8B, 0x87, 0x3E, 0x3E, 0x00,
  0x03, 0xC6, 0x30, 0xCF, 0x3E, 0x3E, 0x00, 0x0F, 0xFC, 0x10, 0x7F, 0xFF, 0x3C, 0x1C,
  0x1F, 0xFC, 0x10, 0x7F, 0xFF, 0xFC, 0x3C, 0x1F, 0xF8, 0x10, 0x3F, 0xFF, 0xFE, 0x7C,
  0x3E, 0x3E, 0x30, 0xF8, 0xFF, 0xFF, 0xFC, 0x18, 0x1F, 0xBF, 0xF3, 0xFF, 0xFF, 0xF8,
  0x00, 0x1F, 0xFF, 0xF7, 0xFF, 0xFF, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
  0x00, 0x7C, 0x7C, 0x7F, 0xFF, 0xFF, 0xC0, 0x00, 0x7C, 0x78, 0x7D, 0xFF, 0xFF, 0xE8,
  0x00, 0x78, 0x7C, 0x3C, 0x1F, 0xFF, 0xFE, 0x00, 0x70, 0x7C, 0x1C, 0x1F, 0xFF, 0xFF,
  0x00, 0x00, 0x38, 0x00, 0x3F, 0xFF, 0xFF, 0x00, 0x00, 0x38, 0x00, 0xFF, 0xFF, 0x9E,
  0x00, 0x00, 0x10, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xF1, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x01, 0xC1, 0xE7, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xE3, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE3, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xC1, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// --- Boss 1: Command Cruiser (24x16) ---
const unsigned char PROGMEM boss1_Cruiser[] = {
  0x18, 0x00, 0x18,
  0x3c, 0x00, 0x3c,
  0x7e, 0x18, 0x7e,
  0xff, 0x3c, 0xff,
  0xe7, 0x7e, 0xe7,
  0xc3, 0xff, 0xc3,
  0x99, 0xe7, 0x99,
  0xbd, 0xc3, 0xbd,
  0x7e, 0x81, 0x7e,
  0x3c, 0x00, 0x3c,
  0x18, 0x3c, 0x18,
  0x18, 0x7e, 0x18,
  0x18, 0xe7, 0x18,
  0x00, 0xc3, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00
};
// --- Boss 2: Hive Mothership (24x16) ---
const unsigned char PROGMEM boss2_Mothership[] = {
  0x1f, 0xf8, 0x00, 0x3f, 0xfc, 0x00, 0x78, 0x1e, 0x00, 0xf1, 0x8f, 0x00,
  0xe3, 0xc7, 0x00, 0xe7, 0xe7, 0x00, 0xef, 0xf7, 0x00, 0xff, 0xff, 0x00,
  0xff, 0xff, 0x00, 0xe7, 0xe7, 0x00, 0xe3, 0xc7, 0x00, 0xf1, 0x8f, 0x00,
  0x78, 0x1e, 0x00, 0x3c, 0x3c, 0x00, 0x1c, 0x38, 0x00, 0x08, 0x10, 0x00
};
// --- Boss 3: Alien Dreadnought (24x16) ---
const unsigned char PROGMEM boss3_Dreadnought[] = {
  0x7f, 0xfe, 0x00, 0xff, 0xff, 0x00, 0xc1, 0x83, 0x00, 0xdb, 0xdb, 0x00,
  0xff, 0xff, 0x00, 0x7e, 0x7e, 0x00, 0x3c, 0x3c, 0x00, 0x3f, 0xfc, 0x00,
  0x1f, 0xf8, 0x00, 0x0f, 0xf0, 0x00, 0x27, 0xe4, 0x00, 0x67, 0xe6, 0x00,
  0xee, 0x77, 0x00, 0xcc, 0x33, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00
};



// Lookup table so a Level can just say "use boss sprite N".
// index 0 = Cruiser, 1 = Dreadnought, 2 = Mothership - matches the
// bossSpriteIndex values used down in level1Bosses/level2Bosses/level3Bosses.
const unsigned char* const bossSprites[3] = {
  boss1_Cruiser, boss2_Mothership, boss3_Dreadnought
};

// --- Player Bullets ---
#define MAX_BULLETS 6
struct Bullet { int x; int y; bool active; };
Bullet bullets[MAX_BULLETS];

unsigned long lastFireTime = 0;
const unsigned long FIRE_RATE_MS = 180;

// --- Ammo & Auto-Reload ---
const int MAX_AMMO = 20;
int ammo = 20;
bool isAutoReloading = false;
unsigned long lastShotTime = 0;
unsigned long autoReloadStartTime = 0;
const unsigned long INACTIVITY_RELOAD_DELAY = 2000;
const unsigned long RELOAD_DURATION_MS = 2000;

// --- Audio ---
struct Note { int freq; int duration; };
// ============================================================
// LEVEL MUSIC
// ============================================================

const Note level1Melody[] = {
  {262, 90}, {311, 90}, {392, 90}, {523, 90},
  {622, 90}, {523, 90}, {392, 90}, {311, 90},
  {262, 90}, {311, 90}, {392, 90}, {523, 90},
  {784, 120}, {622, 90}, {523, 90}, {0, 60},
  {466, 90}, {349, 90}, {311, 90}, {349, 90},
  {466, 90}, {523, 90}, {622, 90}, {0, 60}
};

const Note level2Melody[] = {
  {330, 90}, {392, 90}, {494, 90}, {659, 90},
  {587, 90}, {494, 90}, {392, 90}, {330, 90},
  {330, 90}, {392, 90}, {494, 90}, {587, 90},
  {784, 120}, {659, 90}, {587, 90}, {0, 60},
  {494, 90}, {440, 90}, {392, 90}, {440, 90},
  {494, 90}, {587, 90}, {659, 90}, {0, 60}
};

const Note level3Melody[] = {
  {196, 100}, {247, 100}, {294, 100}, {392, 100},
  {349, 100}, {294, 100}, {247, 100}, {196, 100},
  {220, 100}, {277, 100}, {330, 100}, {440, 100},
  {392, 100}, {330, 100}, {277, 100}, {0, 60},
  {247, 100}, {294, 100}, {370, 100}, {494, 100},
  {440, 100}, {370, 100}, {294, 100}, {0, 60}
};

const Note* currentMelody;
int totalBgmNotes = 0;

int currentBgmNote = 0;
unsigned long lastBgmTime = 0;

// --- Enemies ---
enum EnemyType { SINE = 0, STRAIGHT = 1, ZIGZAG = 2, DIVE = 3, FAST_STRAIGHT = 4, Tracker = 5 };
#define TYPE_BIT(t) (1 << (t))

struct Enemy {
  int x, y, startX;
  bool alive;
  EnemyType type;
  int dir;
  unsigned long nextShotTime;
};
// Bumped from 5 to 10 so the Mothership's 10-drone summon special (see
// BOSS_SUMMON below) has enough concurrent slots for its whole swarm.
#define MAX_ENEMIES 10
Enemy enemies[MAX_ENEMIES];

// --- Enemy Bullets (shared by regular enemies AND the boss) ---
// Bumped from 10 to 30 so the Dreadnought's bullet-storm special (see
// BOSS_BULLETSTORM below) can actually fill the screen instead of being
// capped out almost immediately.
#define MAX_ENEMY_BULLETS 30
struct EnemyBullet { int x; int y; bool active; };
EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];

const int ENEMY_BULLET_SPEED = 2;
const unsigned long ENEMY_FIRE_MIN_MS = 1000;
const unsigned long ENEMY_FIRE_MAX_MS = 2800;

// --- Boss ---
// attackPhase drives each boss's special move:
// - Cruiser (specialType 1): locks onto the player's X and keeps tracking
//   it for the WHOLE duration the laser is firing, so there's no window
//   to dodge once it starts - that's the intended mechanic.
// - Mothership (specialType 2): summons a swarm of tracking drones that
//   fly in from the left/right edges instead of the top.
// - Dreadnought (specialType 3): fires repeated spread volleys that fill
//   the screen with bullets for a few seconds.
enum BossAttackPhase { BOSS_NORMAL, BOSS_TRACK, BOSS_LASER, BOSS_CHARGE, BOSS_SUMMON, BOSS_BULLETSTORM };

const unsigned long BOSS_TRACK_MS = 1800;    // telegraph/tracking duration before firing
const unsigned long BOSS_LASER_MS = 350;    // laser stays live this long, tracking the whole time
const unsigned long BOSS_CHARGE_MS = 250;   // dash duration after firing
const unsigned long BOSS_SPECIAL_COOLDOWN_MS = 4000; // base gap before it can fire again
const int LASER_WIDTH = 6;
const int BOSS_CHARGE_SPEED = 4;
const int BOSS_LASER_DAMAGE = 30; // guaranteed hit - not avoidable (unless the shield eats it)

// Mothership drone-summon special
const int DRONE_SUMMON_TOTAL = 10;
const unsigned long DRONE_SPAWN_GAP_MS = 300;

// Dreadnought bullet-storm special
const int STORM_VOLLEY_TOTAL = 10;
const unsigned long STORM_SHOT_GAP_MS = 150;

#define MAX_BOSSES 3

struct Boss {
  int x, y;
  int hp, maxHp;
  bool active;
  int dir;
  int spriteIndex;
  int specialType;   // 0 = none, 1 = Cruiser tracking laser, 2 = Mothership drone summon, 3 = Dreadnought bullet storm
  unsigned long nextShotTime;

  int attackPhase;
  unsigned long phaseTimer;
  unsigned long nextSpecialTime;
  int laserX;
  bool laserDamageApplied;
  int chargeDir;

  int summonCount;              // drones summoned so far this special (Mothership)
  unsigned long summonNextTime; // when the next drone spawns
  int stormShotsFired;          // volleys fired so far this special (Dreadnought)
  unsigned long stormNextShotTime; // when the next volley fires
  bool specialPowerupSpawned;      // powerup spawned for the current special
};
Boss bosses[MAX_BOSSES];
int activeBossCount = 0;

// --- Escort Ships (Shield Escort + Missile Carrier share this shape) ---
enum EscortState { INACTIVE, ENTERING, PROTECTING, RETREATING };
struct Escort {
  float x, y;
  int hp;  // used only by the Shield Escort
  EscortState state;
};
Escort escort;         // Shield Escort - absorbs hits meant for the player
Escort missileEscort;  // Missile Carrier - fires homing missiles, doesn't shield

enum HackerState { HACKER_INACTIVE, HACKER_ENTERING, HACKER_READY, HACKER_DIVERTING, HACKER_RETURNING };
struct Hacker {
  float x, y;
  HackerState state;
  unsigned long effectStartTime;
};
Hacker hackerShip;

// --- Homing Missiles (fired by the Missile Carrier) ---
#define MAX_MISSILES 10
struct Missile {
  float x, y;
  float vx, vy;
  bool active;
  int targetType;  // 0 = none, 1 = enemy, 2 = boss
  int targetIndex;
};
Missile missiles[MAX_MISSILES];

const float MISSILE_SPEED = 4.0;
const float MISSILE_TURN_RATE = 0.18; // higher = sharper homing turns
const unsigned long MISSILE_FIRE_INTERVAL_MS = 140; // rapid-fire carrier barrage
unsigned long lastMissileFireTime = 0;
const int MAX_MISSILE_AMMO = 10;
int missileAmmo = MAX_MISSILE_AMMO; // missiles remaining in this carrier sortie
const int MISSILE_DAMAGE_TO_BOSS = 3;

// --- Hacker Ship ---
const unsigned long HACKER_DIVERT_MS = 3200;
const unsigned long HACKER_RETURN_MS = 450;
int missileTapCount = 0;
int hackerTapCount = 0;
bool hackerPowerupSpawned = false;
bool shieldPowerupSpawned = false;
bool missileAutoMode = false;
unsigned long specialTapWindowStart = 0;
int specialTapCount = 0;
const unsigned long SPECIAL_TAP_WINDOW_MS = 500;
int hackerSavedEnemyX[MAX_ENEMIES];
int hackerSavedBulletX[MAX_ENEMY_BULLETS];

// --- Waves & Levels ---
struct Wave {
  int enemyCount;
  unsigned long spawnGapMs;
  unsigned long postWaveDelayMs;
  uint8_t typeMask;
};

struct BossConfig {
  int spriteIndex;
  int hp;
  int specialType; // 0 = none, 1 = Cruiser tracking laser, 2 = Mothership drone summon, 3 = Dreadnought bullet storm
};

struct Level {
  const Wave* waves;
  int waveCount;
  const BossConfig* bosses;
  int bossCount;
};

const Wave level1Waves[] = {
  { 2, 12000, 1200, TYPE_BIT(STRAIGHT) },
  { 4, 800, 2500, TYPE_BIT(SINE) | TYPE_BIT(STRAIGHT) },
  {10, 600, 2500, TYPE_BIT(SINE) },
  { 5, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 400, 2500, TYPE_BIT(STRAIGHT) },
  { 2, 9000, 3000, TYPE_BIT(DIVE) },
  { 8, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 600, 2500, TYPE_BIT(SINE) },
  { 7, 600, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) },
  { 8, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 9, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 900, 3000, TYPE_BIT(DIVE) },
};

const Wave level2Waves[] = {
  { 5, 600, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) },
  { 5, 550, 2500, TYPE_BIT(ZIGZAG) | TYPE_BIT(FAST_STRAIGHT) },
  {10, 600, 2500, TYPE_BIT(SINE) },
  { 5, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 400, 2500, TYPE_BIT(STRAIGHT) },
  { 2, 9000, 3000, TYPE_BIT(DIVE) },
  
  { 10, 600, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) },
  { 10, 550, 2500, TYPE_BIT(ZIGZAG) | TYPE_BIT(FAST_STRAIGHT) },
  { 8, 600, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) },
  { 8, 550, 2500, TYPE_BIT(ZIGZAG) | TYPE_BIT(FAST_STRAIGHT) },
  {10, 600, 2500, TYPE_BIT(SINE) },
};

const Wave level3Waves[] = {
  { 5, 500, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) | TYPE_BIT(DIVE) },
  { 5, 450, 3000, TYPE_BIT(ZIGZAG) | TYPE_BIT(FAST_STRAIGHT) | TYPE_BIT(Tracker) },
  { 2, 9000, 3000, TYPE_BIT(DIVE) },
  {10, 600, 2500, TYPE_BIT(SINE) },
  { 5, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 400, 2500, TYPE_BIT(STRAIGHT) },
  
  { 8, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 600, 2500, TYPE_BIT(SINE) },
  { 7, 600, 2500, TYPE_BIT(SINE) | TYPE_BIT(ZIGZAG) },
  { 8, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 9, 600, 2500, TYPE_BIT(ZIGZAG) },
  { 10, 900, 3000, TYPE_BIT(DIVE) },
};



// Level 1: Cruiser solo - tracking laser special. HP nudged up a little.
const BossConfig level1Bosses[] = { { 0, 60, 1 } };
// Level 2: Mothership solo - drone summon special. HP nudged up a little.
const BossConfig level2Bosses[] = { { 1, 100, 2 } };
// Level 3: Dreadnought solo - bullet storm special. HP nudged up a little.
const BossConfig level3Bosses[] = { { 2, 150, 3 } };
// Level 4: all three at once - only the Cruiser keeps its laser


const Level levels[] = {
  { level1Waves, sizeof(level1Waves) / sizeof(Wave), level1Bosses, 1 },
  { level2Waves, sizeof(level2Waves) / sizeof(Wave), level2Bosses, 1 },
  { level3Waves, sizeof(level3Waves) / sizeof(Wave), level3Bosses, 1 },
  
};
const int totalLevels = sizeof(levels) / sizeof(Level);

enum WaveState { WS_SPAWNING, WS_WAIT_CLEAR, WS_BOSS_FIGHT, WS_LEVEL_CLEAR };
WaveState waveState = WS_SPAWNING;

// --- Story / game state ---
// Declared here (before updateWaveSystem, which references it) rather
// than down near game_over(), since Arduino compiles top-to-bottom.
enum GameState { STATE_TITLE, STATE_INTRO, STATE_LEVEL_INTRO, STATE_PLAYING, STATE_GAME_OVER, STATE_VICTORY };
GameState gameState = STATE_TITLE;
unsigned long levelIntroStartTime = 0;
const unsigned long LEVEL_INTRO_AUTO_MS = 4500; // level briefing auto-continues if no button press

int currentLevel = 0;
int currentWave = 0;
int enemiesSpawnedInWave = 0;
unsigned long waveStateTimer = 0;
unsigned long lastSpawnTime = 0;

// Guarantees the homing-missile powerup shows up partway through level 2,
// instead of relying on the random enemy-kill drop chance.
bool missilePowerupSpawned = false;

// --- Powerups ---
// type 1 = Health, 2 = Shield Escort, 3 = Missile Carrier, 4 = Hacker Ship
#define MAX_POWERUPS 4
struct Power {
  int x, y;
  bool available;
  int type;
  const uint8_t* sprite;
};
Power powers[MAX_POWERUPS];

bool checkCollision(int bx, int by, int bw, int bh, int ex, int ey, int ew, int eh);

void fireEnemyBullet(int x, int y) {
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!enemyBullets[i].active) {
      enemyBullets[i].x = x;
      enemyBullets[i].y = y;
      enemyBullets[i].active = true;
      break;
    }
  }
}

EnemyType pickTypeFromMask(uint8_t mask) {
  EnemyType choices[6];
  int n = 0;
  for (int t = 0; t < 6; t++) {
    if (mask & (1 << t)) choices[n++] = (EnemyType)t;
  }
  return (n == 0) ? SINE : choices[random(0, n)];
}

int countAliveEnemies() {
  int count = 0;
  for (int i = 0; i < MAX_ENEMIES; i++) if (enemies[i].alive) count++;
  return count;
}

void spawnEnemy(EnemyType type) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].alive) {
      enemies[i].startX = random(0, PLAY_W - SHIP_SIZE);
      enemies[i].x = enemies[i].startX;
      enemies[i].y = -SHIP_SIZE;
      enemies[i].alive = true;
      enemies[i].type = type;
      enemies[i].dir = (random(0, 2) == 0) ? -1 : 1;
      enemies[i].nextShotTime = millis() + random(ENEMY_FIRE_MIN_MS, ENEMY_FIRE_MAX_MS);
      break;
    }
  }
}

// Spawns one drone entering from the play field's left or right edge
// (instead of the top) for the Mothership's drone-summon special. Reuses
// the existing Tracker enemy type, which already homes toward the
// player's X while it descends - that's what makes it "track the player".
void spawnTrackerDrone(bool fromLeft) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].alive) {
      enemies[i].startX = fromLeft ? 0 : (PLAY_W - SHIP_SIZE);
      enemies[i].x = enemies[i].startX;
      enemies[i].y = 20; // enters from the side at boss height, not the top
      enemies[i].alive = true;
      enemies[i].type = Tracker;
      enemies[i].dir = fromLeft ? 1 : -1;
      enemies[i].nextShotTime = millis() + random(ENEMY_FIRE_MIN_MS, ENEMY_FIRE_MAX_MS);
      break;
    }
  }
}

void spawnBoss() {
  const Level& lvl = levels[currentLevel];
  activeBossCount = lvl.bossCount;

  for (int i = 0; i < activeBossCount; i++) {
    bosses[i].x = 4 + i * 14; // slight initial spread when more than one
    bosses[i].y = 15 + i * 20; // stack vertically so multiple bosses don't overlap
    bosses[i].maxHp = lvl.bosses[i].hp;
    bosses[i].hp = bosses[i].maxHp;
    bosses[i].active = true;
    bosses[i].dir = (i % 2 == 0) ? 1 : -1;
    bosses[i].spriteIndex = lvl.bosses[i].spriteIndex;
    bosses[i].specialType = lvl.bosses[i].specialType;
    bosses[i].nextShotTime = millis() + 1000 + i * 300;

    bosses[i].attackPhase = BOSS_NORMAL;
    bosses[i].phaseTimer = millis();
    bosses[i].nextSpecialTime = millis() + random(3000, 5000);
    bosses[i].laserDamageApplied = false;
    bosses[i].summonCount = 0;
    bosses[i].stormShotsFired = 0;
    bosses[i].specialPowerupSpawned = false;
  }
  for (int i = activeBossCount; i < MAX_BOSSES; i++) {
    bosses[i].active = false;
  }
}

bool anyBossActive() {
  for (int i = 0; i < MAX_BOSSES; i++) {
    if (bosses[i].active) return true;
  }
  return false;
}

void add_powerup(int spawnX, int spawnY, int type = (random(0, 2) == 0) ? 1 : 2) {
  // Shield Escort is locked until Level 1 Wave 5. Random enemy drops must
  // never be able to unlock it earlier.
  if (type == 2 && currentLevel == 0 && currentWave < 4) {
    type = 1;
  }

  for (int i = 0; i < MAX_POWERUPS; i++) {
    if (!powers[i].available) {
      powers[i].available = true;
      powers[i].type = type;
      if (type == 1) powers[i].sprite = powerupHealth;
      else if (type == 2) powers[i].sprite = powerupShipShield;
      else if (type == 3) powers[i].sprite = ship_MissileCarrier;
      else powers[i].sprite = ship_Hacker;
      powers[i].x = spawnX;
      powers[i].y = spawnY;
      break;
    }
  }
}

// Scripted fleet powerups.
// Shield: Level 1, wave 5.
// Missile Carrier: middle of Level 2 (wave 2).
// Hacker Ship: middle of Level 3 (wave 2).




void checkScriptedPowerupSpawns() {
  if (currentLevel == 0 && currentWave == 5 && !shieldPowerupSpawned  && waveState == WS_SPAWNING) {
    if (!shieldPowerupSpawned) {
      escort.state=ENTERING;
      shieldPowerupSpawned = true;
    }
  }

  if (currentLevel == 1 && currentWave == 5 && !missilePowerupSpawned && waveState == WS_SPAWNING) {
    missileEscort.state=ENTERING;
    missilePowerupSpawned = true;
  }

  if (currentLevel == 2 && currentWave == 2 && !hackerPowerupSpawned && waveState == WS_SPAWNING) {
    hackerShip.state=HACKER_ENTERING;
    hackerPowerupSpawned = true;
  }
}

// --- Mid-Wave Dialogue System ---
// Write your own story here. Each entry fires ONCE, the moment that
// level+wave becomes active, as a small banner across the bottom of the
// screen - gameplay keeps running underneath, nothing pauses.
//
// level / wave are both 0-based: level 1 = 0, level 2 = 1, ... and within
// a level, wave 1 = 0, wave 2 = 1, ...
// Use up to 3 short lines (screen is ~64px wide at this text size, so
// keep each line to roughly 10 characters or it'll run off the edge).
// Leave a line as "" if you only need 1 or 2 lines for that entry.
// displayMs: how long THIS banner stays up, in milliseconds. Set to 0 to
// use the global default (DIALOGUE_DISPLAY_MS) instead of a custom value.
struct DialogueEvent {
  int level;
  int wave;
  const char* line1;
  const char* line2;
  const char* line3;
  const char* line4;
  const char* line5;
  unsigned long displayMs;
  bool shown; // runtime flag - don't set this, checkDialogueTriggers() manages it
};










DialogueEvent dialogueEvents[] = {
  // { level, wave, "LINE 1", "LINE 2", "LINE 3", "LINE 4", "LINE 5", displayMs, false },
  { 0, 0, "ha ha", "", "",  "",  "", 600, false },
  { 0, 0, "Flag Ship: ", "HQ,we have"," lost all", "signals of", "our fleet", 0, false },
  { 0, 0, "HQ: we", "can't fall", "back now", "complete", "the task", 0, false },
  { 0, 5, "Shild Ship:", "", "there you", "are!",  "", 0, false },
  { 0, 5, "Flag Ship:", "","Shield", "ship found!",  "", 0, false },
  { 1, 5, "Missile", "ship: ", "Flag Ship", "can you","hear me?", 0, false },
  { 1, 5, "Flag Ship:", "", "dubble tap", "to use", "missiles!", 0, false },
  { 2, 2, "Hacker", "Ship:", "found the",  "fleet's",  "coordinates!", 0, false },
  { 2, 2, "Flag Ship:", "", "the whole", "fleet is", "back!", 0, false },
  { 2, 2, "Flag Ship:", "", "triple tap", "to use", "hacking!", 0, false },

};
const int dialogueEventCount = sizeof(dialogueEvents) / sizeof(DialogueEvent);

int activeDialogueIndex = -1;
unsigned long dialogueStartTime = 0;
const unsigned long DIALOGUE_DISPLAY_MS = 5000; // default banner duration, used when an entry's displayMs is 0

void startLevel1BossRush();

// Level 1 story beat: the Cruiser rushes the fleet as a cutscene right as
// the level begins (before any waves spawn), and all escort ships are
// forced inactive for it.
bool level1BossRushDone = false;
bool bossRushActive = false;
unsigned long bossRushStartTime = 0;
const unsigned long BOSS_RUSH_MS = 1300;
float bossRushX = 20;
float bossRushY = -20;

// Call once per frame during gameplay. Fires the first matching, not-yet-shown
// entry for the current level/wave. Only one dialogue box on screen at a time.
void checkDialogueTriggers() {
  if (activeDialogueIndex != -1) return;

  for (int i = 0; i < dialogueEventCount; i++) {
    if (!dialogueEvents[i].shown &&
        dialogueEvents[i].level == currentLevel &&
        dialogueEvents[i].wave == currentWave) {
      dialogueEvents[i].shown = true;
      activeDialogueIndex = i;
      dialogueStartTime = millis();
      break;
    }
  }
}













// Draws the active dialogue banner (if any) and auto-clears it after its
// display duration. Call this from the render section, after the other
// sprites so the banner sits on top and stays readable.
void draw_mid_wave_dialogue() {
  if (activeDialogueIndex == -1) return;

  DialogueEvent &d = dialogueEvents[activeDialogueIndex];
  unsigned long duration = (d.displayMs > 0) ? d.displayMs : DIALOGUE_DISPLAY_MS;

  if (millis() - dialogueStartTime >= duration) {
    activeDialogueIndex = -1;
    return;
  }

  const int boxY = 20;
  const int boxH = 55;

  //display.fillRect(0, boxY, PLAY_W, boxH, BLACK);
  display.drawRect(0, boxY, PLAY_W, boxH, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  int y = boxY + 3;
  if (d.line1[0]) { display.setCursor(3, y); display.print(d.line1); y += 10; }
  if (d.line2[0]) { display.setCursor(3, y); display.print(d.line2); y += 10; }
  if (d.line3[0]) { display.setCursor(3, y); display.print(d.line3); y += 10; }
  if (d.line4[0]) { display.setCursor(3, y); display.print(d.line4); y += 10; }
  if (d.line5[0]) { display.setCursor(3, y); display.print(d.line5); }
}


void selectLevelMusic() {

  if (currentLevel == 0) {
    currentMelody = level1Melody;
    totalBgmNotes = sizeof(level1Melody) / sizeof(level1Melody[0]);
  }

  else if (currentLevel == 1) {
    currentMelody = level2Melody;
    totalBgmNotes = sizeof(level2Melody) / sizeof(level2Melody[0]);
  }

  else {
    currentMelody = level3Melody;
    totalBgmNotes = sizeof(level3Melody) / sizeof(level3Melody[0]);
  }

  currentBgmNote = 0;
  lastBgmTime = millis();
}


void updateWaveSystem() {
  const Level& lvl = levels[currentLevel];
  const Wave& wave = lvl.waves[currentWave];
  unsigned long now = millis();

  switch (waveState) {
    case WS_SPAWNING:
      if (enemiesSpawnedInWave < wave.enemyCount) {
        if (now - lastSpawnTime >= wave.spawnGapMs) {
          spawnEnemy(pickTypeFromMask(wave.typeMask));
          enemiesSpawnedInWave++;
          lastSpawnTime = now;
        }
      } else {
        waveState = WS_WAIT_CLEAR;
      }
      break;

    case WS_WAIT_CLEAR:
      if (countAliveEnemies() == 0) {
        currentWave++;
        enemiesSpawnedInWave = 0;
        if (currentWave >= lvl.waveCount) {
          waveState = WS_BOSS_FIGHT;
          spawnBoss();
        } else {
          waveState = WS_SPAWNING;
        }
      }
      break;

    case WS_BOSS_FIGHT:
      if (!anyBossActive()) {
        waveState = WS_LEVEL_CLEAR;
        waveStateTimer = now;
      }
      break;

    case WS_LEVEL_CLEAR:
      if (now - waveStateTimer >= 2500) {
        bool finishedFinalLevel = (currentLevel == totalLevels - 1);
        currentLevel = (currentLevel + 1) % totalLevels;
        currentWave = 0;

        selectLevelMusic();
        missilePowerupSpawned = false; // allow it to spawn again on a future lap through level 2
        missileAutoMode = false;
        specialTapCount = 0;
        specialTapWindowStart = 0;
        waveState = WS_SPAWNING;

        if (finishedFinalLevel) {
          gameState = STATE_VICTORY;
        } else {
          gameState = STATE_LEVEL_INTRO;
          levelIntroStartTime = now;
        }
      }
      break;
  }

  checkScriptedPowerupSpawns();
  checkDialogueTriggers();
}

void resetWaveSystem() {
  currentLevel = 0;
  currentWave = 0;
  enemiesSpawnedInWave = 0;
  waveState = WS_SPAWNING;
  lastSpawnTime = millis();
  ammo = MAX_AMMO;
  isAutoReloading = false;
  missilePowerupSpawned = false;
  hackerPowerupSpawned = false;
  shieldPowerupSpawned = false;
  missileTapCount = 0;
  hackerTapCount = 0;
  missileAutoMode = false;
  specialTapWindowStart = 0;
  specialTapCount = 0;

  level1BossRushDone = false;
  bossRushActive = false;
  bossRushStartTime = 0;
  bossRushX = 20;
  bossRushY = -20;

  activeDialogueIndex = -1;
  for (int i = 0; i < dialogueEventCount; i++) dialogueEvents[i].shown = false;

  activeBossCount = 0;
  for (int i = 0; i < MAX_BOSSES; i++) {
    bosses[i].active = false;
    bosses[i].attackPhase = BOSS_NORMAL;
    bosses[i].specialPowerupSpawned = false;
  }

  escort.state = INACTIVE;
  missileEscort.state = INACTIVE;
  hackerShip.state = HACKER_INACTIVE;
  hackerShip.x = -SHIP_SIZE;
  hackerShip.y = PLAY_H;
  hackerShip.effectStartTime = 0;

  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].alive = false;
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
  for (int i = 0; i < MAX_POWERUPS; i++) powers[i].available = false;
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_MISSILES; i++) missiles[i].active = false;
}

bool checkCollision(int bx, int by, int bw, int bh, int ex, int ey, int ew, int eh) {
  return (bx < ex + ew && bx + bw > ex && by < ey + eh && by + bh > ey);
}

const int max_health = 100;
int health = 100;
bool lastSwState = HIGH;

// --- Story ---
const char* introLines[] = {
  "YEAR 2187",
  "",
  "Humanity",
  "is at war",
  "with an",
  "unknown",
  "enemy",
  "your fleet",
  "have to",
  "take them",
  "down",
  
};









const int introLineCount = sizeof(introLines) / sizeof(introLines[0]);

const char* level1Story[] = { "SECTOR 1", "FLAG SHIP:", "HQ we are","at the","location","","HQ: good","work keep","moving" };
const char* level2Story[] = { "SECTOR 2", "FLAG SHIP:", "target","destroyed","and we","found the","shield ship","HQ:good"," work keep ","it up" };
const char* level3Story[] = { "SECTOR 3", "HQ:great", "work so","far only","one last","target left","and try ","to find"," the last","friendly","ship" };
const char* const* levelStories[4] = { level1Story, level2Story, level3Story };
const int levelStoryLineCounts[4] = { 9, 10, 10,};

const char* victoryLines[] = {
  "MISSION",
  "COMPLETE",
  "",
  "EARTH IS",
  "SAFE",
  "FOR NOW",
  "",
  "PRESS FIRE",
  "TO REPLAY"
};
const int victoryLineCount = sizeof(victoryLines) / sizeof(victoryLines[0]);

void draw_title_screen() {
  display.setTextColor(WHITE);
  display.setTextSize(1);

  const char* title1 = "GALAXY ";
  const char* title2 = "SIEGE By";
  const char* channel1 = "Spiritual";
  const char* channel2 = "Engineer  -ing";

  int16_t x1, y1;
  uint16_t w, h;

  display.getTextBounds(title1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((PLAY_W - w) / 2, 1);
  display.print(title1);

  display.getTextBounds(title2, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((PLAY_W - w) / 2, 10);
  display.print(title2);

  // The original logo is converted to a 56x48 monochrome bitmap so it
  // can be shown directly on the 128x64 OLED after setRotation(3).
  display.drawBitmap(4, 21, spiritualEngineeringLogo, 56, 48, WHITE);

  display.getTextBounds(channel1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((PLAY_W - w) / 2, 73);
  display.print(channel1);

  display.getTextBounds(channel2, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((PLAY_W - w) / 2, 83);
  display.print(channel2);

  display.getTextBounds("PRESS FIRE", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((PLAY_W - w) / 2, 110);
  display.print("PRESS FIRE");
}

void draw_story_screen(const char* const* lines, int count) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  int y = 6;
  for (int i = 0; i < count; i++) {
    display.setCursor(4, y);
    display.print(lines[i]);
    y += 10;
  }
}

// Edge-detected button read, shared across every non-gameplay screen so
// a held button doesn't fire through multiple screens in one frame.
bool advance_pressed() {
  bool swState = digitalRead(JOY_SW);
  bool pressed = (swState == LOW && lastSwState ==HIGH );
  lastSwState = swState;
  return pressed;
}

void game_over() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 40);
  display.print("GAME OVER");
  display.setCursor(2, 60);
  display.print("   Press ");
  display.setCursor(2, 72);
  display.print("to Restart");

  if (advance_pressed()) {
    health = max_health;
    playerX = 24;
    playerY = 100;
    resetWaveSystem();
    gameState = STATE_LEVEL_INTRO;
    levelIntroStartTime = millis();
  }
}

void updateEscortShip() {
  if (escort.state == INACTIVE) return;

  float targetX = playerX;
  float targetY = playerY - SHIP_SIZE;

  if (escort.state == ENTERING) {
    escort.x += (targetX - escort.x) * 0.25;
    escort.y += (targetY - escort.y) * 0.25;

    display.setCursor(5,50);
    display.write("Shield!");

    if (abs(escort.x - targetX) < 2 && abs(escort.y - targetY) < 2) {
      escort.state = PROTECTING;
    }
  } else if (escort.state == PROTECTING) {
    escort.x = targetX;
    escort.y = targetY;
  } else if (escort.state == RETREATING) {
    escort.y += 3;
    if (escort.y > PLAY_H) escort.state = INACTIVE;
  }
}
// Picks a target for a fresh homing missile.
// Priority:
//   1. Normal enemies / drones first
//   2. Boss only when there are no unassigned normal enemies
//
// This means the Missile Carrier clears the small ships before turning
// its attention to the boss.
bool pickMissileTarget(int &targetType, int &targetIndex) {

  // --------------------------------------------------
  // FIRST PRIORITY: NORMAL ENEMIES / DRONES
  // --------------------------------------------------
  for (int i = 0; i < MAX_ENEMIES; i++) {

    if (!enemies[i].alive) continue;

    bool alreadyTargeted = false;

    for (int m = 0; m < MAX_MISSILES; m++) {
      if (missiles[m].active &&
          missiles[m].targetType == 1 &&
          missiles[m].targetIndex == i) {

        alreadyTargeted = true;
        break;
      }
    }

    if (!alreadyTargeted) {
      targetType = 1;
      targetIndex = i;
      return true;
    }
  }

  // --------------------------------------------------
  // SECOND PRIORITY: BOSS
  // Only target the boss after all normal enemies
  // already have missiles assigned.
  // --------------------------------------------------
  for (int i = 0; i < MAX_BOSSES; i++) {

    if (!bosses[i].active) continue;

    bool alreadyTargeted = false;

    for (int m = 0; m < MAX_MISSILES; m++) {
      if (missiles[m].active &&
          missiles[m].targetType == 2 &&
          missiles[m].targetIndex == i) {

        alreadyTargeted = true;
        break;
      }
    }

    if (!alreadyTargeted) {
      targetType = 2;
      targetIndex = i;
      return true;
    }
  }

  return false;
}
// Alternates the launch point between the carrier's left and right side
// each shot, so a burst of missiles visibly fans out from both sides
// instead of stacking on the exact same pixel.
int missileLaunchSide = 0;

bool fireHomingMissile() {
  if (missileAmmo <= 0) return false;

  int targetType;
  int targetIndex;

  if (!pickMissileTarget(targetType, targetIndex)) {
    return false;
  }

  for (int i = 0; i < MAX_MISSILES; i++) {

    if (!missiles[i].active) {

      int launchSide = missileLaunchSide;
      missileLaunchSide = 1 - missileLaunchSide;

      missiles[i].active = true;

      missiles[i].x =
        missileEscort.x + (launchSide == 0 ? -2 : SHIP_SIZE + 2);

      missiles[i].y =
        missileEscort.y + (SHIP_SIZE / 2);

      // Launch upward with a slight outward kick matching the launch side
      missiles[i].vx = (launchSide == 0 ? -1.2f : 1.2f);
      missiles[i].vy = -MISSILE_SPEED;

      missiles[i].targetType = targetType;
      missiles[i].targetIndex = targetIndex;

      missileAmmo--;

      tone(SPEAKER_PIN, 900, 30);

      return true;
    }
  }

  return false;
}

// Fires the whole remaining missile stock, staggered a few frames apart
// (see updateMissileQueue()) so each missile is visibly separate instead
// of all spawning on top of each other in a single frame.
int missilesQueued = 0;
unsigned long lastQueuedMissileTime = 0;
const unsigned long MISSILE_QUEUE_INTERVAL_MS = 90;

void fireAllMissiles() {
  // Start strategic auto-fire. updateMissileQueue() will fire exactly enough
  // missiles for the enemies currently present, then keep watching for new ones.
  if (missileAmmo <= 0) return;
  missileAutoMode = true;
  missilesQueued = 0;
  lastQueuedMissileTime = millis() - MISSILE_QUEUE_INTERVAL_MS;
}

 
void updateMissileQueue() {
  if (!missileAutoMode or missileEscort.state != PROTECTING)
    { return; }
    if(missileAmmo <= 0){
      missileEscort.state=RETREATING;
      return;
    }

  int aliveEnemies = 0;
  int assignedEnemyMissiles = 0;

  // Count alive normal enemies / drones
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].alive) aliveEnemies++;
  }

  // Count missiles currently assigned to live enemies
  for (int i = 0; i < MAX_MISSILES; i++) {

    if (missiles[i].active &&
        missiles[i].targetType == 1 &&
        missiles[i].targetIndex >= 0 &&
        missiles[i].targetIndex < MAX_ENEMIES &&
        enemies[missiles[i].targetIndex].alive) {

      assignedEnemyMissiles++;
    }
  }

  // --------------------------------------------------
  // FIRST: Make sure every enemy has a missile
  // --------------------------------------------------
  if (aliveEnemies > assignedEnemyMissiles &&
      millis() - lastQueuedMissileTime >= MISSILE_QUEUE_INTERVAL_MS) {

    if (fireHomingMissile()) {
      lastQueuedMissileTime = millis();
    }

    return;
  }

  // --------------------------------------------------
  // SECOND: If no enemies remain, attack bosses
  // --------------------------------------------------
  int aliveBosses = 0;
  int assignedBossMissiles = 0;

  for (int i = 0; i < MAX_BOSSES; i++) {
    if (bosses[i].active) aliveBosses++;
  }

  for (int i = 0; i < MAX_MISSILES; i++) {

    if (missiles[i].active &&
        missiles[i].targetType == 2 &&
        missiles[i].targetIndex >= 0 &&
        missiles[i].targetIndex < MAX_BOSSES &&
        bosses[missiles[i].targetIndex].active) {

      assignedBossMissiles++;
    }
  }

  // One missile per boss at a time.
  // Once it hits, another missile can be launched.
  if (aliveEnemies == 0 &&
      aliveBosses > assignedBossMissiles &&
      millis() - lastQueuedMissileTime >= MISSILE_QUEUE_INTERVAL_MS) {

    if (fireHomingMissile()) {
      lastQueuedMissileTime = millis();
    }
  }

  if (missileAmmo <= 0) {
    missileEscort.state = RETREATING;
    missileAutoMode = false;
    missilesQueued = 0;
    
  }
}



void startHackerDiversion() {
  if (hackerShip.state != HACKER_READY) return;

  // Destroy every active enemy
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].alive) {
      enemies[i].alive = false;
    }
  }

  // Destroy every enemy bullet
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    enemyBullets[i].active = false;
  }

  // Also destroy any remaining enemy-side threats
  for (int i = 0; i < MAX_MISSILES; i++) {
    // Player missiles are NOT destroyed.
    // They belong to the player/carrier.
  }

  hackerShip.state = HACKER_DIVERTING;
  hackerShip.effectStartTime = millis();

  tone(SPEAKER_PIN, 1400, 80);
}
// True while the hacker's diversion is holding regular enemies/bullets
// off-screen and frozen (both while they're pushed out, and while they're
// gliding back in) - EnemyManagement()/updateEnemyBullets() check this to
// skip normal movement, firing, and collisions for that window.
bool hackerFieldFrozen() {
  return (hackerShip.state == HACKER_DIVERTING || hackerShip.state == HACKER_RETURNING);
}
void updateHackerShip() {
  if (hackerShip.state == HACKER_INACTIVE) return;

  float targetX = playerX - SHIP_SIZE - 4;
  if (targetX < 0) targetX = 0;

  float targetY = playerY;

  // ENTER
  if (hackerShip.state == HACKER_ENTERING) {

    display.setCursor(5,50);
    display.write("Hacker!");
    hackerShip.x += (targetX - hackerShip.x) * 0.25;
    hackerShip.y += (targetY - hackerShip.y) * 0.25;

    if (abs(hackerShip.x - targetX) < 2 &&
        abs(hackerShip.y - targetY) < 2) {

      hackerShip.x = targetX;
      hackerShip.y = targetY;
      hackerShip.state = HACKER_READY;
    }
  }

  // WAITING BESIDE PLAYER
  else if (hackerShip.state == HACKER_READY) {

    hackerShip.x = targetX;
    hackerShip.y = targetY;
  }

  // HACKING
  else if (hackerShip.state == HACKER_DIVERTING) {

    // Keep the hacker beside the player while the hack happens
    hackerShip.x = targetX;
    hackerShip.y = targetY;

    // Make absolutely sure enemies and bullets stay destroyed
    for (int i = 0; i < MAX_ENEMIES; i++) {
      enemies[i].alive = false;
    }

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      enemyBullets[i].active = false;
    }

    // After the hack is complete, leave the battlefield
    if (millis() - hackerShip.effectStartTime >= HACKER_DIVERT_MS) {
      hackerShip.state = HACKER_RETURNING;
    }
  }

  // LEAVE
  else if (hackerShip.state == HACKER_RETURNING) {

    hackerShip.y += 4;

    if (hackerShip.y > PLAY_H) {
      hackerShip.state = HACKER_INACTIVE;
      hackerShip.x = -SHIP_SIZE;
      hackerShip.y = PLAY_H;
    }
  }
}

void updateMissileCarrier() {
  if (missileEscort.state == INACTIVE) return;

  // Missile Carrier stays on the player's right side.
  float targetX = playerX + SHIP_SIZE + 2;
  if (targetX > PLAY_W - SHIP_SIZE) targetX = PLAY_W - SHIP_SIZE;
  float targetY = playerY;

  if (missileEscort.state == ENTERING) {
    display.setCursor(5,50);
    display.write("Missiles!");
    missileEscort.x += (targetX - missileEscort.x) * 0.25;
    missileEscort.y += (targetY - missileEscort.y) * 0.25;
    if (abs(missileEscort.x - targetX) < 2 && abs(missileEscort.y - targetY) < 2) {
      missileEscort.state = PROTECTING;
    }
  } else if (missileEscort.state == PROTECTING) {
    missileEscort.x = targetX;
    missileEscort.y = targetY;
    // Missiles are launched by fire-button taps in handleShooting().
    // The carrier remains beside the player instead of leaving after 10 shots.
  } else if (missileEscort.state == RETREATING) {
    missileEscort.y += 3;
    if (missileEscort.y > PLAY_H) missileEscort.state = INACTIVE;
  }
}





void updateMissiles() {
  for (int i = 0; i < MAX_MISSILES; i++) {
    if (!missiles[i].active) continue;

    float tx = 0, ty = 0;
    bool hasTarget = false;

    if (missiles[i].targetType == 2 && bosses[missiles[i].targetIndex].active) {
      tx = bosses[missiles[i].targetIndex].x + 12;
      ty = bosses[missiles[i].targetIndex].y + 8;
      hasTarget = true;
    } else if (missiles[i].targetType == 1 && enemies[missiles[i].targetIndex].alive) {
      tx = enemies[missiles[i].targetIndex].x + 8;
      ty = enemies[missiles[i].targetIndex].y + 8;
      hasTarget = true;
    }

    if (hasTarget) {
      float dx = tx - missiles[i].x;
      float dy = ty - missiles[i].y;
      float len = sqrt(dx * dx + dy * dy);
      if (len > 0.001) { dx /= len; dy /= len; }
      missiles[i].vx += (dx * MISSILE_SPEED - missiles[i].vx) * MISSILE_TURN_RATE;
      missiles[i].vy += (dy * MISSILE_SPEED - missiles[i].vy) * MISSILE_TURN_RATE;
    }

    // Keep speed constant even while steering
    float speed = sqrt(missiles[i].vx * missiles[i].vx + missiles[i].vy * missiles[i].vy);
    if (speed > 0.001) {
      missiles[i].vx = missiles[i].vx / speed * MISSILE_SPEED;
      missiles[i].vy = missiles[i].vy / speed * MISSILE_SPEED;
    }

    missiles[i].x += missiles[i].vx;
    missiles[i].y += missiles[i].vy;

    if (missiles[i].y < -8 || missiles[i].y > PLAY_H + 8 || missiles[i].x < -8 || missiles[i].x > PLAY_W + 8) {
      missiles[i].active = false;
      continue;
    }

    if (missiles[i].targetType == 2) {
      Boss &b = bosses[missiles[i].targetIndex];
      if (b.active && checkCollision((int)missiles[i].x, (int)missiles[i].y, 3, 3, b.x, b.y, 24, 16)) {
        b.hp -= MISSILE_DAMAGE_TO_BOSS;
        missiles[i].active = false;
        tone(SPEAKER_PIN, 350, 25);
        if (b.hp <= 0) {
          b.active = false;
          //add_powerup(b.x + 4, b.y);
        }
      }
    } else if (missiles[i].targetType == 1) {
      Enemy &e = enemies[missiles[i].targetIndex];
      if (e.alive && checkCollision((int)missiles[i].x, (int)missiles[i].y, 3, 3, e.x, e.y, SHIP_SIZE, SHIP_SIZE)) {
        e.alive = false;
        missiles[i].active = false;
        tone(SPEAKER_PIN, 350, 25);
      }
    }
  }
}




void updateEnemyBullets() {
  if (hackerFieldFrozen()) return; // held off-screen by the hacker diversion

  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!enemyBullets[i].active) continue;

    enemyBullets[i].y += ENEMY_BULLET_SPEED;
    if (enemyBullets[i].y > PLAY_H) {
      enemyBullets[i].active = false;
      continue;
    }

    // Shield Escort absorbs hits meant for the player
    if (escort.state == PROTECTING &&
        checkCollision(enemyBullets[i].x, enemyBullets[i].y, 2, 4, (int)escort.x, (int)escort.y, SHIP_SIZE, SHIP_SIZE)) {
      enemyBullets[i].active = false;
      escort.hp -= 10;
      tone(SPEAKER_PIN, 800, 20);
      if (escort.hp <= 0) escort.state = RETREATING;
      continue;
    }

    if (checkCollision(enemyBullets[i].x, enemyBullets[i].y, 2, 4, playerX, playerY, SHIP_SIZE, SHIP_SIZE)) {
      enemyBullets[i].active = false;
      if (health > 0) health -= 10;
      tone(SPEAKER_PIN, 200, 25);
    }
  }
}
void powerup_management() {
  for (int i = 0; i < MAX_POWERUPS; i++) {
    if (!powers[i].available) continue;

    powers[i].y += 1;

    if (powers[i].y > PLAY_H) {
      powers[i].available = false;
      continue;
    }

    if (checkCollision(
          powers[i].x, powers[i].y, SHIP_SIZE, SHIP_SIZE,
          playerX, playerY, SHIP_SIZE, SHIP_SIZE)) {

      // HEALTH
      if (powers[i].type == 1) {
        health = min(max_health, health + 25);
      }

      // SHIELD ESCORT
      else if (powers[i].type == 2) {

        // Start the shield ship properly every time
        escort.x = -SHIP_SIZE;
        escort.y = PLAY_H;
        escort.hp = 30;
        escort.state = ENTERING;
      }

      // MISSILE CARRIER
      else if (powers[i].type == 3) {

        missileEscort.x = -SHIP_SIZE;
        missileEscort.y = PLAY_H;
        missileAmmo = MAX_MISSILE_AMMO;

        missileEscort.state = ENTERING;

        // Reset its tap system
        missileTapCount = 0;
        missileAutoMode = false;
        lastMissileFireTime = millis();
      }

      // HACKER SHIP
      else if (powers[i].type == 4) {

        hackerShip.x = -SHIP_SIZE;
        hackerShip.y = PLAY_H;

        hackerShip.state = HACKER_ENTERING;
        hackerShip.effectStartTime = 0;

        hackerTapCount = 0;
      }

      powers[i].available = false;

      tone(SPEAKER_PIN, 1200, 60);
    }
  }
}
// Loops over every active boss. Bosses without a special just patrol and
// fire the twin-cannon pattern. Each boss with a specialType runs its own
// extra sequence (see the enum/comment above BossAttackPhase).
void startLevel1BossRush() {
  if (currentLevel != 0 || level1BossRushDone || bossRushActive) return;

  // The fleet is disabled by the Cruiser's rush. The Shield Escort is only
  // touched if it's actually out (it shouldn't be unlocked yet this early -
  // it first appears at Level 1 Wave 6) so it doesn't flash on screen.
  if (escort.state != INACTIVE) escort.state = RETREATING;
  missileEscort.state = RETREATING;
  hackerShip.state = HACKER_RETURNING;
  missileTapCount = 0;
  hackerTapCount = 0;
  specialTapCount = 0;
  specialTapWindowStart = 0;
  missileAutoMode = false;

  bossRushActive = true;
  bossRushStartTime = millis();
  bossRushX = 20;
  bossRushY = -20;
  tone(SPEAKER_PIN, 120, 160);
}

void updateLevel1BossRush() {
  if (!bossRushActive) return;

  unsigned long elapsed = millis() - bossRushStartTime;
  if (elapsed < 850) {
    bossRushY = -20 + (elapsed * 70.0f / 850.0f);
  } else {
    bossRushY = 50 - ((elapsed - 850) * 35.0f / 450.0f);
  }

  if (elapsed >= BOSS_RUSH_MS) {
    bossRushActive = false;
    level1BossRushDone = true;
  }
}

void updateBosses() {
  unsigned long now = millis();

  for (int b = 0; b < MAX_BOSSES; b++) {
    if (!bosses[b].active) continue;
    Boss &boss = bosses[b];

    switch (boss.attackPhase) {

      case BOSS_NORMAL:
        boss.x += boss.dir * 1;
        if (boss.x <= 2 || boss.x >= PLAY_W - 24) boss.dir *= -1;

        if (now >= boss.nextShotTime) {
          fireEnemyBullet(boss.x + 4, boss.y + 16);
          fireEnemyBullet(boss.x + 18, boss.y + 16);
          boss.nextShotTime = now + 1400;
        }

        if (boss.specialType != 0 && now >= boss.nextSpecialTime) {
          if (boss.specialType == 1) {
            // Cruiser: tracking laser
            boss.attackPhase = BOSS_TRACK;
            boss.phaseTimer = now;
            // One shield powerup drop as the laser winds up - a chance to
            // grab it before the unavoidable beam fires.
            add_powerup(boss.x, boss.y + 8, 2);
          } else if (boss.specialType == 2) {
            // Mothership: spawn the Missile Carrier BEFORE every special.
            // The powerup starts at the boss and falls toward the player
            // during the special's wind-up/attack window.
            add_powerup(boss.x, boss.y + 8, 3);
            boss.specialPowerupSpawned = true;
            boss.attackPhase = BOSS_SUMMON;
            boss.phaseTimer = now;
            boss.summonCount = 0;
            boss.summonNextTime = now + 1500; // small delay so the powerup can be grabbed first
          } else if (boss.specialType == 3) {
            // Dreadnought: spawn the Hacker powerup BEFORE every special.
            add_powerup(boss.x, boss.y + 8, 4);
            boss.specialPowerupSpawned = true;
            boss.attackPhase = BOSS_BULLETSTORM;
            boss.phaseTimer = now;
            boss.stormShotsFired = 0;
            boss.stormNextShotTime = now + 1500; // small delay so the powerup can be grabbed first
          }
        }
        break;

      case BOSS_TRACK:
        // Telegraph: creeps toward the player's X before committing to fire
        if (boss.x < playerX - 4) boss.x += 2;
        else if (boss.x > playerX + 4) boss.x -= 2;
        if (boss.x < 2) boss.x = 2;
        if (boss.x > PLAY_W - 24) boss.x = PLAY_W - 24;

        if (now - boss.phaseTimer >= BOSS_TRACK_MS) {
          boss.laserDamageApplied = false;
          boss.attackPhase = BOSS_LASER;
          boss.phaseTimer = now;
          tone(SPEAKER_PIN, 150, 100);
        }
        break;

      case BOSS_LASER:
        // Keeps tracking the player's live X for the whole firing window -
        // this is what makes it unavoidable once it starts, by design.
        if (boss.x < playerX - 4) boss.x += 2;
        else if (boss.x > playerX + 4) boss.x -= 2;
        if (boss.x < 2) boss.x = 2;
        if (boss.x > PLAY_W - 24) boss.x = PLAY_W - 24;
        boss.laserX = boss.x + 12 - (LASER_WIDTH / 2);

        if (!boss.laserDamageApplied) {
          // Shield Escort eats the hit if it's out - otherwise it's guaranteed
          // to land on the player. Either way it only applies once per firing.
          if (escort.state == PROTECTING) {
            escort.hp -= BOSS_LASER_DAMAGE;
            tone(SPEAKER_PIN, 800, 20);
            if (escort.hp <= 0) escort.state = RETREATING;
          } else {
            if (health > 0) health -= BOSS_LASER_DAMAGE;
            tone(SPEAKER_PIN, 180, 60);
          }
          boss.laserDamageApplied = true;
        }

        if (now - boss.phaseTimer >= BOSS_LASER_MS) {
          boss.chargeDir = (playerX < boss.x) ? -1 : 1;
          boss.attackPhase = BOSS_CHARGE;
          boss.phaseTimer = now;
        }
        break;

      case BOSS_CHARGE:
        boss.x += boss.chargeDir * BOSS_CHARGE_SPEED;
        if (boss.x < 2)  { boss.x = 2;          boss.dir = 1;  }
        if (boss.x > PLAY_W - 24) { boss.x = PLAY_W - 24; boss.dir = -1; }

        if (now - boss.phaseTimer >= BOSS_CHARGE_MS) {
          boss.attackPhase = BOSS_NORMAL;
          boss.nextSpecialTime = now + BOSS_SPECIAL_COOLDOWN_MS + random(0, 2000);
          boss.nextShotTime = now + 500;
        }
        break;

      case BOSS_SUMMON:
        // Mothership special: spawns drones one at a time, alternating
        // left/right edges, up to DRONE_SUMMON_TOTAL, then goes back to
        // normal. Doesn't matter if the player can't kill every drone -
        // the Missile Carrier powerup is scripted to arrive before this
        // fight to help clear them.
        if (boss.summonCount < DRONE_SUMMON_TOTAL && now >= boss.summonNextTime) {
          bool fromLeft = (boss.summonCount % 2 == 0);
          spawnTrackerDrone(fromLeft);
          boss.summonCount++;
          boss.summonNextTime = now + DRONE_SPAWN_GAP_MS;
        }
        if (boss.summonCount >= DRONE_SUMMON_TOTAL) {
          boss.specialPowerupSpawned = false;
          boss.attackPhase = BOSS_NORMAL;
          boss.nextSpecialTime = now + BOSS_SPECIAL_COOLDOWN_MS + random(0, 2000);
          boss.nextShotTime = now + 500;
        }
        break;

      case BOSS_BULLETSTORM:
        // Dreadnought special: repeated spread volleys across the whole
        // play width so the screen fills up with bullets for a few seconds.
        if (boss.stormShotsFired < STORM_VOLLEY_TOTAL && now >= boss.stormNextShotTime) {
          int offset = (boss.stormShotsFired % 2 == 0) ? 2 : 6;
          for (int lane = offset; lane < PLAY_W; lane += 8) {
            fireEnemyBullet(lane, boss.y + 16);
          }
          boss.stormShotsFired++;
          boss.stormNextShotTime = now + STORM_SHOT_GAP_MS;
        }
        if (boss.stormShotsFired >= STORM_VOLLEY_TOTAL) {
          boss.specialPowerupSpawned = false;
          boss.attackPhase = BOSS_NORMAL;
          boss.nextSpecialTime = now + BOSS_SPECIAL_COOLDOWN_MS + random(0, 2000);
          boss.nextShotTime = now + 500;
        }
        break;
    }

    // Player bullets vs this boss
    for (int j = 0; j < MAX_BULLETS; j++) {
      if (bullets[j].active && checkCollision(bullets[j].x, bullets[j].y, 2, 4, boss.x, boss.y, 24, 16)) {
        bullets[j].active = false;
        boss.hp -= 2;
        tone(SPEAKER_PIN, 400, 20);
        if (boss.hp <= 0) {
          boss.active = false;
          add_powerup(boss.x + 4, boss.y);
        }
      }
    }
  }
}

void EnemyManagement() {
  updateWaveSystem();
  updateBosses();

  bool frozen = hackerFieldFrozen();

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].alive) continue;
    if (frozen) continue; // held off-screen by the hacker diversion - no movement/firing/collisions

    switch (enemies[i].type) {
      case SINE:
        enemies[i].y += 1;
        enemies[i].x = enemies[i].startX + (int)(12.0 * sin(enemies[i].y * 0.08));
        break;
      case STRAIGHT: enemies[i].y += 1; break;
      case FAST_STRAIGHT: enemies[i].y += 3; break;
      case ZIGZAG:
        enemies[i].y += 1;
        enemies[i].x += enemies[i].dir * 2;
        if (enemies[i].x <= 0 || enemies[i].x >= PLAY_W - SHIP_SIZE) enemies[i].dir *= -1;
        break;
      case DIVE:
        enemies[i].y += 2;
        if (enemies[i].x < playerX) enemies[i].x += 1;
        else if (enemies[i].x > playerX) enemies[i].x -= 1;
        break;
      case Tracker:
        enemies[i].y += 1;
        if (enemies[i].x < playerX) enemies[i].x += 2.5 ;
        else if (enemies[i].x > playerX) enemies[i].x -= 1.4;
        break;
    }

    if (enemies[i].x < 0) enemies[i].x = 0;
    if (enemies[i].x > PLAY_W - SHIP_SIZE) enemies[i].x = PLAY_W - SHIP_SIZE;
    if (enemies[i].y > PLAY_H) { enemies[i].alive = false; continue; }

    if (enemies[i].y > 0 && millis() >= enemies[i].nextShotTime) {
      fireEnemyBullet(enemies[i].x + (SHIP_SIZE / 2) - 1, enemies[i].y + SHIP_SIZE);
      enemies[i].nextShotTime = millis() + random(ENEMY_FIRE_MIN_MS, ENEMY_FIRE_MAX_MS);
    }

    // Bullet vs Enemy
    for (int j = 0; j < MAX_BULLETS; j++) {
      if (bullets[j].active && checkCollision(bullets[j].x, bullets[j].y, 2, 4, enemies[i].x, enemies[i].y, SHIP_SIZE, SHIP_SIZE)) {
        bullets[j].active = false;
        int dX = enemies[i].x, dY = enemies[i].y;
        enemies[i].alive = false;
        if (random(0, 5) == 1) add_powerup(dX, dY);
        tone(SPEAKER_PIN, 300, 30);
        break;
      }
    }

    // Enemy vs Player - the Shield Escort intercepts this too, exactly
    // like it already does for bullets, so it actually protects against
    // ramming enemies instead of only projectiles.
    if (enemies[i].alive && checkCollision(enemies[i].x, enemies[i].y, SHIP_SIZE, SHIP_SIZE, playerX, playerY, SHIP_SIZE, SHIP_SIZE)) {
      if (escort.state == PROTECTING) {
        escort.hp -= 20;
        tone(SPEAKER_PIN, 800, 20);
        if (escort.hp <= 0) escort.state = RETREATING;
      } else {
        if (health > 0) health -= 20;
      }
      enemies[i].alive = false;
      tone(SPEAKER_PIN, 300, 30);
    }
  }
}

void player_health_bar() {
  int maxWidth = 16;
  int fillWidth = map(health, 0, 100, 0, maxWidth);
  display.drawRect(playerX, playerY + 16, maxWidth, 3, WHITE);
  display.fillRect(playerX, playerY + 16, fillWidth, 3, WHITE);
  if (health <= 0) gameState = STATE_GAME_OVER;
}

bool isPlayingSFX = false;
unsigned long sfxStartTime = 0;

void triggerLaserSFX() {
  tone(SPEAKER_PIN, 1800, 40);
  isPlayingSFX = true;
  sfxStartTime = millis();
}

void updateAudio() {

  unsigned long currentMillis = millis();

  if (isPlayingSFX) {
    if (currentMillis - sfxStartTime >= 40) {
      isPlayingSFX = false;
    } else {
      return;
    }
  }

  int noteDuration = currentMelody[currentBgmNote].duration;

  if (currentMillis - lastBgmTime >= noteDuration) {

    lastBgmTime = currentMillis;

    currentBgmNote++;

    if (currentBgmNote >= totalBgmNotes) {
      currentBgmNote = 0;
    }

    int freq = currentMelody[currentBgmNote].freq;

    if (freq == 0) {
      noTone(SPEAKER_PIN);
    } else {
      tone(
        SPEAKER_PIN,
        freq,
        noteDuration * 0.85
      );
    }
  }
}

void updateMovement() {
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  if (xVal < 1500 && playerX > 0) playerX -= 2;
  if (xVal > 2500 && playerX < (PLAY_W - SHIP_SIZE)) playerX += 2;
  if (yVal < 1500 && playerY > 0) playerY -= 2;
  if (yVal > 2500 && playerY < (PLAY_H - SHIP_SIZE - 4)) playerY += 2;
}
void handleShooting() {
  unsigned long now = millis();

  bool swState = digitalRead(JOY_SW);
  bool buttonPressed = (swState == LOW);
  bool tap = (buttonPressed && lastSwState == HIGH
  );

  // --------------------------------------------------
  // SPECIAL POWERUP TAP WINDOW
  // 3 taps within 500 ms = Hacker
  // 2 taps within 500 ms = Missile Carrier (when the window expires)
  // --------------------------------------------------
  if (tap && (missileEscort.state == PROTECTING || hackerShip.state == HACKER_READY)) {
    if (specialTapCount == 0 || now - specialTapWindowStart > SPECIAL_TAP_WINDOW_MS) {
      specialTapCount = 0;
      specialTapWindowStart = now;
    }

    specialTapCount++;

    if (specialTapCount >= 3) {
      if (hackerShip.state == HACKER_READY) {
        startHackerDiversion();
      }
      specialTapCount = 0;
      specialTapWindowStart = 0;
    }
  }

  // Wait until the 500 ms window expires before interpreting exactly two taps
  // as Missile Carrier activation. This prevents a possible third tap from
  // being split across two actions.
  if (specialTapCount == 2 &&
      now - specialTapWindowStart > SPECIAL_TAP_WINDOW_MS) {
    if (missileEscort.state == PROTECTING) {
      fireAllMissiles();
    }
    specialTapCount = 0;
    specialTapWindowStart = 0;
  }

  // Clear an incomplete single tap once its timing window expires.
  if (specialTapCount == 1 &&
      now - specialTapWindowStart > SPECIAL_TAP_WINDOW_MS) {
    specialTapCount = 0;
    specialTapWindowStart = 0;
  }

  // --------------------------------------------------
  // NORMAL PLAYER SHOOTING
  // Holding FIRE still shoots normally
  // --------------------------------------------------
  if (buttonPressed &&
      ammo > 0 &&
      (now - lastFireTime >= FIRE_RATE_MS)) {

    for (int i = 0; i < MAX_BULLETS; i++) {

      if (!bullets[i].active) {

        bullets[i].x = playerX + (SHIP_SIZE / 2) - 1;
        bullets[i].y = playerY;
        bullets[i].active = true;

        lastFireTime = now;
        lastShotTime = now;

        ammo--;

        isAutoReloading = false;

        triggerLaserSFX();

        break;
      }
    }
  }

  // --------------------------------------------------
  // AUTO RELOAD
  // --------------------------------------------------
  if (ammo <= 0 && !isAutoReloading) {
    isAutoReloading = true;
    autoReloadStartTime = now;
  }

  if (ammo < MAX_AMMO &&
      !isAutoReloading &&
      (now - lastShotTime >= INACTIVITY_RELOAD_DELAY)) {

    isAutoReloading = true;
    autoReloadStartTime = now-1000;
  }

  if (isAutoReloading) {

    if (now - autoReloadStartTime >= RELOAD_DURATION_MS) {

      ammo = MAX_AMMO;
      isAutoReloading = false;
    }
  }

  // Remember current button state
  lastSwState = swState;

  // --------------------------------------------------
  // UPDATE PLAYER BULLETS
  // --------------------------------------------------
  for (int i = 0; i < MAX_BULLETS; i++) {

    if (bullets[i].active) {

      bullets[i].y -= 4;

      if (bullets[i].y < 0) {
        bullets[i].active = false;
      }
    }
  }
}
void draw_level_hud() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("L");
  display.print(currentLevel + 1);
  display.print("/");
  display.print(totalLevels);

  display.setCursor(32, 0);
  if (isAutoReloading) {
    unsigned long remaining = (RELOAD_DURATION_MS - (millis() - autoReloadStartTime)) / 1000 + 1;
    display.print("R:");
    display.print(remaining);
    display.print("s");
  } else {
    display.print(ammo);
    display.print("/");
    display.print(MAX_AMMO);
  }

  int hudY = 8;
  bool anyTracking = false;
  for (int i = 0; i < MAX_BOSSES; i++) {
    if (!bosses[i].active) continue;
    int bar = map(bosses[i].hp, 0, bosses[i].maxHp, 0, 24);
    display.drawRect(20, hudY, 24, 3, WHITE);
    display.fillRect(20, hudY, bar, 3, WHITE);
    if (bosses[i].attackPhase == BOSS_TRACK) anyTracking = true;
    hudY += 5;
  }
  if (anyTracking && (millis() / 150) % 2 == 0) {
    display.setCursor(46, 8);
    display.print("!");
  }

  if (waveState == WS_LEVEL_CLEAR) {
    display.setCursor(4, 55);
    display.print("LEVEL ");
    display.print(currentLevel + 1);
    display.setCursor(4, 67);
    display.print("COMPLETE!");
  }
}

void draw_bosses() {
  for (int i = 0; i < MAX_BOSSES; i++) {
    if (!bosses[i].active) continue;
    Boss &boss = bosses[i];

    if (boss.attackPhase == BOSS_TRACK) {
      // Blinking warning line so the player has a chance to get clear before it fires
      if ((millis() / 150) % 2 == 0) {
        display.drawFastVLine(boss.x + 12, boss.y + 16, PLAY_H - (boss.y + 16), WHITE);
      }
    } else if (boss.attackPhase == BOSS_LASER) {
      // The actual beam - solid, and its x re-centers every frame as it tracks
      display.fillRect(boss.laserX, boss.y + 16, LASER_WIDTH, PLAY_H - (boss.y + 16), WHITE);
    }

    display.drawBitmap(boss.x, boss.y, bossSprites[boss.spriteIndex], 24, 16, WHITE);
  }
}

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  Wire.begin(21, 22);
  Wire.setClock(400000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(3);
  pinMode(JOY_SW, INPUT_PULLUP);
  resetWaveSystem();
  selectLevelMusic();
  // Shield Escort is intentionally left INACTIVE here (resetWaveSystem()
  // already set it that way) - it should stay locked until it's unlocked
  // at Level 1 Wave 6, not be available from the very start.
  
}
int introStartTime;
void loop() {
  display.clearDisplay();

  switch (gameState) {

    case STATE_TITLE:
      introStartTime = millis();
      draw_title_screen();
      if (digitalRead(JOY_SW) == LOW) {
        gameState = STATE_INTRO;
      }
      break;

    case STATE_INTRO:
  draw_story_screen(introLines, introLineCount);

  if (millis() - introStartTime >= 5000) {
    gameState = STATE_LEVEL_INTRO;
    levelIntroStartTime = millis();
  }

  break;

    case STATE_LEVEL_INTRO:
      draw_story_screen(levelStories[currentLevel], levelStoryLineCounts[currentLevel]);
      if (advance_pressed() || millis() - levelIntroStartTime >= LEVEL_INTRO_AUTO_MS) {
        gameState = STATE_PLAYING;
        // Level 1's Cruiser rush cutscene now plays right as the level
        // begins (before any waves spawn), instead of mid-wave.
        if (currentLevel == 0 && !level1BossRushDone) {
          startLevel1BossRush();
        }
      }
      break;

    case STATE_PLAYING: {
      updateAudio();
      updateMovement();
      if (!bossRushActive) {
        handleShooting();
        
        updateMissileCarrier();
        EnemyManagement();
        updateEnemyBullets();
        updateMissiles();
        powerup_management();
        updateMissileQueue();
        updateEscortShip();
        updateHackerShip();
        // Run the hacker after enemy movement so it can actually pull
        // enemies/bullets off-screen for the whole diversion window.
        
      } 
      else {
        updateLevel1BossRush();
      }
      
      // Render Player Bullets
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) display.drawBitmap(bullets[i].x, bullets[i].y, bullet, 2, 4, WHITE);
      }

      // Render Enemy Bullets
      for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) display.drawBitmap(enemyBullets[i].x, enemyBullets[i].y, bullet, 2, 4, WHITE);
      }

      // Render Homing Missiles
      for (int i = 0; i < MAX_MISSILES; i++) {
        if (missiles[i].active) display.fillRect((int)missiles[i].x, (int)missiles[i].y, 2, 3, WHITE);
      }

      // Render Powerups
      for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powers[i].available) display.drawBitmap(powers[i].x, powers[i].y, powers[i].sprite, SHIP_SIZE, SHIP_SIZE, WHITE);
      }

      // Render Enemies
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive) display.drawBitmap(enemies[i].x, enemies[i].y, enemyShipScout, SHIP_SIZE, SHIP_SIZE, WHITE);
      }

      // Render the scripted Level 1 Cruiser rush.
      if (bossRushActive) {
        display.drawBitmap((int)bossRushX, (int)bossRushY, boss1_Cruiser, 24, 16, WHITE);
      }

      // Render Bosses (sprite(s) + laser telegraph/beam)
      draw_bosses();

      // Render Shield Escort + its own small HP bar
      if (escort.state != INACTIVE) {
        display.drawBitmap((int)escort.x, (int)escort.y, powerupShipShield, SHIP_SIZE, SHIP_SIZE, WHITE);
        if (escort.state == PROTECTING) {
          display.drawCircle(playerX + 8, playerY, 20, WHITE);
          int value = map(escort.hp, 0, 30, 0, 10);
          display.drawRect((int)escort.x + 10, (int)escort.y - 4, 10, 3, WHITE);
          display.fillRect((int)escort.x + 10, (int)escort.y - 4, value, 3, WHITE);
        }
      }

      // Render Missile Carrier + its missile-count bar
      if (missileEscort.state != INACTIVE) {
        display.drawBitmap((int)missileEscort.x, (int)missileEscort.y, ship_MissileCarrier, SHIP_SIZE, SHIP_SIZE, WHITE);
        if (missileEscort.state == PROTECTING) {
          int value = map(missileAmmo, 0, MAX_MISSILE_AMMO, 0, 10);
          display.drawRect((int)missileEscort.x + 10, (int)missileEscort.y - 4, 10, 3, WHITE);
          display.fillRect((int)missileEscort.x + 10, (int)missileEscort.y - 4, value, 3, WHITE);
        }
      }

      // Render Hacker Ship on the player's left side.
      if (hackerShip.state != HACKER_INACTIVE) {
        display.drawBitmap((int)hackerShip.x, (int)hackerShip.y, ship_Hacker, SHIP_SIZE, SHIP_SIZE, WHITE);
      }

      player_health_bar();
      display.drawBitmap(playerX, playerY, playerShipUp, SHIP_SIZE, SHIP_SIZE, WHITE);
      draw_level_hud();
      draw_mid_wave_dialogue();
      break;
    }

    case STATE_GAME_OVER:
      game_over();
      break;

    case STATE_VICTORY:
      draw_story_screen(victoryLines, victoryLineCount);
      if (advance_pressed()) {
        resetWaveSystem();
        gameState = STATE_LEVEL_INTRO;
        levelIntroStartTime = millis();
      }
      break;
  }

  display.display();
  delay(5);
}
