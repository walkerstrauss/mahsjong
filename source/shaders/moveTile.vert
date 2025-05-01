R"(////////// SHADER BEGIN /////////
// Author: Alexander Balako
// Version: 04/30/2025

// Vertex Shader is meant for move / deforming geometry

/**
* This shader makes the tiles appear to be facing the
* direction that the mouse or drag is moving towards.
*/ 

in vec2 aPosition;     // Vertex position
in vec2 aTexCoord;     // Texture coordinate

uniform mat4 uModel;
uniform mat4 uPerspective;     // Model-View-Projection matrix
uniform vec2 u_drag;    // Drag direction (normalized or clamped)

out vec2 v_texCoord;

void main() {
    // Skew strength factor
    float skewAmount = 0.1;

    // Calculate skewed position
    vec2 skewedPosition = aPosition;

    // Apply horizontal and vertical skew based on drag direction
    skewedPosition.x += u_drag.x * aPosition.y * skewAmount;
    skewedPosition.y += u_drag.y * aPosition.x * skewAmount;

    gl_Position = uPerspective * uModel * vec4(skewedPosition, 0.0, 1.0);
    v_texCoord = aTexCoord;
}


/////////// SHADER END //////////)"