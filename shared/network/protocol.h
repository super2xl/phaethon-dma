#pragma once
#include "../math/vector.h"
#include <cstdint>
#include <cstring>

namespace phaethon {
namespace protocol {

using namespace math;

constexpr uint32_t MAGIC = 0xFAE7401; // PHAETHON in hex-ish
constexpr uint16_t VERSION = 1;
constexpr uint16_t DEFAULT_PORT = 31337;
constexpr int MAX_PLAYERS = 64;

// Player flags (bitfield)
enum PlayerFlags : uint32_t {
    FLAG_ALIVE      = (1 << 0),
    FLAG_VISIBLE    = (1 << 1),
    FLAG_SCOPED     = (1 << 2),
    FLAG_DEFUSING   = (1 << 3),
    FLAG_PLANTING   = (1 << 4),
    FLAG_RELOADING  = (1 << 5),
    FLAG_FLASHED    = (1 << 6),
    FLAG_ON_GROUND  = (1 << 7),
    FLAG_DUCKING    = (1 << 8),
};

#pragma pack(push, 1)

// Single player data structure
struct PlayerData {
    Vector3 position;
    Vector3 head_position;
    Vector3 velocity;

    int32_t health;
    int32_t armor;
    int32_t team;           // 2 = Terrorist, 3 = Counter-Terrorist

    uint32_t flags;         // Bitfield: alive, visible, scoped, etc

    uint32_t weapon_id;     // Current weapon
    int32_t shots_fired;    // Recoil counter

    char name[32];          // Player name (null-terminated)

    // Default constructor
    PlayerData() {
        std::memset(this, 0, sizeof(PlayerData));
    }
};

// Main packet: laptop -> desktop (game state)
struct OverlayPacket {
    uint32_t magic;         // Packet validation
    uint16_t version;       // Protocol version
    uint16_t player_count;  // Number of valid players
    uint32_t timestamp;     // Milliseconds since start

    // View matrix for WorldToScreen calculations
    float view_matrix[16];

    // Player array
    PlayerData players[MAX_PLAYERS];

    // Local player data
    Vector3 local_position;
    Vector3 local_angles;
    int32_t local_team;
    int32_t local_health;

    // Game state
    bool is_in_game;
    int32_t map_hash;       // Simple map identifier

    // Screen dimensions (for proper scaling)
    int32_t screen_width;
    int32_t screen_height;

    // Default constructor
    OverlayPacket() {
        magic = MAGIC;
        version = VERSION;
        player_count = 0;
        timestamp = 0;
        std::memset(view_matrix, 0, sizeof(view_matrix));
        local_team = 0;
        local_health = 0;
        is_in_game = false;
        map_hash = 0;
        screen_width = 1920;
        screen_height = 1080;
    }

    bool IsValid() const {
        return magic == MAGIC && version == VERSION;
    }
};

// Configuration packet: desktop -> laptop (feature settings)
struct ConfigPacket {
    uint32_t magic;
    uint16_t version;

    // Feature toggles
    bool esp_enabled;
    bool aimbot_enabled;
    bool triggerbot_enabled;
    bool rcs_enabled;           // Recoil control

    // ESP settings
    bool esp_box;
    bool esp_skeleton;
    bool esp_health;
    bool esp_name;
    bool esp_distance;
    bool esp_weapon;
    bool esp_snaplines;

    // ESP colors (RGBA)
    uint32_t color_team_visible;
    uint32_t color_team_occluded;
    uint32_t color_enemy_visible;
    uint32_t color_enemy_occluded;

    // Aimbot settings
    float aimbot_fov;           // FOV limit in degrees
    float aimbot_smooth;        // Smoothing factor (0-1, higher = slower)
    uint8_t aimbot_bone;        // 0=head, 1=neck, 2=chest, 3=pelvis
    uint8_t aimbot_key;         // VK_* keycode (e.g., VK_XBUTTON1)
    bool aimbot_visibility_check;
    bool aimbot_team_check;

    // Triggerbot settings
    uint32_t trigger_delay_ms;
    uint8_t trigger_key;

    // RCS settings
    float rcs_amount;           // 0-1, how much recoil to compensate

    // Default constructor
    ConfigPacket() {
        magic = MAGIC;
        version = VERSION;

        esp_enabled = true;
        aimbot_enabled = false;
        triggerbot_enabled = false;
        rcs_enabled = false;

        esp_box = true;
        esp_skeleton = true;
        esp_health = true;
        esp_name = true;
        esp_distance = true;
        esp_weapon = false;
        esp_snaplines = false;

        // Default colors (RGBA hex format)
        color_team_visible = 0x00FF00FF;    // Green
        color_team_occluded = 0x00FF0080;   // Green semi-transparent
        color_enemy_visible = 0xFF0000FF;   // Red
        color_enemy_occluded = 0xFF000080;  // Red semi-transparent

        aimbot_fov = 5.f;
        aimbot_smooth = 0.3f;
        aimbot_bone = 0; // Head
        aimbot_key = 0x05; // VK_XBUTTON1 (mouse button 4)
        aimbot_visibility_check = true;
        aimbot_team_check = true;

        trigger_delay_ms = 50;
        trigger_key = 0x06; // VK_XBUTTON2 (mouse button 5)

        rcs_amount = 0.8f;
    }

    bool IsValid() const {
        return magic == MAGIC && version == VERSION;
    }
};

// Heartbeat packet (bidirectional - keep connection alive)
struct HeartbeatPacket {
    uint32_t magic;
    uint16_t version;
    uint32_t timestamp;

    HeartbeatPacket() {
        magic = MAGIC;
        version = VERSION;
        timestamp = 0;
    }

    bool IsValid() const {
        return magic == MAGIC && version == VERSION;
    }
};

#pragma pack(pop)

// Packet size validation
static_assert(sizeof(OverlayPacket) < 65507, "OverlayPacket too large for UDP");
static_assert(sizeof(ConfigPacket) < 1024, "ConfigPacket should be small");

} // namespace protocol
} // namespace phaethon
