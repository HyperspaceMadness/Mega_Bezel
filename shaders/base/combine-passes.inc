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
    along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

#pragma name CombinePass

/////////////// IMPORTS ///////////////
#include "common/globals-and-image-layers-params.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

#define RENDER_SIMPLE_MODE_NONE 0
#define RENDER_SIMPLE_MODE_BLACK_SCREEN 1
#define RENDER_SIMPLE_MODE_BLACK_SCREEN_AND_BG 2
#define RENDER_SIMPLE_MODE_PINK_MASK 3
#define RENDER_SIMPLE_MODE_BLACK_WHITE_MASK 4

#define RENDER_SIMPLE_MASK_TYPE_SCREEN 0
#define RENDER_SIMPLE_MASK_TYPE_TUBE 1
#define RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME 2
#define RENDER_SIMPLE_MASK_TYPE_BEZEL 3
#define RENDER_SIMPLE_MASK_TYPE_FRAME 4

vec2 UNFLIPPED_VIEWPORT_COORD = vec2(0.5);
vec2 FLIPPED_VIEWPORT_COORD = vec2(0.5);


//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	// Not sure why we need linearize this but it seems to have a smoother range this way
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D CacheInfoPass;
layout(set = 0, binding = 2) uniform sampler2D Original;
layout(set = 0, binding = 3) uniform sampler2D FetchDrezPass;
layout(set = 0, binding = 4) uniform sampler2D DeditherPass;
layout(set = 0, binding = 5) uniform sampler2D ColorCorrectPass;
layout(set = 0, binding = 6) uniform sampler2D LinearizePass;
layout(set = 0, binding = 7) uniform sampler2D CrtPass;

#ifdef IS_NO_REFLECT_PRESET
#define MiddlePass CrtPass
#else
layout(set = 0, binding = 9) uniform sampler2D ReflectPass;
#define MiddlePass ReflectPass
#endif

layout(set = 0, binding = 10) uniform sampler2D SceneLayersPass;

layout(set = 0, binding = 11) uniform sampler2D CombinePassFeedback;
#define PassFeedback CombinePassFeedback


vec3 PrepGraphicsForHDR(vec3 in_color)
{
	vec3 out_color = in_color.rgb;
	
	out_color *= HSM_GLOBAL_GRAPHICS_BRIGHTNESS;
	
	#ifdef IS_HDR_PRESET
   if((HCRT_HDR < 1.0f) && (HCRT_COLOUR_ACCURATE < 1.0f))
      if(HCRT_OUTPUT_COLOUR_SPACE == 2.0f)
         out_color= (out_color * k709_to_XYZ) * kXYZ_to_DCIP3; 

   if(HCRT_HDR > 0.5f)
      out_color = InverseTonemap(out_color, HCRT_MAX_NITS, HCRT_PAPER_WHITE_NITS);

	if((HCRT_HDR >= 1.0f) && (HCRT_COLOUR_ACCURATE < 1.0f))
		out_color  *= k2020Gamuts[uint(HCRT_EXPAND_GAMUT)] * (HCRT_PAPER_WHITE_NITS / kMaxNitsFor2084);
	#endif

	return out_color;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	UNFLIPPED_VIEWPORT_COORD = vTexCoord;
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);

	if (HSM_SHOW_PASS_INDEX > 0.5)
	{
		FragColor = vec4(0);

		bool pass_needs_delinearize = true;
		vec2 coord_for_raw_passes = HSM_SHOW_PASS_APPLY_SCREEN_COORD > 0.5 ? HSM_GetCRTShaderCurvedCoord(SCREEN_COORD) : vTexCoord;

		// TODO Can we make this simpler without the #ifdef?
		#ifndef IS_NO_REFLECT_PRESET
		// REFLECTION
		if (HSM_SHOW_PASS_INDEX == 1)
		{
			FragColor = texture(ReflectPass, vTexCoord);
			pass_needs_delinearize = HSM_NON_USER_DELINEARIZE_REFLECT_PASS < 0.5;
		}
		#endif

		// TUBE
		if (HSM_SHOW_PASS_INDEX == 2)
			FragColor = texture(CrtPass, vTexCoord);

		// CRT
		if (HSM_SHOW_PASS_INDEX == 3)
			FragColor = texture(CrtPass, vTexCoord);

		// INTERLACE & LINEARIZE
		if (HSM_SHOW_PASS_INDEX == 4)
		{
			FragColor = texture(LinearizePass, (floor(coord_for_raw_passes * textureSize(LinearizePass, 0)) + 0.5) / textureSize(LinearizePass, 0));
			pass_needs_delinearize = HSM_NON_USER_LINEARIZE_INTERLACE_PASS > 0.5;
		}

		// COLOR CORRECT & UPSCALE
		if (HSM_SHOW_PASS_INDEX == 5)
		{
			FragColor = texture(ColorCorrectPass, (floor(coord_for_raw_passes * textureSize(ColorCorrectPass, 0)) + 0.5) / textureSize(ColorCorrectPass, 0));
			pass_needs_delinearize = false;
		}

		// DEDITHER
		if (HSM_SHOW_PASS_INDEX == 6)
		{
			FragColor = texture(DeditherPass, (floor(coord_for_raw_passes * textureSize(DeditherPass, 0)) + 0.5) / textureSize(DeditherPass, 0));
			pass_needs_delinearize = false;
		}

		// DREZ
		if (HSM_SHOW_PASS_INDEX == 7)
		{
			FragColor = texture(FetchDrezPass, coord_for_raw_passes);
			pass_needs_delinearize = false;
		}

		// CORE
		if (HSM_SHOW_PASS_INDEX == 8)
		{
			FragColor = texture(Original, coord_for_raw_passes);
			pass_needs_delinearize = false;
		}

		// IMAGE LAYERS
		if (HSM_SHOW_PASS_INDEX > 8)
		{
			vec4 image_layers_packed = texture(SceneLayersPass, vTexCoord);

			vec4 precomp_background = vec4(0);
			vec4 precomp_middleground = vec4(0);
			vec4 precomp_foreground = vec4(0);
			vec4 precomp_additive = vec4(0);

			HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
								precomp_foreground, 
								precomp_middleground, 
								precomp_background, 
								precomp_additive);

			if (HSM_SHOW_PASS_INDEX == 9)
				FragColor = precomp_additive;
			if (HSM_SHOW_PASS_INDEX == 10)
				FragColor = precomp_foreground;
			if (HSM_SHOW_PASS_INDEX == 11)
				FragColor = precomp_middleground;
			if (HSM_SHOW_PASS_INDEX == 12)
				FragColor = precomp_background;

			pass_needs_delinearize = false;
		}

		// Linearize passes which are not already in linear space
		if (pass_needs_delinearize)
			FragColor = HSM_Delinearize(FragColor, DEFAULT_SRGB_GAMMA);

		if (HSM_SHOW_PASS_ALPHA > 0.5)
			FragColor = vec4(FragColor.a);

		return;
	}

	// // Seems to make it slower
	// if (HSM_IsOutsideReflectionBoundary())
	// {
	// 	vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	// 	if (HSM_CACHE_GRAPHICS_ON > 0.5 && feedback_color_test.a < 0 && !CACHE_INFO_CHANGED && HSM_RENDER_SIMPLE_MODE < 0.5)
	// 	{
	// 		FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
	// 		return;
	// 	}
	// }

	HSM_GetBezelCoords(TUBE_DIFFUSE_COORD, 
						TUBE_DIFFUSE_SCALE, 
						TUBE_SCALE, 
						TUBE_DIFFUSE_ASPECT,
						HSM_RENDER_SIMPLE_MODE > 0.5,
						BEZEL_OUTSIDE_SCALE,
						BEZEL_OUTSIDE_COORD, 
						BEZEL_OUTSIDE_CURVED_COORD, 
						FRAME_OUTSIDE_CURVED_COORD);

	
	vec4 image_layers_packed = texture(SceneLayersPass, vTexCoord, 0);

	vec4 precomp_background = vec4(0);
	vec4 precomp_middleground = vec4(0);
	vec4 precomp_foreground = vec4(0);
	vec4 precomp_additive = vec4(0);

	HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
						precomp_foreground, 
						precomp_middleground, 
						precomp_background, 
						precomp_additive);

	precomp_foreground = HSM_Linearize(precomp_foreground, DEFAULT_SRGB_GAMMA);
	precomp_middleground = HSM_Linearize(precomp_middleground, DEFAULT_SRGB_GAMMA);
	precomp_background = HSM_Linearize(precomp_background, DEFAULT_SRGB_GAMMA);
	precomp_additive = HSM_Linearize(precomp_additive, DEFAULT_SRGB_GAMMA);

	float reflection_mask = precomp_background.a;
	precomp_background.rgb = PrepGraphicsForHDR(precomp_background.rgb);
	vec4 out_color = precomp_background;

	vec4 crt_layer = texture(MiddlePass, vTexCoord);

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
		crt_layer.rgb = vec3(0);

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	// TODO, need to see how to manage with parameters
	#ifndef IS_NO_REFLECT_PRESET
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);

	TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	// Apply Reflection Image Mask
	crt_layer *= TUBE_MASK + (1 - TUBE_MASK) * reflection_mask;
	#endif

	precomp_middleground.rgb = PrepGraphicsForHDR(precomp_middleground.rgb);
	out_color = HSM_PreMultAlphaBlend(out_color, precomp_middleground);

	if (HSM_RENDER_SIMPLE_MODE < 0.5)
		out_color = HSM_PreMultAlphaBlend(out_color, crt_layer);

	precomp_foreground.rgb = PrepGraphicsForHDR(precomp_foreground.rgb);
	out_color = HSM_PreMultAlphaBlend(out_color, precomp_foreground);

	precomp_additive.rgb = PrepGraphicsForHDR(precomp_additive.rgb);
	out_color = HSM_PreMultAlphaBlend(out_color, precomp_additive * vec4(1, 1, 1, 0));

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);

		TUBE_DIFFUSE_CURVED_COORD = HSM_GetCurvedCoord(TUBE_DIFFUSE_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, TUBE_DIFFUSE_ASPECT);
		TUBE_DIFFUSE_MASK = HSM_GetCornerMask(((TUBE_DIFFUSE_CURVED_COORD - 0.5) * 1.001) + 0.5, TUBE_DIFFUSE_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE, HSM_TUBE_BLACK_EDGE_SHARPNESS);

		TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);

		INSIDE_BEZEL_MASK = HSM_GetCornerMask(BEZEL_OUTSIDE_CURVED_COORD, TUBE_DIFFUSE_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_BZL_OUTER_CORNER_RADIUS_SCALE, 0.9);
		BEZEL_MASK = INSIDE_BEZEL_MASK * (1 - TUBE_MASK);

		OUTSIDE_BEZEL_MASK = 1 - INSIDE_BEZEL_MASK;
		OUTSIDE_FRAME_MASK = 1 - HSM_GetCornerMask(FRAME_OUTSIDE_CURVED_COORD, TUBE_DIFFUSE_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);

		FRAME_MASK = OUTSIDE_BEZEL_MASK * (1 - OUTSIDE_FRAME_MASK);

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_SCREEN )
			out_color = HSM_PreMultAlphaBlend(out_color, vec4(0, 0, 0, 1) * (TUBE_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_SCREEN_AND_BG )
			out_color = HSM_PreMultAlphaBlend(out_color, vec4(0, 0, 0, 1) * (TUBE_MASK + OUTSIDE_FRAME_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_MASK )
		{
			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_SCREEN )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_DIFFUSE_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_TUBE )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_MASK + OUTSIDE_FRAME_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (1 - BEZEL_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_FRAME )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (1 - FRAME_MASK));
		}
		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_WHITE_MASK )
		{
			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_SCREEN )
				out_color = vec4(1 - TUBE_DIFFUSE_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_TUBE )
				out_color = vec4(1 - TUBE_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME )
				out_color = vec4(1 - TUBE_MASK) - OUTSIDE_FRAME_MASK;

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL )
				out_color = vec4(BEZEL_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_FRAME )
				out_color = vec4(FRAME_MASK);
		}
	}

#ifdef IS_HDR_PRESET	
	vec3 output_colour = vec3(0);
	LinearToOutputColor(out_color.rgb, output_colour);
	out_color = vec4(output_colour, 1.0f);
#else
	out_color = HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
#endif

	// // If we have calculated an image then set -1 as a flag to show that we have
	// if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
	// 	FragColor.a = -1;

	FragColor = out_color;
}