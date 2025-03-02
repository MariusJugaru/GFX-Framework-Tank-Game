#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform float time;
uniform int texture_id;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    
    vec4 color1;

    if (texture_id == 9) {
        color1 = texture2D(texture_1, vec2(texcoord.x + time, texcoord.y));
    } else {
        color1 = texture2D(texture_1, texcoord);
    }
    
    

    
    if (color1.a < 0.5f) {
        discard;
    }

    vec4 color2;
    
    if (texture_id == 9) {
        color2 = texture2D(texture_2, vec2(texcoord.x + time, texcoord.y));
    } else {
        color2 = texture2D(texture_2, texcoord);
    }
    vec4 color = mix(color1, color2, 0.5f);

    out_color = color;

}
