#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 PosLightSpace;

struct Material{
    sampler2D texture_diffuse1; // 漫反射和环境光采用同一个贴图
    sampler2D texture_specular1;
    // sampler2D emission; 自发光项
    sampler2D shadow_direct;
    samplerCube shadow_point;

    float shininess;
};

struct DirLight{
    bool enable;

    vec3 direction; // 平行光源

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    bool enable;
    vec3 position; // 点光源
    // 距离衰减
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    bool enable;
    vec3 position;
    vec3 direction;

    float cutOff; //聚光
    float outerCutOff; //外围
    // 距离衰减
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};
  
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform int isTexture;//是否加载纹理 否则显示一种颜色selfColor
uniform vec3 selfColor;

uniform float far_plane;

uniform int shadowType;

// uniform float matrixLight;
// uniform float matrixMove;

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float PointShadowCalculation(vec3 lightPos);
float PointShadowCalculationWithPCF(vec3 lightPos);
float BlockerSearch(vec3 fragToLight);
float PercentageCloserSoftShadowCalculation(vec3 lightPos);

//Depth Test
float near = 0.1; 
float far  = 100.0; 
// 偏移量方向数组
vec3 offsetDirections[20] = vec3[]
(
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    // 平行光
    if(dirLight.enable)
        result += CalDirLight(dirLight, Normal, viewDir);
    // 点光源
    for(int i=0; i < NR_POINT_LIGHTS; i++)
        if(pointLights[i].enable)
            result += CalPointLight(pointLights[i], Normal, FragPos, viewDir);
    // 聚光
    if(spotLight.enable)
        result += CalSpotLight(spotLight, Normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

    // //DepthTest
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    // FragColor = vec4(vec3(depth), 1.0);

}

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.texture_specular1, TexCoords));
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if(isTexture == 1)
    {
        ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.texture_diffuse1, TexCoords));
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * vec3(texture(material.texture_specular1, TexCoords));
    }
    else{
        ambient = light.ambient * selfColor;
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * selfColor;
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * selfColor;
    }

    float shadow = ShadowCalculation(PosLightSpace, norm, lightDir);

    // return (ambient + (diffuse + specular));
    return (ambient + (1.0f-shadow) * (diffuse + specular));
}


vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if(isTexture == 1)
    {
        ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.texture_diffuse1, TexCoords));
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * vec3(texture(material.texture_specular1, TexCoords));
    }
    else{
        ambient = light.ambient * selfColor;
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * selfColor;
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * selfColor;
    }

    //距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = 0.0;

    if(shadowType == 0)
        shadow = PointShadowCalculation(light.position);
    else if (shadowType == 1)
        shadow = PointShadowCalculationWithPCF(light.position);
    else if (shadowType == 2)
        shadow = PercentageCloserSoftShadowCalculation(light.position);
    
    return (ambient + (1.0f-shadow) * (diffuse + specular));
}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if(isTexture == 1)
    {
        ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.texture_diffuse1, TexCoords));
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * vec3(texture(material.texture_specular1, TexCoords));
    }
    else{
        ambient = light.ambient * selfColor;
        diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * selfColor;
        specular = light.specular * pow(max(dot(norm, halfwayDir), 0.0), material.shininess) * selfColor;
    }

    // 聚光 平滑过渡
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    
    diffuse *= intensity;
    specular *= intensity;

    //距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    // float closestDepth = texture(material.shadow, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    // 检查当前片段是否在阴影中
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(material.shadow_direct, 0);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //PCF
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++){ 
            float pcfDepth = texture(material.shadow_direct, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= 9.0;

    //当一个点比光的远平面还要远时，它的投影坐标的z坐标大于1.0 (解决远处为阴影 配合GL_CLAMP_TO_BORDER环绕方式)
    if(currentDepth > 1.0)
        shadow = 0.0;

    return shadow;
}

float PointShadowCalculation(vec3 lightPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = FragPos - lightPos;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(material.shadow_point, fragToLight).r;
    // It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

// float PointShadowCalculationWithPCF(vec3 lightPos){
//     vec3 fragToLight = FragPos - lightPos;
//     float shadow = 0.0;
//     float bias = 0.05; 
//     float samples = 4.0;
//     float offset = 0.1;
//     float currentDepth = length(fragToLight);
//     for(float x = -offset; x < offset; x += offset / (samples * 0.5))
//     {
//         for(float y = -offset; y < offset; y += offset / (samples * 0.5))
//         {
//             for(float z = -offset; z < offset; z += offset / (samples * 0.5))
//             {
//                 float closestDepth = texture(material.shadow_point, fragToLight + vec3(x, y, z)).r; 
//                 closestDepth *= far_plane;   // Undo mapping [0;1]
//                 if(currentDepth - bias > closestDepth)
//                     shadow += 1.0;
//             }
//         }
//     }
//     shadow /= (samples * samples * samples);

//     return shadow;
// }
//改进后 使用偏移量方向数组 根据观察者的距离来增加偏移半径
float PointShadowCalculationWithPCF(vec3 lightPos){
    vec3 fragToLight = FragPos - lightPos;
    float shadow = 0.0;
    float bias = 0.05; 
    int samples = 20;
    // float offset = 0.05;
    float offset = (1.0 + (length(viewPos - FragPos) / far_plane)) / 25.0;
    float currentDepth = length(fragToLight);
    for(int i=0; i < samples ; i++){
        float closestDepth = texture(material.shadow_point, fragToLight + offsetDirections[i] * offset).r; 
        closestDepth *= far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

float BlockerSearch(vec3 fragToLight){
    float dBlocker = 0.0;
    int blocker_num = 0;
    float bias =0.05;
    int samples = 20;
    float offset = 0.05;
    float currentDepth = length(fragToLight);
    for(int i=0; i < samples ; i++){
        float blockerDepth = texture(material.shadow_point, fragToLight + offsetDirections[i]*offset).r * far_plane;
        if(currentDepth - bias > blockerDepth){
            blocker_num++;
            dBlocker += blockerDepth;
        }
    }
    dBlocker /= blocker_num;
    return dBlocker;
}


float PercentageCloserSoftShadowCalculation(vec3 lightPos)
{
    //Block search
    vec3 fragToLight = FragPos - lightPos;
    float w_light = 1;//光源大小
    float currentDepth = length(fragToLight);
    float dBlocker = BlockerSearch(fragToLight);

    // if(dBlocker < 0.01)
    //     return 0.0;
    
    //filter size
    float filtersize = (currentDepth - dBlocker) * w_light / dBlocker;

    //PCF
    float shadow=0.0;
    int samples = 20;
    float bias =0.05;
    float offset = filtersize/2;

    for(int i=0; i < samples ; i++){
        float closestDepth = texture(material.shadow_point, fragToLight + offsetDirections[i]*offset).r; 
        closestDepth *= far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);
    return shadow;
}
