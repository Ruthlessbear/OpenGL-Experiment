#version 450 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];
uniform mat4 projection;

const vec2 noiseScale = vec2(800.0f / 4.0f, 600.0f / 4.0f);
float radius = 0.5;
float bias = 0.010f;

void main()
{    
    vec3 fragPos = texture(gPositionDepth, TexCoords.xy).xyz;
    vec3 normal = texture(gNormal, TexCoords.xy).rgb;
    vec3 randomVec = texture(noiseTexture, TexCoords * noiseScale).rgb;
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    int kernelSize = 64;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 random_sample = tbn * samples[i];
        random_sample = fragPos.xyz + random_sample * radius;

        //转到ndc空间[-1,1] 再到屏幕空间[0, 1]
        vec4 offset = vec4(random_sample, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;

        //这里取z是为了获取相机空间下距离视角位置的距离值
        float sampleDepth = texture(gPositionDepth, offset.xy).z;

        //随机采样的点位与相机距离 和 当前与相机距离值比较 计算自遮蔽深浅程度 有点像高度图
        //用于消除边缘
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));
        //bias是一个边缘系数 就和溶解效果里面加个clip边缘一样
        //采样贴图的距离值大于随机点位距离值时，会有自遮蔽
        occlusion += (sampleDepth >= random_sample.z + bias ? 1.0f : 0.0) * rangeCheck;
    }

    //求平均值 输出遮蔽因子
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}