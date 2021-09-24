#version 450

#include "hsm-globals-and-screen-scale-params.inc"

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
} params;

#include "../../../../../include/compat_macros.inc"

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 blur_dxdy;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	const vec2 dxdy_scale = global.SourceSize.xy * global.OutputSize.zw;
	blur_dxdy = dxdy_scale * global.SourceSize.zw;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 blur_dxdy;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D MBZ_PostCRTPass;

#include "../../../../../include/blur-functions.h"

void main()
{
   FragColor = HSM_Linearize(vec4(tex2Dblur9x9(MBZ_PostCRTPass, vTexCoord, blur_dxdy), 1), HSM_DEFAULT_CRT_GAMMA);
}