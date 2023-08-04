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

#pragma name FinalCompositePass

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
layout(set = 0, binding = 8) uniform sampler2D NightLightingImage;
layout(set = 0, binding = 9) uniform sampler2D NightLighting2Image;

#ifdef IS_NO_REFLECT_PRESET
#define MiddlePass CrtPass
#else
layout(set = 0, binding = 10) uniform sampler2D ReflectPass;
#define MiddlePass ReflectPass
#endif

layout(set = 0, binding = 11) uniform sampler2D SceneLayersPass;
layout(set = 0, binding = 12) uniform sampler2D BounceLightPass;
layout(set = 0, binding = 13) uniform sampler2D ReflectPrepFullscreenGlowPass;

layout(set = 0, binding = 14) uniform sampler2D FinalCompositePassFeedback;
#define PassFeedback FinalCompositePassFeedback

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
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);
	vec2 VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);

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
	// 		FragColor = texture(PassFeedback, vTexCoord);
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
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_foreground.a = pow(precomp_foreground.a, DEFAULT_SRGB_GAMMA);

	precomp_middleground = HSM_Linearize(precomp_middleground, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_middleground.a = pow(precomp_middleground.a, DEFAULT_SRGB_GAMMA);
	
	precomp_background = HSM_Linearize(precomp_background, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_background.a = pow(precomp_background.a, DEFAULT_SRGB_GAMMA);

	float static_shadows = precomp_background.a;
	precomp_background.a = 1.0;

	precomp_additive = HSM_Linearize(precomp_additive, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_additive.a = pow(precomp_additive.a, DEFAULT_SRGB_GAMMA);
	float reflection_mask = precomp_additive.a;

	// Gray Material Bounce Light Display Mode
	if (HSM_LIGHT_BOUNCE_DISPLAY_MODE == 1)
	{
		vec3 material_color = vec3(0.2);
		precomp_foreground.rgb = material_color * precomp_foreground.a;
		precomp_middleground.rgb = material_color * precomp_middleground.a;
		precomp_background.rgb = material_color;
		precomp_additive.rgb = vec3(0.0);
	}

	// Adjust Albedo Brightness of all non-additive layers
	precomp_foreground.rgb *= HSM_LIGHT_BOUNCE_ALBEDO_BRIGHTNESS;
	precomp_middleground.rgb *= HSM_LIGHT_BOUNCE_ALBEDO_BRIGHTNESS;
	precomp_background.rgb *= HSM_LIGHT_BOUNCE_ALBEDO_BRIGHTNESS;\

	vec4 out_color = vec4(0);

	vec4 image_layers_packed_shadow_stretch = texture(SceneLayersPass, (vTexCoord - 0.5) / 1.1 + 0.5 + vec2(SCREEN_SCALE.x * 0.02, 0));
	vec4 precomp_background_shadow_stretch = vec4(0);
	vec4 precomp_middleground_shadow_stretch = vec4(0);
	vec4 precomp_foreground_shadow_stretch = vec4(0);
	vec4 precomp_additive_shadow_stretch = vec4(0);

	HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed_shadow_stretch, 
								precomp_foreground_shadow_stretch, 
								precomp_middleground_shadow_stretch, 
								precomp_background_shadow_stretch, 
								precomp_additive_shadow_stretch);

	// Needs to divide by 2
	float mix_horz = HSM_LIGHT_BOUNCE_MIX_HORZ / 2;
	float mix_vert = HSM_LIGHT_BOUNCE_MIX_VERT / 2;

	vec2 bounce_coord = vTexCoord;
	// vec2 bounce_coord = HSM_Anamorphic_Distort(bounce_coord, HSM_LIGHT_BOUNCE_ANAMORPH);
	vec2 bounce_coord_ctr = (bounce_coord - 0.5) * SCREEN_SCALE / HSM_LIGHT_BOUNCE_SCALE;

	vec4 bounce_light = texture(BounceLightPass, bounce_coord_ctr + 0.5);
	vec4 bounce_light_center = texture(ReflectPrepFullscreenGlowPass, bounce_coord_ctr * 0.5 + 0.5);
	vec4 bounce_light_mirrored_x = texture(BounceLightPass, vec2(-1 * bounce_coord_ctr.x, bounce_coord_ctr.y) + 0.5);
	vec4 bounce_light_mirrored_y = texture(BounceLightPass, vec2(bounce_coord_ctr.x, -1 * bounce_coord_ctr.y) + 0.5);
	bounce_light = (1 - ( mix_horz + mix_vert)) * bounce_light + 
					mix_horz * bounce_light_mirrored_x + 
					mix_vert * bounce_light_mirrored_y;

	float bounce_center_Vignette = clamp(HSM_GetStrengthPowerVignetteFactor(bounce_coord_ctr * 0.5 + 0.5, 20, 2), 0, 1);
	bounce_light = mix(bounce_light, bounce_light_center, HSM_LIGHT_BOUNCE_MIX_FULLSCREEN_GLOW * bounce_center_Vignette);


	// Do HSV alterations on the bounce_light
	if (HSM_LIGHT_BOUNCE_SATURATION != 1 || HSM_LIGHT_BOUNCE_BRIGHTNESS != 1)
	{
		vec3 bounce_light_hsv = HSM_RGBtoHSV(bounce_light.rgb);
		bounce_light_hsv.y *= HSM_LIGHT_BOUNCE_SATURATION;
		bounce_light_hsv.z *= HSM_LIGHT_BOUNCE_BRIGHTNESS;
		bounce_light.rgb = HSM_HSVtoRGB(bounce_light_hsv);
	}

	// Add some noise to avoid banding
	bounce_light.rgb += HSM_DitheredNoise(vTexCoord, global.OutputSize.xy, 1);

	// if (HSM_LIGHT_BOUNCE_DISPLAY_MODE > 0.5)
	// {
	// 	FragColor = pow(bounce_light, vec4(1 / DEFAULT_SRGB_GAMMA));
	// 	return;
	// }

	// Prep the Nightlighting images
	vec3 nightlight1_rgb = vec3(1);
	vec3 nightlight2_rgb = vec3(1);
	HSM_Fill_Nightlight_Images(VIEWPORT_COORD, HSM_GetViewportCoordWithFlip(vTexCoord), TUBE_DIFFUSE_COORD, TUBE_DIFFUSE_SCALE, HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE, NightLightingImage, NightLighting2Image, nightlight1_rgb, nightlight2_rgb);

	vec4 composite_base_color = vec4(0);
	if (HSM_COMPOSITE_BASE_COLOR > 0.5)
	{
		// Red BG
		if (HSM_COMPOSITE_BASE_COLOR == 1)
			composite_base_color = vec4(1.0, 0.0, 0.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Pink (Magenta) BG
		if (HSM_COMPOSITE_BASE_COLOR == 2)
			composite_base_color = vec4(1.0, 0.0, 1.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Yellow BG
		if (HSM_COMPOSITE_BASE_COLOR == 3)
			composite_base_color = vec4(1.0, 1.0, 0.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Aqua BG
		if (HSM_COMPOSITE_BASE_COLOR == 4)
			composite_base_color = vec4(0.0, 1.0, 1.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
	}

	out_color = composite_base_color;

	if (HSM_COMPOSITE_BACKGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_background_orig = precomp_background;

		// Adjust brightness and add static shadows
		precomp_background.rgb *= HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER;
		precomp_background.rgb *= (1 - clamp(HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH * (1 - static_shadows), 0, 1));
		// precomp_background.rgb *= 1 - mix(vec3(0), vec3(static_shadows), HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH);

		// Apply Night Lighting Images
		precomp_background.rgb = ApplyNightlightImages(	precomp_background.rgb,
														nightlight1_rgb,
														nightlight2_rgb,
														HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER,
														HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER,
														1, 
														BLEND_MODE_NORMAL,
														HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);
		
		precomp_background.rgb *= 1 - clamp(HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH * (1 - static_shadows), 0, 1);

		// Add bounce light
		precomp_background.rgb += precomp_background_orig.rgb * HSM_COMPOSITE_BACKGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * mix((1 - precomp_middleground_shadow_stretch.a), 1, 1 - HSM_LIGHT_BOUNCE_SHADOW_STRENGTH);

		// Prep Color
		precomp_background.rgb = PrepGraphicsForHDR(precomp_background.rgb);

		// The background is always 100% opaque so we can just replace the out_color
		out_color = precomp_background;
	}

	// Add additional shadow depth by darkening bouncelight shadow areas

	vec4 crt_layer = vec4(0);
	if (HSM_COMPOSITE_CRT_AND_REFLECTION_ON > 0.5)
	{
		crt_layer = texture(MiddlePass, vTexCoord);
	}

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		crt_layer.rgb = vec3(0);
	}

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	// // TODO, need to see how to manage with parameters
	// #ifndef IS_NO_REFLECT_PRESET
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);

	TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	// // Apply Reflection Image Mask
	crt_layer *= TUBE_MASK + (1 - TUBE_MASK) * reflection_mask;
	// #endif

	if (HSM_COMPOSITE_MIDDLEGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_middleground_orig = precomp_middleground;

		// Adjust brightness and add static shadows
		precomp_middleground.rgb *= HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER * static_shadows;
		
		// Apply Night Lighting Images
		precomp_middleground.rgb = ApplyNightlightImages(	precomp_middleground.rgb,
														nightlight1_rgb,
														nightlight2_rgb,
														HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER,
														HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER,
														1, 
														BLEND_MODE_NORMAL,
														HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);
		// Add Bounce Light
		precomp_middleground.rgb += precomp_middleground_orig.rgb * HSM_COMPOSITE_MIDDLEGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * precomp_middleground_orig.a;

		// Prep Color
		precomp_middleground.rgb = PrepGraphicsForHDR(precomp_middleground.rgb);

		// Composite
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_middleground);
	}

	if (HSM_COMPOSITE_CRT_AND_REFLECTION_ON > 0.5 && HSM_RENDER_SIMPLE_MODE < 0.5)
		out_color = HSM_PreMultAlphaBlend(out_color, crt_layer);

	if (HSM_COMPOSITE_FOREGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_foreground_orig = precomp_foreground;

		// Adjust brightness and add static shadows
		precomp_foreground.rgb *= HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER * static_shadows;
		
		// Apply Night Lighting Images
		precomp_foreground.rgb = ApplyNightlightImages(	precomp_foreground.rgb,
														nightlight1_rgb,
														nightlight2_rgb,
														HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER,
														HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER,
														1, 
														BLEND_MODE_NORMAL,
														HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);

		// Add Bounce Light
		precomp_foreground.rgb += precomp_foreground_orig.rgb * HSM_COMPOSITE_FOREGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * precomp_foreground_orig.a;

		// Prep Color
		precomp_foreground.rgb = PrepGraphicsForHDR(precomp_foreground.rgb);

		// Composite
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_foreground);
	}

	if (HSM_COMPOSITE_ADDITIVE_ON > 0.5)
	{
		precomp_additive.rgb *= HSM_COMPOSITE_ADDITIVE_BRIGHTNESS;
		precomp_additive.rgb = PrepGraphicsForHDR(precomp_additive.rgb);
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_additive * vec4(1, 1, 1, 0));
	}

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
		if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
			bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

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

	if(HSM_COMPOSITE_SHOW_ALPHA > 0.5)
	{
		if (HSM_COMPOSITE_SHOW_ALPHA > 1.5)
			out_color.rgb = clamp(out_color.rgb * 0.5 + 0.5 * (1 - out_color.a) * vec3(0.5, 0.0, 0.0), 0.0, 1.0);
		else
			out_color = out_color.aaaa;
	}

#ifdef IS_HDR_PRESET	
	vec3 hdr_colour = vec3(0);
	LinearToOutputColor(out_color.rgb, hdr_colour);
	out_color = vec4(hdr_colour, 1.0f);
#else
	out_color = HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
#endif

	// // If we have calculated an image then set -1 as a flag to show that we have
	// if (vTexCoord.x < (2 / global.OutputSize.x) && vTexCoord.y < (2 / global.OutputSize.y))
	// 	FragColor.a = -1;

	FragColor = out_color;
}