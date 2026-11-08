#include "transform.h"
#include "custom_math.h"
#include <assert.h>

transform_t* transform_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    transform_t* tr = malloc(sizeof(transform_t)); // TODO: Change to libdragon mem allocation
    if (!tr) return NULL;

    tr->render_mtx = IDENTITY;
    tr->is_dirty = FALSE;
    tr->position = pos;
    tr->rotation = rot;
    tr->scale = scale;
    tr->shear = shr;

    return tr;
}

void update_render_mtx(mtx4f_t* mtx, vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    assert(mtx); // Crash if the passed matrix is null

    // Retrieve LUT indexes from rotation angles
    int x_idx = angle_to_lut_idx(rot.x);
    int y_idx = angle_to_lut_idx(rot.y);
    int z_idx = angle_to_lut_idx(rot.z);

    // Compute angles from sine/cosine LUTs
    float cx = cos_lut[x_idx], sx = sin_lut[x_idx]; // X axis
    float cy = cos_lut[y_idx], sy = sin_lut[y_idx]; // Y axis
    float cz = cos_lut[z_idx], sz = sin_lut[z_idx]; // Z axis

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

baked_transform_t* baked_tr_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    baked_transform_t* tr = malloc(sizeof(baked_transform_t)); // TODO: Change to libdragon mem allocation
    if (!tr) return NULL;

    update_render_mtx(&tr->render_mtx, pos, rot, scale, shr); // Calculate the render matrix once when initialized
    return tr;
}
