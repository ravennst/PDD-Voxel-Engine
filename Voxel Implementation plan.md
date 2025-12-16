# PDD Voxel Engine – Working Specification Summary

This document summarizes the **current assumptions, constraints, and derived specifications** agreed upon so far in the voxel-structure brainstorming thread. It is intentionally scoped to *what has been decided*, not future design work.

---

## 1. Planetary Scale Assumptions

- **Canonical planet:** Dezengarth  
- **Adjusted diameter:** ~11,012 km (derived for clean divisibility)  
- **Adjusted radius:** ~5,506 km  

These values were chosen to allow integer-divisible surface tiling without materially affecting lore or gameplay.

---

## 2. Mother-Chunk (Planet Topology Layer)

- Planet surface is partitioned into **10 rhombus-shaped mother-chunks** (icosahedron-derived).
- Mother-chunks:
  - Contain **no voxel data**
  - Exist purely to define **orientation, seams, and coordinate transforms**
  - Handle X/Y flips and axis swaps near poles and edges

### Mother-Chunk Edge Geometry

- **Arc angle per edge:** acos(√5 / 5)
- **Final chosen arc length per edge:** **6096 km**

---

## 3. Macro / Parent Octree Chunk Layer

- Each mother-chunk edge is divided into **381 equal segments**
- **Macro (parent) chunk size:** **16 km × 16 km**

### Counts

- **Parent chunks per mother-chunk:**  
  381 × 381 = **145,161**

- **Parent chunks planet-wide:**  
  **1,451,610**

---

## 4. Vertical (Radial) Constraints

- **Workable voxel band:** ±8 km around sea level  
  (16 km total)

- **Molten core assumption**:
  - Prevents full-radius mining
  - Limits radial XY voxel scaling distortion

- Horizontal voxel scale variance across band: **~0.3%**

---

## 5. Voxel Resolution & Addressing

- **Canonical terrain voxel size:** **1.0 meter**
- Smaller resolutions (0.5 m / 0.25 m) are not global and are only used via local refinement

### Addressing Rules

- Voxel identity uses **integers only**
  - Rhombus ID
  - Chunk indices
  - Local voxel indices

- World positions are **derived using floating point**
- No global 1 m voxel coordinate system exists

---

## 6. Leaf Chunks

- **Leaf chunk size:** **32 m × 32 m × 32 m**
- Each leaf chunk contains:
  - 32 × 32 × 32 = **32,768 voxels**

- Octree subdivision **stops at leaf chunks**
- Voxels only exist inside leaf chunks

---

## 7. Near-Field Voxel Bubble

- **True voxel representation radius:** **8 leaf chunks**
  - 8 × 32 m = **256 m** (≈ 250 m target)

- Shape: spherical around the player

- Approximate horizontal chunk count:
  - **~200–300 leaf chunks**

---

## 8. Air, Visibility, and Streaming

- **Open air:** implicit, zero data
- **Sky:** no voxel data
- **Underground:** voxels loaded only near the player

### Streaming Policy

- Load radius = **8 chunks**
- Unload radius = **9 chunks** (hysteresis)
- Rendered chunks are a subset of loaded chunks:
  - View frustum
  - Occlusion (players cannot see through walls)

---

## 9. Far-Field Representation

- Beyond the voxel bubble:
  - No voxel data
  - Procedural surface mesh only
  - Distance-based LOD reduction

- Far-field meshes are generated from planetary math, not voxel data

---

## 10. Numeric Safety

- All chunk and voxel addressing fits comfortably within **64-bit integers**
- Practical limits are:
  - Memory
  - Streaming bandwidth
  - Meshing cost

- Integer range is **not** a limiting factor

---

## 11. Design Philosophy

- Integers define *what exists*
- Floats define *where it is drawn*
- High-resolution detail is **local, procedural, and temporary**
- Planet-scale terrain is handled via **topology + streaming**, not monolithic voxel grids

---

**Status:**  
This represents a stable conceptual foundation suitable for moving into prototype-level implementation (chunk keys, generators, streaming logic).