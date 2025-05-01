R"(////////// SHADER BEGIN /////////
// Author: Alexander Balako
// Version: 04/30/2025

// Fragment shaders are meant to control the low of objects: glow, transparency, etc.

/**
* This shader makes the tiles appear to be facing the
* direction that the mouse or drag is moving towards.
*/ 

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D u_texture;

void main() {
    fragColor = texture(u_texture, v_texCoord);
}



/////////// SHADER END //////////)"