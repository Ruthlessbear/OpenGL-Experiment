#version 450 core
out vec4 FragColor;

in vec3 w_normal;
in vec3 w_position;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0) ;
uniform float diffuse_factor = 1f;

void main()
{    
    vec3 view_dir = normalize(cameraPos - w_position);
    vec3 light_dir = normalize(lightPos - w_position);
    vec3 half_dir = normalize(view_dir + light_dir);

//    vec3 diffuse  = lightColor.xyz * (max(dot(normalize(w_normal), light_dir), 0) * 0.5f + 0.5) * diffuse_factor;
//    float fresnel = 1 - max(dot(view_dir, light_dir), 0);

    FragColor = vec4(1.0f);
}