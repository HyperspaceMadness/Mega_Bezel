#version 450

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float dsize;
} params;

#pragma parameter dsize "Scanline dithering size" 1.0 0.0 2.0 0.10
#define dsize         params.dsize     // Dithering size

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

void main()
{

   vec2 tex = vTexCoord;
   vec2 pos = tex * params.OutputSize.xy;
   vec2 dy  = dsize*vec2(0.0, params.OutputSize.w);
   float mixer = floor(mod(pos.x,2.0));
   tex = mix(tex + 0.0*dy, tex - dy, mixer);
   
   FragColor = vec4(texture(Source, tex).rgb, 1.0);
}