#version 450 core
out vec4 FragColor;

in vec3 base_normal;

in vec3 world_position;
in mat3 TBN;

in vec2 TexCoords;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0) ;
uniform samplerCube skybox;

//diffuse factor
uniform float diffuse_factor = 1f;

//specular factor
uniform float specular_glass = 30f;
uniform float specular_factor = 1.0f;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;


void main()
{    
    vec3 view_dir = normalize(cameraPos - world_position);
    vec3 light_dir = normalize(lightPos - world_position);
    vec3 half_dir = normalize(view_dir + light_dir);
    
    vec3 base_color = vec3(0.0, 0.0, 0.0);

    //Get Normal
//    vec3 normal_tangent = texture(texture_normal1, TexCoords.xy).rgb;
//    normal_tangent = normal_tangent * 2.0 - 1.0;
//    vec3 world_normal = transpose(TBN) * normal_tangent;
    vec3 world_normal = base_normal;
    

    vec3 tex_diffuse = texture(texture_diffuse1, TexCoords.xy).rgb;

    vec3 diffuse  = lightColor.xyz * tex_diffuse.xyz * (max(dot(normalize(world_normal), light_dir), 0) * 0.5f + 0.5) * diffuse_factor;

    vec3 tex_specular = texture(texture_specular1, TexCoords.xy).rgb;
    vec3 specular = lightColor.xyz * tex_specular.xyz * pow(max(dot(normalize(world_normal), half_dir), 0), specular_glass) * specular_factor;
    
    FragColor = vec4((diffuse.xyz + specular.xyz), 1.0f);
//    FragColor = vec4(world_normal.xyz, 1.0f);
}