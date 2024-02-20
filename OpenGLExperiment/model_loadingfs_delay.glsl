#version 450 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
//layout (location = 2) out vec4 gColorSpec;
layout (location = 2) out vec3 gDiffuse;
layout (location = 3) out vec3 gSpecular;

//out vec4 FragColor;

in vec3 base_normal;
in vec3 world_position;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0) ;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


void main()
{    
    vec3 light_dir = normalize(lightPos - world_position);
    vec3 world_normal = base_normal;

    vec3 tex_diffuse = texture(texture_diffuse1, TexCoords.xy).rgb;
    vec3 tex_specular = texture(texture_specular1, TexCoords.xy).rgb;

    gPosition = world_position.xyz;
    gNormal.rgb = normalize(world_normal).xyz;
    gDiffuse.rgb = tex_diffuse.xyz;
    gSpecular.rgb = tex_specular.xyz;
//    FragColor = vec4(gDiffuse.rgb, 1.0f);
}