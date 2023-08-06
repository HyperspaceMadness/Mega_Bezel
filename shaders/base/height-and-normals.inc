/*
    Mega Bezel - Creates a graphic treatment for the game play area to give a retro feel
    Copyright (C) 2019-2023 HyperspaceMadness - HyperspaceMadness@outlook.com

    See more at the libretro forum
    https://forums.libretro.com/t/hsm-mega-bezel-reflection-shader-feedback-and-updates

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/////////////// IMPORTS ///////////////
#include "common/globals-and-image-layers-params.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

#pragma name HeightAndNormalsPass

// Float format so we can avoid banding
#pragma format R32G32B32A32_SFLOAT

// // Generating height & normals from a shadertoy by granito
// const vec3 GrayscaleWeights = vec3(0.2125,0.7154,0.0721);

// // Sample map to generate Normal map and Height map
// vec4 GenerateNormalHeight (sampler2D tex, vec2 uv, vec2 res, float width)
// {
//     vec2 texelSize = 1. / (res * width);
//     vec4 h;
//     h[0] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 0,-1)) ).rgb);
//     h[1] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2(-1, 0)) ).rgb);
//     h[2] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 1, 0)) ).rgb);
//     h[3] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 0, 1)) ).rgb);
//     vec3 n;
//     n.y = h[0] - h[3];
//     n.x = h[1] - h[2];
//     n.z = .25;
//     float height = dot(GrayscaleWeights, texture(tex, uv).rgb);
//     // return vec4(normalize(n), height);

// 	return vec4(height);
// }

// // Multipass version of above
// vec4 GenerateNormalHeightMultipass (sampler2D tex, vec2 uv, vec2 res)
// {
//     float dist = 0.5;
//     vec4 multi = vec4(0.);
// 	multi += GenerateNormalHeight(tex, uv, res, 1. * dist);
// 	multi += GenerateNormalHeight(tex, uv, res, 2.5 * dist);
// 	multi += GenerateNormalHeight(tex, uv, res, 5. * dist);
// 	multi += GenerateNormalHeight(tex, uv, res, 10. * dist);    
//     multi *= 0.25;

// 	// multi = GenerateNormalHeight(tex, uv, res, 1. * dist);
//     return multi;
// }


#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 vTexCoord;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.00001;
}

#pragma stage fragment

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1)	uniform sampler2D CacheInfoPass;

//  Layers
layout(set = 0, binding = 2)	uniform sampler2D Background;
// layout(set = 0, binding = 3)	uniform sampler2D BgHeight;
// layout(set = 0, binding = 4)	uniform sampler2D BgNormals;

// layout(set = 0, binding = 5)	uniform sampler2D Device;
// layout(set = 0, binding = 7)	uniform sampler2D DeviceHeight;
// layout(set = 0, binding = 6)	uniform sampler2D DeviceNormals;

// layout(set = 0, binding = 8)	uniform sampler2D ReflectionMask;


//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);
	vec2 VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);

	// HSM_GetBezelCoords(TUBE_DIFFUSE_COORD, 
	// 					TUBE_DIFFUSE_SCALE, 
	// 					TUBE_SCALE, 
	// 					TUBE_DIFFUSE_ASPECT,
	// 					HSM_RENDER_SIMPLE_MODE > 0.5,
	// 					BEZEL_OUTSIDE_SCALE,
	// 					BEZEL_OUTSIDE_COORD, 
	// 					BEZEL_OUTSIDE_CURVED_COORD, 
	// 					FRAME_OUTSIDE_CURVED_COORD);
	
	// vec4 image_layers_packed = texture(SceneLayersPass, vTexCoord, 0);

	// int bg_channel_index = 0;
	// 			// vec4 GenerateNormalHeightMultipass (sampler2D tex, vec2 uv, int channel_index, vec2 res)

	// vec4 out_color = GenerateNormalHeightMultipass(Background, vTexCoord, vec2(1920, 1080));
	// out_color = vec4(out_color.a);
	// out_color = texture(Background, vTexCoord);
	// FragColor = out_color;
}