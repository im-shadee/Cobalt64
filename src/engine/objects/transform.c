#include "transform.h"
#include "trig.h"
#include "custom_math.h"
#include <assert.h>
#include <stdlib.h>

transform_t* transform_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, transform_hierarchy_t hierarchy) {
    transform_t* tr = malloc(sizeof(transform_t));
    if (!tr) return NULL;

    tr->is_dirty = FALSE;
    tr->position = pos;
    tr->rotation = rot;
    tr->scale = scale;
    tr->shear = shr;
    tr->hierarchy = hierarchy;

    update_render_mtx(&tr->render_mtx, pos, rot, scale, shr); // Calculate the render matrix once when initialized
    return tr;
}

void update_render_mtx(mtx4f_t* mtx, vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    assert(mtx); // Crash if the passed matrix is null

    // Convert float degrees to 16-bit BAMs
    uint16_t x_bam = (uint16_t)(rot.x * DEG_TO_BAM);
    uint16_t y_bam = (uint16_t)(rot.y * DEG_TO_BAM);
    uint16_t z_bam = (uint16_t)(rot.z * DEG_TO_BAM);

    // Fetch fixed-point values from LUT and scale them
    float cx = fixed_cos(x_bam) * FIXED_TO_FLOAT;
    float sx = fixed_sin(x_bam) * FIXED_TO_FLOAT;
    
    float cy = fixed_cos(y_bam) * FIXED_TO_FLOAT;
    float sy = fixed_sin(y_bam) * FIXED_TO_FLOAT;
    
    float cz = fixed_cos(z_bam) * FIXED_TO_FLOAT;
    float sz = fixed_sin(z_bam) * FIXED_TO_FLOAT;

    // Compute rotation matrix values (compute matrix indexes instead of creating matrices)
    float r00 = cy * cz;
    float r01 = -cy * sz;
    float r02 = sy;
    
    float r10 = sx * sy * cz + cx * sz;
    float r11 = -sx * sy * sz + cx * cz;
    float r12 = -sx * cy;
    
    float r20 = -cx * sy * cz + sx * sz;
    float r21 = cx * sy * sz + sx * cz;
    float r22 = cx * cy;

    // Compute scale and shearing combined
    float s00 = scale.x;
    float s01 = shr.x * scale.y;
    float s02 = shr.y * scale.z;

    float s11 = scale.y;
    float s12 = shr.z * scale.z;

    float s22 = scale.z;

    // Assign the result of the multiplication of rotation, shearing/scale and translation elements to the render matrix
    mtx->m[0][0] = s00 * r00 + s01 * r10 + s02 * r20;
    mtx->m[0][1] = s00 * r01 + s01 * r11 + s02 * r21;
    mtx->m[0][2] = s00 * r02 + s01 * r12 + s02 * r22;
    mtx->m[0][3] = pos.x;

    mtx->m[1][0] = s11 * r10 + s12 * r20;
    mtx->m[1][1] = s11 * r11 + s12 * r21;
    mtx->m[1][2] = s11 * r12 + s12 * r22;
    mtx->m[1][3] = pos.y;

    mtx->m[2][0] = s22 * r20;
    mtx->m[2][1] = s22 * r21;
    mtx->m[2][2] = s22 * r22;
    mtx->m[2][3] = pos.z;

    mtx->m[3][0] = 0.0f;
    mtx->m[3][1] = 0.0f;
    mtx->m[3][2] = 0.0f;
    mtx->m[3][3] = 1.0f;
}

baked_transform_t* baked_tr_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, transform_hierarchy_t hierarchy) {
    baked_transform_t* tr = malloc(sizeof(baked_transform_t));
    if (!tr) return NULL;

    tr->hierarchy = hierarchy;
    update_render_mtx(&tr->render_mtx, pos, rot, scale, shr); // Calculate the render matrix once when initialized
    return tr;
}
