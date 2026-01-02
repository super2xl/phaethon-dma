#pragma once
#include <cstdint>

namespace phaethon {
namespace offsets {

// CS2 Module Offsets
// These will be updated via offset dumper tool or manual pattern scanning
// Last updated: TBD

namespace client_dll {
    // Entity System
    constexpr uintptr_t dwEntityList = 0x0; // TODO: Update with actual offset
    constexpr uintptr_t dwLocalPlayerController = 0x0;
    constexpr uintptr_t dwLocalPlayerPawn = 0x0;
    constexpr uintptr_t dwViewMatrix = 0x0;
    constexpr uintptr_t dwViewAngles = 0x0;

    // Game State
    constexpr uintptr_t dwGameRules = 0x0;
    constexpr uintptr_t dwGlobalVars = 0x0;

    // Input
    constexpr uintptr_t dwSensitivity = 0x0;
    constexpr uintptr_t dwMouseEnable = 0x0;
}

namespace engine_dll {
    // Window / Screen
    constexpr uintptr_t dwWindowWidth = 0x0;
    constexpr uintptr_t dwWindowHeight = 0x0;

    // Network
    constexpr uintptr_t dwNetworkGameClient = 0x0;
}

// Schema Offsets (from CSchemaSystem)
// These are relative to entity base addresses
namespace schema {
    // C_BaseEntity
    namespace CBaseEntity {
        constexpr uint32_t m_pGameSceneNode = 0x0;
        constexpr uint32_t m_iHealth = 0x0;
        constexpr uint32_t m_iTeamNum = 0x0;
        constexpr uint32_t m_fFlags = 0x0;
        constexpr uint32_t m_vecVelocity = 0x0;
        constexpr uint32_t m_lifeState = 0x0;
        constexpr uint32_t m_pCollision = 0x0;
    }

    // CGameSceneNode
    namespace CGameSceneNode {
        constexpr uint32_t m_vecAbsOrigin = 0x0;
        constexpr uint32_t m_angRotation = 0x0;
        constexpr uint32_t m_bDormant = 0x0;
        constexpr uint32_t m_pBoneArray = 0x0;
        constexpr uint32_t m_modelState = 0x0;
    }

    // C_CSPlayerPawn
    namespace CCSPlayerPawn {
        constexpr uint32_t m_iHealth = 0x0;
        constexpr uint32_t m_ArmorValue = 0x0;
        constexpr uint32_t m_iShotsFired = 0x0;
        constexpr uint32_t m_bIsScoped = 0x0;
        constexpr uint32_t m_angEyeAngles = 0x0;
        constexpr uint32_t m_aimPunchAngle = 0x0;
        constexpr uint32_t m_aimPunchCache = 0x0;
        constexpr uint32_t m_pWeaponServices = 0x0;
        constexpr uint32_t m_pItemServices = 0x0;
        constexpr uint32_t m_pCameraServices = 0x0;
        constexpr uint32_t m_entitySpottedState = 0x0;
        constexpr uint32_t m_bSpotted = 0x0;
    }

    // CCSPlayerController
    namespace CCSPlayerController {
        constexpr uint32_t m_hPawn = 0x0;
        constexpr uint32_t m_sSanitizedPlayerName = 0x0;
        constexpr uint32_t m_bPawnIsAlive = 0x0;
        constexpr uint32_t m_iPing = 0x0;
        constexpr uint32_t m_iScore = 0x0;
        constexpr uint32_t m_iMVPs = 0x0;
    }

    // CCollisionProperty
    namespace CCollisionProperty {
        constexpr uint32_t m_vecMins = 0x0;
        constexpr uint32_t m_vecMaxs = 0x0;
        constexpr uint32_t m_nSolidType = 0x0;
        constexpr uint32_t m_usSolidFlags = 0x0;
    }

    // CPlayerWeaponServices
    namespace CPlayerWeaponServices {
        constexpr uint32_t m_hActiveWeapon = 0x0;
        constexpr uint32_t m_hMyWeapons = 0x0;
    }

    // C_BasePlayerWeapon
    namespace CBasePlayerWeapon {
        constexpr uint32_t m_AttributeManager = 0x0;
        constexpr uint32_t m_iClip1 = 0x0;
        constexpr uint32_t m_iClip2 = 0x0;
        constexpr uint32_t m_pReserveAmmo = 0x0;
    }

    // CEntityInstance
    namespace CEntityInstance {
        constexpr uint32_t m_pEntity = 0x0;
        constexpr uint32_t m_nReferenceCount = 0x0;
    }

    // CEntityIdentity
    namespace CEntityIdentity {
        constexpr uint32_t m_pInstance = 0x0;
        constexpr uint32_t m_name = 0x0;
        constexpr uint32_t m_designerName = 0x0;
        constexpr uint32_t m_flags = 0x0;
        constexpr uint32_t m_pPrev = 0x0;
        constexpr uint32_t m_pNext = 0x0;
        constexpr uint32_t m_pPrevByClass = 0x0;
        constexpr uint32_t m_pNextByClass = 0x0;
    }
}

// Bone indices
namespace bones {
    constexpr int HEAD = 6;
    constexpr int NECK = 5;
    constexpr int CHEST = 4;
    constexpr int PELVIS = 0;

    constexpr int LEFT_SHOULDER = 8;
    constexpr int LEFT_ELBOW = 9;
    constexpr int LEFT_HAND = 10;

    constexpr int RIGHT_SHOULDER = 13;
    constexpr int RIGHT_ELBOW = 14;
    constexpr int RIGHT_HAND = 15;

    constexpr int LEFT_HIP = 22;
    constexpr int LEFT_KNEE = 23;
    constexpr int LEFT_ANKLE = 24;

    constexpr int RIGHT_HIP = 25;
    constexpr int RIGHT_KNEE = 26;
    constexpr int RIGHT_ANKLE = 27;
}

// Team IDs
namespace teams {
    constexpr int SPECTATOR = 1;
    constexpr int TERRORIST = 2;
    constexpr int COUNTER_TERRORIST = 3;
}

// Life state
namespace lifestate {
    constexpr int ALIVE = 0;
    constexpr int DYING = 1;
    constexpr int DEAD = 2;
}

// Entity flags
namespace entity_flags {
    constexpr uint32_t FL_ONGROUND = (1 << 0);
    constexpr uint32_t FL_DUCKING = (1 << 1);
    constexpr uint32_t FL_ANIMDUCKING = (1 << 2);
    constexpr uint32_t FL_WATERJUMP = (1 << 3);
    constexpr uint32_t FL_ONTRAIN = (1 << 4);
    constexpr uint32_t FL_INRAIN = (1 << 5);
    constexpr uint32_t FL_FROZEN = (1 << 6);
    constexpr uint32_t FL_ATCONTROLS = (1 << 7);
    constexpr uint32_t FL_CLIENT = (1 << 8);
    constexpr uint32_t FL_FAKECLIENT = (1 << 9);
}

} // namespace offsets
} // namespace phaethon
