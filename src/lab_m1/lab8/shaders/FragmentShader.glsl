#version 330

// Input
in vec3 world_position;
in vec3 world_normal;


// Uniforms for light properties
uniform vec3 light_direction[2];
uniform vec3 light_position[2];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms
uniform int lightMode;

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;



float point_light_contribution(vec3 light_position, vec3 light_direction) {
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float diffuse_light = 0;

    float specular_light = 0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(world_normal, H), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    float cut_off = radians(30.0f);
    float spot_light = dot(-L, light_direction);
    float light;

    if (lightMode == 0) {
        if (spot_light > cos(cut_off)) {
            diffuse_light = material_kd * max(dot(world_normal, L), 0);

            float spot_light_limit = cos(cut_off);
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);

             light = (diffuse_light + specular_light) * light_att_factor;
        } else {
            float d = distance(light_position, world_position);
            float attenuation = 1 / d * d;
            light = (diffuse_light + specular_light) * attenuation;
        }
    } else {
        diffuse_light = material_kd * max(dot(world_normal, L), 0);

        float d = distance(light_position, world_position);
        float attenuation = 1 / d * d;
        light = (diffuse_light + specular_light) * attenuation;
    }

    return light;
}

void main()
{

    vec3 L = normalize(light_position[0] - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    // TODO(student): Define ambient, diffuse and specular light components
    float ambient_light = 0.25;

    float diffuse_light = 0;

    float specular_light = 0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(world_normal, H), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    float cut_off = radians(30.0f);
    float spot_light = dot(-L, light_direction[0]);
    float light;

    if (lightMode == 0) {
        if (spot_light > cos(cut_off)) {
            diffuse_light = material_kd * max(dot(world_normal, L), 0);

            float spot_light_limit = cos(cut_off);
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);

             light = ambient_light + (diffuse_light + specular_light) * light_att_factor;
        } else {
            float d = distance(light_position[0], world_position);
            float attenuation = 1 / d * d;
            light = ambient_light + (diffuse_light + specular_light) * attenuation;
        }
    } else {
        diffuse_light = material_kd * max(dot(world_normal, L), 0);

        float d = distance(light_position[0], world_position);
        float attenuation = 1 / d * d;
        light = ambient_light + (diffuse_light + specular_light) * attenuation;
    }


    


    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.

    


    // TODO(student): Write pixel out color
    float color = ambient_light + point_light_contribution(light_position[0], light_direction[0]) + point_light_contribution(light_position[1], light_direction[1]);
    out_color = vec4(color * object_color, 1);

}
