# Project Kickoff - __phaethon__ DMA Framework
**Date:** 2026-01-01
**Status:** Initial Setup Phase

## Project Overview
Two-PC DMA framework for SofaLeague CS2 HvH competitive environment.

**Architecture:**
- **Laptop:** DMA memory operations via MemProcFS
- **Desktop:** CS2 client + optional overlay visualization
- **Communication:** UDP packets over local network

## Research Completed

### 1. Andromeda CS2 Base Analysis ✓
**Agent ID:** ad35e09

**Key Findings:**
- Entity system: Two-level (Controller + Pawn) with CHandle resolution
- Offset management: Schema-based system (dump to JSON for DMA)
- Math utilities: WorldToScreen with view matrix, 8-point bounding boxes
- Bone system: 18 key bones via CModelState::m_pBones
- Entity caching: Pre-compute expensive operations

**Adaptable Components:**
- `CS2/SDK/Math/*` - Vector3, QAngle, Matrix utilities (copy directly)
- Entity structure patterns (adapt for remote read)
- CEntityCache system for tracking
- ESP rendering logic
- Bounding box calculation algorithms

**Critical Offsets Identified:**
```cpp
// Entity System
CGameEntitySystem::entityList
CGameEntitySystem::highestEntityIndex

// Schema Offsets
C_BaseEntity::m_pGameSceneNode
C_BaseEntity::m_iHealth
C_BaseEntity::m_iTeamNum
CGameSceneNode::m_vecAbsOrigin
CCSPlayerController::m_hPawn
C_CSPlayerPawn::m_ArmorValue
C_CSPlayerPawn::m_angEyeAngles

// View Matrix
ViewMatrix / GetMatricesForView
```

### 2. MemProcFS Integration Analysis ✓
**Agent ID:** a1eaede

**Key Findings:**
- **Recommendation:** Use DLL linking (NOT static compilation)
- Core API surface identified: VMMDLL_* functions
- Scatter-gather system for batched reads (optimal performance)
- Module enumeration via VMMDLL_ProcessGetModuleBaseU

**Integration Strategy:**
```
laptop/
├── lib/
│   ├── vmm.lib          - Import library
│   └── leechcore.lib
└── bin/
    ├── laptop.exe
    ├── vmm.dll          - Runtime
    ├── leechcore.dll
    └── FTD3XX.dll       - FPGA driver
```

**Critical APIs for CS2:**
- `VMMDLL_PidGetFromName` - Find game process
- `VMMDLL_ProcessGetModuleBaseU` - Get module base
- `VMMDLL_Scatter_*` - Batch read entities (high performance)
- `VMMDLL_MemRead/Write` - Simple operations

## Directory Structure Created
```
phaethon-dma/
├── docs/
│   ├── architecture/    - System design documents
│   ├── progress/        - Development progress tracking
│   ├── api/             - API references and usage
│   └── integration/     - Integration guides
├── shared/
│   ├── network/         - Protocol definitions, socket wrapper
│   ├── math/            - Vector, matrix, geometry utilities
│   ├── offsets/         - CS2 offsets and schema
│   └── utils/           - Config, logging, memory utils
├── laptop/
│   ├── dma/             - MemProcFS wrapper, scatter-gather
│   ├── game/            - CS2 interface, entity management
│   ├── features/        - ESP data, aimbot calculations
│   ├── network/         - Network sender
│   └── config/          - Configuration files
├── desktop/
│   ├── overlay/         - D3D11 overlay, rendering
│   ├── network/         - Network receiver
│   └── hooks/           - D3D hooks for overlay
├── tools/
│   ├── offset_updater/  - Offset update automation
│   └── schema_dumper/   - Schema offset dumper
└── references/
    ├── Andromeda-CS2-Base/  - Reference architecture
    └── MemProcFS/           - MemProcFS source/binaries
```

## Next Steps

1. **Initialize Git Repository** ✓ (in progress)
2. **Create Visual Studio Solution**
   - Solution: __phaethon__.sln
   - Projects: shared (static lib), laptop (console app), desktop (windows app)
3. **Implement Core Libraries**
   - Math utilities from Andromeda
   - Network protocol definitions
   - MemProcFS C++ wrapper
4. **Build CS2 Interface**
   - Entity reading with scatter-gather
   - Offset management system
   - Player data extraction

## Technical Decisions

### MemProcFS Integration
**Decision:** DLL linking (NOT static compilation)
**Rationale:**
- Simpler integration and maintenance
- Avoids AGPL v3.0 derivative work concerns
- Official updates/bugfixes preserved
- Smaller executable size
- **Trade-off:** Requires DLLs to be distributed with laptop.exe

### Project Structure
**Decision:** Visual Studio 2022 solution with 3 projects
**Rationale:**
- Clean separation of concerns
- Shared code reuse (both laptop & desktop)
- Optimized for 7950X3D with parallel builds
- Professional, maintainable architecture

### Offset Management
**Decision:** Pre-dumped JSON offsets (not runtime schema resolution)
**Rationale:**
- Andromeda uses runtime schema system (internal access)
- DMA can't access schema system remotely
- JSON allows easy updates per CS2 patch
- Tools can regenerate offset files

## Philosophy
**"Know your limits"** - Pushing technical boundaries in controlled competitive HvH environments.

Clean, professional codebase. More extensive systems than Andromeda, but better organized.

---
*Last Updated: 2026-01-01*
