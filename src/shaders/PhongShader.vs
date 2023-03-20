#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 PosLightSpace;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// 在世界空间中计算光照的情况 观察空间中则要乘观察矩阵
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal; // 进行不等比缩放时要乘法线矩阵(一般定义在CPU中再传过来)
	// Normal = aNormal;
	TexCoords = aTexCoords;
	PosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);// 把世界空间顶点位置转换为光空间
}