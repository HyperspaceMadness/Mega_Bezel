#version 450

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float blendMode;
	float nsharpen;
} params;

#pragma parameter nsharpen  "NTSC Sharpening" 0.0 0.0 0.33 0.01
#pragma parameter blendMode "NTSC Blend Mode (Main Mode Control)" 0.0 0.0 2.0 1.0

layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
} global;

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
   gl_Position = global.MVP * Position;
   vTexCoord = TexCoord;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D PrePass;

vec3 plant (vec3 tar, float r)
{
	float t = max(max(tar.r,tar.g),tar.b) + 0.00001;
	return tar * r / t;
}

vec3 declip(vec3 c, float b)
{
	float m = max(max(c.r,c.g),c.b);
	if (m > b) c = c*b/m;
	return c;
}


vec3 stexture (vec2 TexCoord, vec2 dx) {
	vec3 a = texture(Source, TexCoord -dx).rgb;
	vec3 b = texture(Source, TexCoord    ).rgb;
	vec3 c = texture(Source, TexCoord +dx).rgb;
	vec3 r = (1.0 + 2.0*params.nsharpen)*b -  params.nsharpen*(a + c);
	return min( clamp (r, 0.8*min(min(a,b),c), max(max(a,b),c)), 1.0);
}


void main()
{
	vec2 dx = vec2(params.SourceSize.z * 0.5, 0.0);
	vec3 col1 = stexture(vTexCoord -dx, 2.0*dx).rgb;
	vec3 col2 = stexture(vTexCoord +dx, 2.0*dx).rgb;
	vec3 colc = max(col1, col2);
	vec3 col = plant(sqrt(col1*col2), max(max(colc.r, colc.g),colc.b));
	
	vec3 orig = texture(PrePass, vTexCoord).rgb;
	vec3 res = normalize(col + 0.00001) * min(length(col), length(orig));

	float k2 = 1.0/(dot(col1 - res, col1 - res) + 0.0001);
	float k3 = 1.0/(dot(col2 - res, col2 - res) + 0.0001);
	
	vec3 res1 = (k2 * col1 + k3 * col2) / (k2 + k3);
	res1 = clamp(res1, min(col1,col2), max(col1, col2)); 

	if ( params.blendMode == 1.0) res = res1; else if ( params.blendMode == 2.0) res = stexture(vTexCoord, 2.0*dx);

	FragColor = vec4(res, 1.0);
}