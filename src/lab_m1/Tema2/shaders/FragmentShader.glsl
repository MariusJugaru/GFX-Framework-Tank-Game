#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_coordinate;
in vec3 frag_color;
in float noise_val;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    vec3 color2 = vec3(min(1, frag_color.x + 0.25f), min(1, frag_color.y + 0.25f), min(1, frag_color.z + 0.15f));
    out_color = vec4(mix(frag_color, color2, noise_val), 1);

}
