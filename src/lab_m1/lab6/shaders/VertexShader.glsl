#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec3 v_coordinate;
layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_coordinate;
out vec3 frag_color;
out vec3 new_position;

void main()
{
    // TODO(student): Send output to fragment shader
    frag_position = v_position;
    frag_normal = v_normal;
    frag_coordinate = v_coordinate;
    frag_color = v_color;

    
    frag_color = vec3(v_color[0] * abs(sin(time)), v_color[1] * 2 * abs(cos(time)), v_color[2] * 3 * abs(sin(time)));
    new_position = vec3(v_position[0] * cos(time), v_position[1] * cos(time), v_position[2] * sin(time));

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(new_position, 1.0);

}
