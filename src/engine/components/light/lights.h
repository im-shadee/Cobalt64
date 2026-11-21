#ifndef LIGHTS_H
#define LIGHTS_H

#include "color.h"
#include "layers.h"
#include <stdbool.h>

/** Defines the emission behavior of a dynamic LightSource component. */
typedef enum __attribute__((packed)) {
    LIGHT_TYPE_POINT = 0, // Emits light outward from the component's position with distance falloff
    LIGHT_TYPE_DIRECTIONAL = 1 // Infinitely distant light casting parallel rays along the component's forward vector
} light_source_type_t;

/**
 * Unique, scene-wide non-directional base light applied uniformly to all surfaces.
 *
 * Represents indirect environmental bounce light (sky/ground fill) so shadowed faces are not pitch black. 
 * Does not consume any of Tiny3D's 7 active light slots.
 */
typedef Color8 AmbientLight; // 8 bytes

/**
 * Dynamic light source component attached to scene entities (up to 7 active per draw call).

 * Position and direction are derived from the owning entity's Transform component.
 */
typedef struct
{
    float size;
    Color8 light_color;
    light_source_type_t type;
    light_layer_t light_layer_mask;
    bool enabled;
    bool ignoreNormals;
} LightSource; // 12 bytes

#endif // LIGHTS_H
