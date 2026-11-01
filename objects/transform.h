#include "matrices.h"
#include "vectors.h"
#include "stdint.h"


/** 
 * Data structure holding the position, rotation and scale of an object as well as its rendering matrix.
 * During reads, the matrix is recalculated only if it's dirty.
 */
typedef struct {
    vec3f position, rotation, scale, shear;
    mat4f render_mtx;
    uint8_t is_dirty; // TODO: Move to per-scene bitset
} transform; // 121 bytes (120 after moving the flag)

/** Data structure holding the rendering matrix of a static object, calculated at initialization. */
typedef struct {
    mat4f render_mtx;
} static_transform; // 64 bytes

transform* transform_create(vec3f pos, vec3f rot, vec3f scale, vec3f shr) {
    
}

inline void transform_set_rotation(transform* tr, vec3f angles);
inline void transform_set_position(transform* tr, vec3f pos);
inline void transform_set_scale(transform* tr, vec3f scale);
inline void transform_set_shear(transform* tr, vec3f shear);
