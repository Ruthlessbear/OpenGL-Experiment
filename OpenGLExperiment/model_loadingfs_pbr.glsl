#version 450 core
out vec4 FragColor;

in vec3 base_normal;

in vec3 world_position;
in mat3 TBN;

in vec2 TexCoords;
in vec4 DepthPosTexCoords;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightPos2;
//平行光
uniform vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 lightColor2 = vec4(0.8f, 0.8f, 0.8f, 1.0f);

uniform float lightStrength = 1.5f;
uniform samplerCube skybox;

//diffuse factor
uniform float diffuse_factor = 1f;

//specular factor
uniform float specular_glass = 30f;
uniform float specular_factor = 1.0f;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_depth;

//PBR Factor
uniform vec3 albedo = vec3(0.3f, 0.3f, 0.3f);
uniform float metallic = 0.9f;
uniform float roughness = 0.3f;
uniform float ao = 0.1f;

const float PI = 3.14159265359;

// 菲涅尔

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0f, 1.0f), 5.0f);
}

// 正态分布/法线分布

float DistributionGGX(vec3 normal, vec3 h, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, h), 0.0f);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

// 几何分布

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

void main()
{    
    vec3 view_dir = normalize(cameraPos - world_position);
    
    vec3 world_normal = normalize(base_normal);

    vec3 tex_diffuse = texture(texture_diffuse1, TexCoords.xy).rgb;
    vec3 tex_specular = texture(texture_specular1, TexCoords.xy).rgb;
    float tex_shadow = texture(texture_depth, DepthPosTexCoords.xy).r;

    vec3 L0 = vec3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i <= 2; i++)
    {
        vec3 light_dir = vec3(0.0f, 0.0f, 0.0f);
        vec3 light_color = vec3(0.0f, 0.0f, 0.0f);
        if(i == 0)
        {
            light_dir = normalize(lightPos - world_position);
            light_color = lightColor.xyz;
        }
        else if(i == 1)
        {
            light_dir = normalize(lightPos2 - world_position);
            light_color = lightColor2.xyz;
        }

        vec3 half_dir = normalize(view_dir + light_dir);

        // DFG/4(wo·n)(wi·n) wo指视角方向
        vec3 f0 = vec3(0.04f);
        f0 = mix(f0, tex_diffuse, metallic);
        vec3 Fresnel = fresnelSchlick(max(dot(world_normal, view_dir), 0.0f), f0);
    
        float NDF = DistributionGGX(world_normal, half_dir, roughness);

        float G = GeometrySmith(world_normal, view_dir, light_dir, roughness);

        float denominator = 4.0f * max(dot(world_normal, view_dir), 0.0f) * max(dot(world_normal, light_dir), 0.0f) + 0.001;
        vec3 specular = (NDF * G * Fresnel) / denominator;

        //compute ks/kd
        vec3 Ks = Fresnel;
        vec3 Kd = vec3(1.0f) - Ks;
        //金属不会折射光线  故材质为金属时kd为0
        Kd *= 1.0 - metallic;

        //radiance
        //  光照衰减
        //  float light_distance = length(lightPos - world_position);
        //  float attenuation = 1.0f / (light_distance * light_distance) * lightStrength;
        float attenuation = 1.0f * lightStrength;


        vec3 radiance = light_color.xyz * attenuation;
        float NdotL = max(dot(world_normal, light_dir), 0.0f);

        vec3 BRDF = Kd * tex_diffuse / PI + Ks * specular * tex_specular;
        L0 += BRDF * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 LDR_Color = ambient + L0;

    //Translate to HDR
    vec3 HDR_Color = LDR_Color / (LDR_Color + vec3(1.0f));
    HDR_Color = pow(HDR_Color, vec3(1.0f/ 2.2f));

    FragColor = vec4(HDR_Color.xyz, 1.0f);
}



