#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_coordinate;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_coordinate;
out vec3 frag_color;
out float noise_val;

float random (in vec3 st) {
    return fract(sin(dot(st.xz,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise(in vec3 st) {
    float a = random(st);
    float b = random(st + vec3(1.0, 1.0, 0.0));
    float c = random(st + vec3(0.0, 0.66, 1.0));

    return mix(a, b, c);
}

void main()
{
    noise_val = noise(v_position);

    frag_position = vec3(v_position.x, noise_val, v_position.z);
    frag_normal = v_normal;
    frag_coordinate = v_coordinate;
    frag_color = v_color;


    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(v_position.x, noise_val, v_position.z, 1.0);

}
