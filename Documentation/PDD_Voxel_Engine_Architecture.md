# PDD Voxel Engine (pdd-voxel)

## Initial Architecture, Coding Goals, and Godot Integration

This document defines **what pdd-voxel is**, **what it is not**, and the concrete coding goals required to move from a bare-bones GDExtension to a working, streamed 3D voxel terrain inside Godot.

It is intentionally structured as an **engineering contract** rather than a tutorial: each section describes responsibilities, interfaces, and success criteria so progress can be validated incrementally.

---

## 1. Division of Responsibility

### 1.1 Godot Responsibilities (Scene / GDScript Layer)

Godot should remain responsible for high-level gameplay and presentation:

- Player controller and camera  
- Input handling  
- UI, debug overlays, and tooling  
- Lighting, sky, and environment  
- Non-terrain entities (props, NPCs, foliage)  
- Save/load of game state *excluding* raw voxel data  

Godot **consumes** terrain; it does not generate or manage it.

---

### 1.2 pdd-voxel Responsibilities (C++ GDExtension)

pdd-voxel owns all computationally heavy and structural terrain systems:

- World coordinate system (planet-scale abstraction)  
- Chunk addressing and spatial indexing  
- Terrain streaming (load / unload)  
- Procedural generation (density, height, biome)  
- Mesh extraction from voxel data  
- Collision mesh generation  
- Voxel editing (dig/build) and delta persistence  
- Level-of-detail (LOD) generation and seam handling  
- Threading and performance isolation  

pdd-voxel must expose a **clean, minimal interface** to Godot.

---

## 2. Core Architectural Concept

pdd-voxel provides **one primary Node** that Godot can place in a scene:

```
PDDVoxelTerrain : Node3D
```

From Godot’s perspective:
- The terrain is just another node  
- The player moves  
- Terrain appears and disappears automatically  

All complexity remains internal to the extension.

---

## 3. The Minimal Vertical Slice (Critical Path)

This is the *non-negotiable* implementation order. Each goal must be runnable and visually verifiable before proceeding.

---

### Goal 1 — Terrain Manager Node

Create a GDExtension class:

```
class PDDVoxelTerrain : public Node3D
```

Expose the following properties to the Godot Inspector:

- seed : int  
- planet_radius : float  
- chunk_world_size : float  
- view_distance_chunks : int  
- lod_enabled : bool (initially unused)  
- generate_collisions : bool  
- terrain_material : Material  
- player_node : NodePath  

**Success Criteria**:
- Node can be added to a scene  
- Properties appear in Inspector  
- Node runs without errors and logs basic state  

---

### Goal 2 — Chunk Key System and Streaming Ring

Implement a stable, hashable **ChunkKey**:

- Cartesian (x, y, z) chunks initially  
- Deterministic and reproducible from world position  

Implement:

```
Set<ChunkKey> get_required_chunks(player_position)
```

Then:
- Compute `required - loaded` → load  
- Compute `loaded - required` → unload  

**Success Criteria**:
- Moving the player prints chunk load/unload events  
- No duplicate loads  
- No memory leaks  

---

### Goal 3 — Density Field Generation (Simple First)

Before planetary math, use a trivial generator:

Examples:
- Flat plane: density = y - height  
- Sphere shell: density = radius - length(position)  
- Optional noise modulation  

This generator must be:
- Deterministic  
- Stateless  
- Purely a function of (ChunkKey, seed)  

**Success Criteria**:
- Each chunk generates consistent voxel data  
- Regeneration produces identical results  

---

### Goal 4 — Mesh Extraction

Select **one** algorithm initially:

- Marching Cubes **or**  
- Surface Nets  

Avoid Dual Contouring until editing and seams matter.

Output:
- Raw vertex buffers  
- Index buffers  
- Optional normals  

**Success Criteria**:
- Meshes are watertight  
- Correct orientation and scale  
- No visible cracks *within* a chunk  

---

### Goal 5 — Thread-Safe Mesh Upload

Required threading model:

- Worker threads:
  - Density generation  
  - Mesh extraction  

- Main thread:
  - Creation of ArrayMesh  
  - Assignment to MeshInstance3D  
  - Node attachment / removal  

**Rule**: Godot scene objects are main-thread only.

**Success Criteria**:
- No stuttering during movement  
- No crashes from cross-thread access  

---

### Goal 6 — Collision Generation (Early but Optional)

Initial collision strategy:

- ConcavePolygonShape3D per chunk  

Later optimization can replace this.

**Success Criteria**:
- Player can stand on terrain  
- Collision aligns with rendered mesh  

---

## 4. How Godot Uses pdd-voxel

From the Godot side, usage should be intentionally boring.

### Scene Setup

```
World (Node3D)
├── Player (CharacterBody3D)
├── Camera3D
└── PDDVoxelTerrain
```

### Runtime Flow

- Godot updates player position  
- PDDVoxelTerrain reads position  
- Chunks stream automatically  
- Meshes appear/disappear  

No gameplay logic touches voxel internals.

---

## 5. Planetary World Implications (Future-Safe Design)

### 5.1 Coordinate Abstraction

A clean separation between:
- Godot world space (meters)  
- Planet space (lat/long/height or face + barycentric + height)  

All generation logic must depend on **planet space**, not Godot transforms.

---

### 5.2 Chunk Identity Stability

Planet chunks must:
- Never drift  
- Never change keys as the player moves  
- Be addressable independently of floating origin  

This allows:
- Streaming  
- Save/load  
- Delta application  

---

## 6. Recommended Milestones

### Milestone A — Streaming Proof
- Cartesian chunks  
- Simple generator  
- Basic meshing  
- Streaming ring  

### Milestone B — Editing
- Voxel modification  
- Per-chunk deltas  
- Deterministic regen + delta replay  

### Milestone C — Planet Integration
- Icosahedron / triangular chunking  
- Planet height & biome functions  
- Material layers  

### Milestone D — Performance & LOD
- Multi-resolution meshes  
- Seam stitching  
- Prioritized background jobs  

---

## 7. Immediate Next Action

Implement **Milestone A** only.

Create `PDDVoxelTerrain` as a Node3D that streams simple cube chunks around the player and displays visible meshes.

This single vertical slice establishes:
- Godot ↔ GDExtension integration  
- Chunk lifecycle correctness  
- Thread safety discipline  

Everything else builds cleanly on top of it.
