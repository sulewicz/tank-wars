#include "models.h"

#define DECLARE_MODEL(v, t)                                                    \
  {.verts = v, .tris = t, .tris_count = sizeof(t) / sizeof(t[0]) / 3}

const vec3f_t flag_verts[3] = {
    {0.f, 0.f, 0.f}, {10.f, 0.f, 0.f}, {0.f, 10.f, 0}};
const uint32_t flag_tris[1 * 3] = {0, 1, 2};
model_t flag_model = DECLARE_MODEL(flag_verts, flag_tris);

// Cube model (edge length 10, bottom at y = 0, centered at x=0, z=0)
const vec3f_t cube_verts[8] = {
    {-5.f, 0.f, -5.f},  {5.f, 0.f, -5.f},
    {5.f, 0.f, 5.f},    {-5.f, 0.f, 5.f}, // bottom face
    {-5.f, 10.f, -5.f}, {5.f, 10.f, -5.f},
    {5.f, 10.f, 5.f},   {-5.f, 10.f, 5.f} // top face
};

const uint32_t cube_tris[12 * 3] = {
    // Bottom face
    0, 1, 2, 0, 2, 3,
    // Top face
    4, 6, 5, 4, 7, 6,
    // Front face
    0, 4, 5, 0, 5, 1,
    // Back face
    2, 6, 7, 2, 7, 3,
    // Left face
    0, 3, 7, 0, 7, 4,
    // Right face
    1, 5, 6, 1, 6, 2};

model_t cube_model = DECLARE_MODEL(cube_verts, cube_tris);

// Tank model: body, turret, and barrel (centered at x=0, z=0, bottom at y=0)

const vec3f_t tank_verts[24] = {
    // Body (wider, flatter)
    {-10.f, 0.f, -5.f}, {10.f, 0.f, -5.f},
    {10.f, 0.f, 5.f},   {-10.f, 0.f, 5.f},
    {-10.f, 5.f, -5.f}, {10.f, 5.f, -5.f},
    {10.f, 5.f, 5.f},   {-10.f, 5.f, 5.f},
    
    // Turret (smaller cube on top)
    {-4.f, 5.f, -4.f},  {4.f, 5.f, -4.f},
    {4.f, 5.f, 4.f},    {-4.f, 5.f, 4.f},
    {-4.f, 10.f, -4.f}, {4.f, 10.f, -4.f},
    {4.f, 10.f, 4.f},   {-4.f, 10.f, 4.f},
    
    // Barrel (thin stretched box pointing +X)
    {4.f, 7.f, -1.f}, {14.f, 7.f, -1.f},
    {14.f, 7.f, 1.f}, {4.f, 7.f, 1.f},
    {4.f, 9.f, -1.f}, {14.f, 9.f, -1.f},
    {14.f, 9.f, 1.f}, {4.f, 9.f, 1.f}
};

const uint32_t tank_tris[36 * 3] = {
    // Body bottom face
    0, 1, 2, 0, 2, 3,
    // Body top face
    4, 6, 5, 4, 7, 6,
    // Body front face
    0, 4, 5, 0, 5, 1,
    // Body back face
    2, 6, 7, 2, 7, 3,
    // Body left face
    0, 3, 7, 0, 7, 4,
    // Body right face
    1, 5, 6, 1, 6, 2,

    // Turret bottom face
    8, 9, 10, 8, 10, 11,
    // Turret top face
    12, 14, 13, 12, 15, 14,
    // Turret front face
    8, 12, 13, 8, 13, 9,
    // Turret back face
    10, 14, 15, 10, 15, 11,
    // Turret left face
    8, 11, 15, 8, 15, 12,
    // Turret right face
    9, 13, 14, 9, 14, 10,

    // Barrel bottom face
    16, 17, 18, 16, 18, 19,
    // Barrel top face
    20, 22, 21, 20, 23, 22,
    // Barrel left face
    16, 19, 23, 16, 23, 20,
    // Barrel right face
    17, 21, 22, 17, 22, 18,
    // Barrel top face (again for completeness)
    19, 18, 22, 19, 22, 23,
    // Barrel bottom face (again for completeness)
    16, 20, 21, 16, 21, 17
};

model_t tank_model = DECLARE_MODEL(tank_verts, tank_tris);

// Projectile model: small stretched box pointing along +X axis

const vec3f_t projectile_verts[8] = {
    // Back face (starting at x = 0)
    {0.f, 7.5f, -0.5f}, {0.f, 7.5f, 0.5f},
    {0.f, 8.5f, 0.5f}, {0.f, 8.5f, -0.5f},
    
    // Front face (x = +4)
    {4.f, 7.5f, -0.5f}, {4.f, 7.5f, 0.5f},
    {4.f, 8.5f, 0.5f}, {4.f, 8.5f, -0.5f}
};

const uint32_t projectile_tris[12 * 3] = {
    // Back face
    0, 1, 2, 0, 2, 3,
    // Front face
    4, 7, 6, 4, 6, 5,
    // Top face
    3, 2, 6, 3, 6, 7,
    // Bottom face
    0, 4, 5, 0, 5, 1,
    // Left face
    0, 3, 7, 0, 7, 4,
    // Right face
    1, 5, 6, 1, 6, 2
};

model_t projectile_model = DECLARE_MODEL(projectile_verts, projectile_tris);

// Diamond explosion model with two perpendicular flat diamonds
// The first diamond is in the XY plane
// The second diamond is in the XZ plane

// Actually don't need to redeclare shared vertices
const vec3f_t explosion_diamond_verts[7] = {
    // Center point
    {0.0f, 0.0f, 0.0f},
    
    // First diamond (XY plane)
    {1.0f, 0.0f, 0.0f},   // Right
    {0.0f, 1.0f, 0.0f},   // Top
    {-1.0f, 0.0f, 0.0f},  // Left
    {0.0f, -1.0f, 0.0f},  // Bottom
    
    // Second diamond (XZ plane) - perpendicular to first (only unique vertices)
    {0.0f, 0.0f, 1.0f},   // Front
    {0.0f, 0.0f, -1.0f}   // Back
    
    // Third diamond (ZY plane) uses already defined vertices
};

const uint32_t explosion_diamond_tris[12 * 3] = {
    // First diamond (XY plane) - 4 triangles
    0, 1, 2,  // Center -> Right -> Top
    0, 2, 3,  // Center -> Top -> Left
    0, 3, 4,  // Center -> Left -> Bottom
    0, 4, 1,  // Center -> Bottom -> Right
    
    // Second diamond (XZ plane) - 4 triangles
    0, 1, 5,  // Center -> Right -> Front
    0, 5, 3,  // Center -> Front -> Left
    0, 3, 6,  // Center -> Left -> Back
    0, 6, 1,  // Center -> Back -> Right
    
    // Third diamond (ZY plane) - 4 triangles
    0, 5, 2,  // Center -> Front -> Top
    0, 2, 6,  // Center -> Top -> Back
    0, 6, 4,  // Center -> Back -> Bottom
    0, 4, 5   // Center -> Bottom -> Front
};

model_t explosion_model = DECLARE_MODEL(explosion_diamond_verts, explosion_diamond_tris);