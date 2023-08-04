#version 450

/*
    Mega Bezel - Creates a graphic treatment for the game play area to give a retro feel
    Copyright (C) 2019-2023 HyperspaceMadness - HyperspaceMadness@outlook.com

    Incorporates much great feedback from the libretro forum, and thanks 
    to Hunterk who helped me get started

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

// Float format so we can avoid banding
// #pragma format R16G16B16A16_SFLOAT

/////////////// IMPORTS ///////////////
#include "common/globals-and-simple-bg-params.inc"
#include "common/params-2-bezel.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

vec2 VIEWPORT_COORD = vec2(0.5);

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;
layout(location = 7) out vec2 UNFLIPPED_VIEWPORT_COORD;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	UNFLIPPED_VIEWPORT_COORD = vTexCoord * 1.0001;
	vTexCoord * 1.0001;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;
layout(location = 7) in vec2 UNFLIPPED_VIEWPORT_COORD;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D CacheInfoPass;

layout(set = 0, binding = 2) uniform sampler2D Source;

layout(set = 0, binding = 3) uniform sampler2D BackgroundImage;

layout(set = 0, binding = 4) uniform sampler2D CabinetGlassImage;
layout(set = 0, binding = 5) uniform sampler2D ReflectionMaskImage;

layout(set = 0, binding = 6) uniform sampler2D SimpleBGPassFeedback;
#define PassFeedback SimpleBGPassFeedback

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);

	if (HSM_IsOutsideReflectionBoundary())
	{
		vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
		if (HSM_CACHE_GRAPHICS_ON > 0.5 && feedback_color_test.a < 0 && !CACHE_INFO_CHANGED)
		{
			FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
			return;
		}
	}

	//----------------------------------------------------
	// CRT Pass
	//----------------------------------------------------
	// Get the CRT pass and make it in linear space & mask the area outside the screen
	vec4 frag_color_linear = HSM_Linearize(texture(Source, UNFLIPPED_VIEWPORT_COORD.xy), DEFAULT_SRGB_GAMMA);

	frag_color_linear = HSM_ApplySimpleBG(frag_color_linear, vTexCoord, BackgroundImage);

	// FragColor = frag_color_linear;

// Apply output gamma if this not an HDR preset because it is the last pass
// #ifndef IS_HDR_PRESET
	FragColor = HSM_Delinearize(frag_color_linear, DEFAULT_SRGB_GAMMA);
// #endif

	// If we have calculated an image then set -1 as a flag to show that we have
	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
		FragColor.a = -1;

	return;
}