/*
    Mega Bezel - Creates a graphic treatment for the game play area to give a retro feel
    Copyright (C) 2019-2021 HyperspaceMadness - HyperspaceMadness@outlook.com

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

#include "../base/common/common-functions-bezel.inc"

#pragma name TubeLayersPass

// 32 Bit Float format so we compress 4 8 bit rgba into 1 32 Bit rgba
#pragma format R32G32B32A32_SFLOAT

#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 vTexCoord;
layout(location = 7) out vec2 UNFLIPPED_VIEWPORT_COORD;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.00001;

	UNFLIPPED_VIEWPORT_COORD = vTexCoord;
}

#pragma stage fragment

layout(location = 0) in vec2 vTexCoord;
layout(location = 7) in vec2 UNFLIPPED_VIEWPORT_COORD;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D Source;

layout(set = 0, binding = 2) uniform sampler2D CacheInfoPass;
layout(set = 0, binding = 3) uniform sampler2D DerezedPass;

layout(set = 0, binding = 4) uniform sampler2D BackgroundImage;
layout(set = 0, binding = 5) uniform sampler2D BackgroundVertImage;

#ifdef USE_TUBEFX

layout(set = 0, binding = 6) uniform sampler2D TubeDiffuseImage;
layout(set = 0, binding = 7) uniform sampler2D TubeShadowImage;
layout(set = 0, binding = 8) uniform sampler2D TubeColoredGelImage;
layout(set = 0, binding = 9) uniform sampler2D TubeStaticReflectionImage;
layout(set = 0, binding = 10) uniform sampler2D NightLightingImage;
layout(set = 0, binding = 11) uniform sampler2D NightLighting2Image;

#endif

layout(set = 0, binding = 13) uniform sampler2D TubeFxPassFeedback;
#define PassFeedback TubeFxPassFeedback

// Torridgristle - ScanlineSimple pass - Public domain

#ifdef USE_TUBEFX
vec3 HSM_GetTubeHighlight(vec2 tube_curved_coord, bool apply_to_mirror, in sampler2D TubeStaticReflectionImage)
{
	tube_curved_coord = HSM_GetViewportCoordWithFlip(tube_curved_coord);
	// vec4 out_color = vec4(0);

	vec2 mirrored_tube_coord = tube_curved_coord;
	if (apply_to_mirror)
		mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	vec2 tube_highlight_coord = (mirrored_tube_coord - 0.5) / vec2(HSM_TUBE_STATIC_SCALE * HSM_TUBE_STATIC_SCALE_X, HSM_TUBE_STATIC_SCALE) + 0.5;
	tube_highlight_coord = (tube_highlight_coord - 0.5) / vec2(HSM_TUBE_STATIC_SCALE_X, 1) + 0.5;

	tube_highlight_coord.x = (tube_highlight_coord.x + HSM_TUBE_STATIC_POS_X - 0.5) * HSM_FLIP_VIEWPORT_HORIZONTAL + 0.5;
	tube_highlight_coord.y = (tube_highlight_coord.y + HSM_TUBE_STATIC_POS_Y - 0.5) * HSM_FLIP_VIEWPORT_VERTICAL + 0.5;

	// Mask the very outer edge of the tube for a very small shadowing effect
	float tube_highlight_mask = HSM_GetCornerMask((tube_curved_coord - 0.5) + 0.5 , TUBE_DIFFUSE_ASPECT, HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS, 0.9);

	vec4 tube_highlight_image = vec4(0);

	if (HSM_TUBE_STATIC_DITHER_SAMPLES > 0)
	{
		// Dithering for the glass reflection if needed to reduce banding
		tube_highlight_image = HSM_GetStoichaicBlurredSample(TubeStaticReflectionImage, tube_highlight_coord, HSM_TUBE_STATIC_DITHER_SAMPLES, HSM_TUBE_STATIC_DITHER_DISTANCE, HSM_TUBE_STATIC_DITHER_AMOUNT);
	}
	else
	{
		tube_highlight_image = HSM_GetMipmappedTexSample(TubeStaticReflectionImage, tube_highlight_coord, TUBE_SCALE, 0);
	}
	tube_highlight_image = HSM_Linearize(tube_highlight_image, DEFAULT_SRGB_GAMMA);

	vec3 out_color =	tube_highlight_image.rgb * 
						tube_highlight_image.a *
						tube_highlight_mask * 
						HSM_GLOBAL_GRAPHICS_BRIGHTNESS * 
						HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY;
						
	return out_color;
}
#endif

void main()
{
	HSM_UpdateCacheInfoChanged(CacheInfoPass);
	if (HSM_CACHE_GRAPHICS_ON > 0.5 && !CACHE_INFO_CHANGED ||
		HSM_AB_COMPARE_FREEZE_CRT_TUBE == 1 && HSM_GetIsInABCompareArea(vTexCoord))
	{
		FragColor = texture(PassFeedback, vTexCoord);
		return;
	}

	VIEWPORT_UNSCALED_COORD = HSM_GetViewportCoordWithFlip(vTexCoord);

	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);
	
	// if (HSM_IsOutsideReflectionBoundary())
	// {
	// 	vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	// 	if (HSM_CACHE_GRAPHICS_ON > 0.5 && feedback_color_test.a < 0 && !CACHE_INFO_CHANGED)
	// 	{
	// 		FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
	// 		return;
	// 	}
	// }

	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);

	vec4 color_linear = texture(Source, UNFLIPPED_VIEWPORT_COORD);

	vec2 VIEWPORT_COORD = vTexCoord;

	if (HSM_MONOCHROME_MODE > 0.5 && HSM_GetUseOnCurrentScreenIndex(HSM_MONOCHROME_DUALSCREEN_VIS_MODE))
		color_linear = HSM_ApplyMonochrome(color_linear);

	vec4 crt_color = color_linear;

	crt_color *= HSM_POST_CRT_BRIGHTNESS;

	vec2 mirrored_screen_coord = HSM_GetMirrorWrappedCoord(screen_curved_coord);

	// vec4 out_color = vec4(0);


	// TODO we see artifacts of the reflection at the edge of the screen when there is extra tube thickness and the reflection is scaled
	float screen_mask = HSM_GetCornerMask((screen_curved_coord - 0.5) * 0.999 + 0.5, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_SCREEN_CORNER_RADIUS_SCALE, 0.9);
						
	// Have to get the scale of the coordinates so we can figure out the size of the onscreen rectangle of the area 
	HSM_GetBezelCoords( TUBE_DIFFUSE_COORD, 
						TUBE_DIFFUSE_SCALE, 
						TUBE_SCALE, 
						SCREEN_ASPECT, 
						false,
						BEZEL_OUTSIDE_SCALE,
						BEZEL_OUTSIDE_COORD, 
						BEZEL_OUTSIDE_CURVED_COORD, 
						FRAME_OUTSIDE_CURVED_COORD);

	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);
	vec2 mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);
	
	float vignette_factor = HSM_GetScreenVignetteFactor(mirrored_tube_coord);
	if (HSM_GetUseScreenVignette())
	{
		float vignette_factor_outside_screen = HSM_SCREEN_VIGNETTE_IN_REFLECTION * vignette_factor + (1 - HSM_SCREEN_VIGNETTE_IN_REFLECTION) * 1;
		vignette_factor = screen_mask * vignette_factor + (1 - screen_mask) * vignette_factor_outside_screen;

		crt_color.rgb *= vignette_factor;
	}

// #ifdef USE_TUBEFX
// 	if (HSM_AB_COMPARE_SHOW_MODE == 1 && HSM_GetIsInABCompareArea(VIEWPORT_COORD))
// 	{
// 		vec2 ab_screen_coord = HSM_CRT_CURVATURE_SCALE * screen_curved_coord + (1 - HSM_CRT_CURVATURE_SCALE) * SCREEN_COORD;
// 		ab_screen_coord = HSM_GetMirrorWrappedCoord(ab_screen_coord);

// 		vec4 source_color = HSM_GetCroppedTexSample(DerezedPass, ab_screen_coord);

// 		source_color = HSM_Linearize(source_color, GAMMA_INPUT);

// 		crt_color = source_color;
// 	}
// #endif

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	float tube_mask =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	float screen_to_tube_mask_invert = 1 - (tube_mask - screen_mask);

	float black_edge_corner_radius = HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	vec2 tube_diffuse_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 0);
	float tube_diffuse_mask =  HSM_GetCornerMask(tube_diffuse_curved_coord, TUBE_DIFFUSE_ASPECT, black_edge_corner_radius, 0.99);

	float black_edge_mask_invert = 1 - (tube_mask - tube_diffuse_mask);

	// crt_color.rgb *= screen_to_tube_mask_invert;

// #ifdef USE_TUBEFX
	// AMBIENT LIGHTING IMAGES
	vec3 ambient_rgb = vec3(1);
	vec3 ambient2_rgb = vec3(1);
	HSM_Fill_Ambient_Images(VIEWPORT_COORD, 
							VIEWPORT_UNSCALED_COORD, 
							TUBE_DIFFUSE_COORD_MIXED_POS, 
							TUBE_DIFFUSE_SCALE_1ST_SCREEN, 
							HSM_AMBIENT_LIGHTING_SWAP_IMAGE_MODE, 
							NightLightingImage, 
							NightLighting2Image, 
							ambient_rgb, 
							ambient2_rgb);

	vec3 tube_shadow_mask = vec3(1);
	if (HSM_TUBE_SHADOW_IMAGE_ON > 0.5)
	{
		vec2 shadow_coord = tube_diffuse_curved_coord;
		shadow_coord = HSM_GetMirrorWrappedCoord(shadow_coord);

		// TODO Shadow Coordinate isn't right with 3D Curvature

		shadow_coord.x = (shadow_coord.x - HSM_TUBE_SHADOW_IMAGE_POS_X - 0.5) / HSM_TUBE_SHADOW_IMAGE_SCALE_X + 0.5;
		shadow_coord.y = (shadow_coord.y + HSM_TUBE_SHADOW_IMAGE_POS_Y) / HSM_TUBE_SHADOW_IMAGE_SCALE_Y;

		vec2 curvature_values = HSM_TUBE_SHADOW_CURVATURE_SCALE * HSM_GetCurvatureValues(TUBE_DIFFUSE_ASPECT);
		shadow_coord = HSM_Get2DCurvedCoord(shadow_coord, curvature_values);

		shadow_coord.x = HSM_FLIP_VIEWPORT_HORIZONTAL * (shadow_coord.x - 0.5) + 0.5;
		shadow_coord.y = HSM_FLIP_VIEWPORT_VERTICAL * (shadow_coord.y - 0.5) + 0.5;
		

		vec4 tube_shadow_sample = HSM_GetMipmappedTexSample(TubeShadowImage, shadow_coord, TUBE_SCALE * vec2(HSM_TUBE_SHADOW_IMAGE_SCALE_X, HSM_TUBE_SHADOW_IMAGE_SCALE_Y), 0);
		tube_shadow_sample = HSM_GLOBAL_GRAPHICS_BRIGHTNESS * HSM_GetPreMultipliedColorLinear(tube_shadow_sample, SOURCE_MATTE_PREMULTIPLIED, DEFAULT_SRGB_GAMMA);

		tube_shadow_mask = (1 - HSM_TUBE_SHADOW_IMAGE_OPACITY) + HSM_TUBE_SHADOW_IMAGE_OPACITY * tube_shadow_sample.rgb;
	}

	vec4 precomp_tube_diffuse = vec4(0, 0, 0, 1);
	float adjusted_tube_diffuse_amount = HSM_TUBE_DIFFUSE_MODE == 0 ? 0 : HSM_TUBE_DIFFUSE_IMAGE_AMOUNT;

	// When Tube is Black
	if (HSM_TUBE_DIFFUSE_MODE == 0)
	{
		precomp_tube_diffuse = vec4(0, 0, 0, 1);
	}
	if (HSM_TUBE_DIFFUSE_MODE == 1)
	{
		// TubeDiffuseImage
		vec2 diffuse_coord = tube_diffuse_curved_coord;

		diffuse_coord = HSM_GetMirrorWrappedCoord(diffuse_coord);
		diffuse_coord.x = HSM_FLIP_VIEWPORT_HORIZONTAL * (diffuse_coord.x - 0.5) + 0.5;
		diffuse_coord.y = HSM_FLIP_VIEWPORT_VERTICAL * (diffuse_coord.y - 0.5) + 0.5;

		diffuse_coord = (diffuse_coord - 0.5) / (HSM_TUBE_DIFFUSE_IMAGE_SCALE * vec2(HSM_TUBE_DIFFUSE_IMAGE_SCALE_X, 1)) + 0.5;

		// HSM_RotateCoordinate(diffuse_coord, HSM_TUBE_DIFFUSE_IMAGE_ROTATION)
		precomp_tube_diffuse = HSM_GetMipmappedTexSample(TubeDiffuseImage, HSM_RotateCoordinate(diffuse_coord, HSM_TUBE_DIFFUSE_IMAGE_ROTATION), TUBE_SCALE * HSM_TUBE_DIFFUSE_IMAGE_SCALE, 0);
		precomp_tube_diffuse.rgb = HSM_GLOBAL_GRAPHICS_BRIGHTNESS * HSM_GetPreMultipliedColorLinear(vec4(precomp_tube_diffuse.rgb, 1), SOURCE_MATTE_PREMULTIPLIED, DEFAULT_SRGB_GAMMA).rgb;
		precomp_tube_diffuse.rgb = HSM_ApplyHSVAdjustment(precomp_tube_diffuse.rgb, HSM_TUBE_DIFFUSE_IMAGE_HUE, HSM_TUBE_DIFFUSE_IMAGE_SATURATION, HSM_TUBE_DIFFUSE_IMAGE_BRIGHTNESS, HSM_TUBE_DIFFUSE_IMAGE_COLORIZE_ON, HSM_TUBE_DIFFUSE_IMAGE_GAMMA);
		precomp_tube_diffuse.rgb *= HSM_TUBE_DIFFUSE_IMAGE_AMOUNT;

		precomp_tube_diffuse.rgb *= tube_shadow_mask;

		// If CRT Blend Mode is Multiply (2) then the tube must be fully opaque
		precomp_tube_diffuse.rgb = HSM_ApplyAmbientImage(precomp_tube_diffuse.rgb, ambient_rgb, HSM_TUBE_DIFFUSE_IMAGE_AMBIENT_LIGHTING);
		precomp_tube_diffuse.rgb = HSM_ApplyAmbientImage(precomp_tube_diffuse.rgb, ambient2_rgb, HSM_TUBE_DIFFUSE_IMAGE_AMBIENT2_LIGHTING);
	}
	if (HSM_TUBE_DIFFUSE_MODE == 2)
	{
		precomp_tube_diffuse = vec4(0, 0, 0, 0);
	}

	precomp_tube_diffuse *= HSM_GetTubeOpacity();
	precomp_tube_diffuse.rgb *= tube_diffuse_mask;
	precomp_tube_diffuse *= tube_mask;

	// out_color = precomp_tube_diffuse;
	// out_color.rgb += crt_color.rgb;

	// // Don't apply the CRT images
	// if (HSM_CRT_BLEND_MODE == 0)
	// 	out_color = precomp_tube_diffuse;

	// // Apply the CRT image additively to the tube diffuse color
	// if (HSM_CRT_BLEND_MODE == 1)
	// 	// out_color = HSM_BlendModeLayerMix(precomp_tube_diffuse, out_color, BLEND_MODE_ADD, HSM_CRT_BLEND_AMOUNT);
	// 	out_color.rgb += precomp_tube_diffuse.rgb;

	// // Apply the CRT image with multiply blending
	// if (HSM_CRT_BLEND_MODE == 2)
	// {	
	// 	out_color = clamp(out_color, 0, 1);
	// 	out_color = HSM_BlendModeLayerMix(precomp_tube_diffuse, out_color, BLEND_MODE_MULTIPLY, HSM_CRT_BLEND_AMOUNT);
	// }

	vec4 precomp_gel = vec4(0);

	// Add a Colored Gel image on top of the screen, this is what colors games like space invaders or battlezone
	if (HSM_GetUseTubeColoredGelImage())
	{
		vec2 gel_coord = HSM_GetMirrorWrappedCoord(tube_diffuse_curved_coord);
		gel_coord.x = HSM_TUBE_COLORED_GEL_IMAGE_FLIP_HORIZONTAL * (gel_coord.x - 0.5) + 0.5;
		gel_coord.y = HSM_TUBE_COLORED_GEL_IMAGE_FLIP_VERTICAL * (gel_coord.y - 0.5) + 0.5;
		gel_coord = (gel_coord - 0.5) / HSM_TUBE_COLORED_GEL_IMAGE_SCALE + 0.5;

		vec4 gel_image = HSM_GetMipmappedTexSample(TubeColoredGelImage, gel_coord, TUBE_SCALE, 0);
		gel_image = HSM_GLOBAL_GRAPHICS_BRIGHTNESS * HSM_Linearize(gel_image, DEFAULT_SRGB_GAMMA);

		// if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0)
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb * gel_image.rgb, HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT);

		// if (HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0)
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb + gel_image.rgb, HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT);

		precomp_gel = gel_image;
		if (HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
		{
			precomp_gel.a = clamp( precomp_gel.a - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD, 0, 1 ) / (1 - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD);
			precomp_gel.rgb *= HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_BRIGHTNESS;
			precomp_gel.rgb *= tube_shadow_mask;

			if (HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT > 0.001)
				precomp_gel.rgb = HSM_ApplyScanlineMask(precomp_gel, SCREEN_SCALE, SCREEN_COORD, screen_curved_coord, tube_curved_coord, HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT).rgb;

			precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT_LIGHTING);
			precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient2_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT2_LIGHTING);

			precomp_gel = HSM_BlendModeLayerMix(precomp_gel, vec4(vec3(vignette_factor), 1), BLEND_MODE_MULTIPLY, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_VIGNETTE);

			// Create a precomp for the normal layering
			precomp_gel = HSM_BlendModeLayerMix(gel_image, vec4(0, 0, 0, 0), BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);

			// TODO, this needs to be applied in final comp pass
			// if ( HSM_SHOW_CRT_ON_TOP_OF_COLORED_GEL == 0 )
				// precomp_tube_diffuse = HSM_BlendModeLayerMix(precomp_tube_diffuse, gel_image, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
			
			// if ( HSM_SHOW_CRT_ON_TOP_OF_COLORED_GEL == 1 )
			// {
			// 	gel_image.rgb *= tube_shadow_mask;
			// 	vec4 normal_blended = HSM_BlendModeLayerMix(vec4(0, 0, 0, 1), gel_image, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
			// 	precomp_tube_diffuse = normal_blended + precomp_tube_diffuse;
			// }
		}
		precomp_gel = gel_image;
	}
// #endif

// #ifndef USE_TUBEFX
// 	out_color = crt_color;
// #endif

	// if (HSM_SCREEN_REFLECTION_ALLOW_REFLECTION > 0.5)
	// {
	// 	out_color.rgb *= black_edge_mask_invert;
	// }
	// else
	// {
		precomp_gel.rgb *= tube_diffuse_mask;
		precomp_gel *= tube_mask;
	// }

// #ifdef USE_TUBEFX
	vec4 precomp_tube_highlight = vec4(0);
	if (HSM_GetUseTubeStaticReflection())
	{
		vec3 tube_highlight_image = HSM_GetTubeHighlight(tube_curved_coord, false, TubeStaticReflectionImage);
		tube_highlight_image = HSM_ApplyAmbientImage(tube_highlight_image, ambient_rgb, HSM_TUBE_STATIC_AMBIENT_LIGHTING);
		tube_highlight_image = HSM_ApplyAmbientImage(tube_highlight_image, ambient2_rgb, HSM_TUBE_STATIC_AMBIENT2_LIGHTING);

		tube_highlight_image *= HSM_TUBE_STATIC_SHADOW_OPACITY * tube_shadow_mask + (1 - HSM_TUBE_STATIC_SHADOW_OPACITY);

		tube_highlight_image = clamp(HSM_TUBE_STATIC_BLACK_LEVEL * (tube_highlight_image - 1) + 1, 0, 1);

		tube_highlight_image *= 1 - HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY + HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY * adjusted_tube_diffuse_amount;

		precomp_tube_highlight.rgb = tube_highlight_image * HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY;
	}
// #endif

	precomp_tube_diffuse = HSM_Apply_Sinden_Lightgun_Border(precomp_tube_diffuse, tube_diffuse_curved_coord, tube_diffuse_mask, black_edge_corner_radius);

	precomp_tube_diffuse = HSM_Delinearize(precomp_tube_diffuse, DEFAULT_SRGB_GAMMA);
	precomp_gel = HSM_Delinearize(precomp_gel, DEFAULT_SRGB_GAMMA);
	precomp_tube_highlight = HSM_Delinearize(precomp_tube_highlight, DEFAULT_SRGB_GAMMA);
	precomp_tube_highlight.a = vignette_factor;
	vec4 precomp_nightlight = HSM_Delinearize(vec4(ambient_rgb, 1), DEFAULT_SRGB_GAMMA);
	 // Put Nightlighting images in YUV

	vec4 out_color = HSM_Pack_4_Rgba_To_1_Rgba(precomp_tube_highlight, precomp_tube_diffuse, precomp_gel, precomp_nightlight);

// #ifdef ADD_SIMPLE_BG
// 	out_color = HSM_ApplySimpleBG(out_color, vTexCoord, BackgroundImage, BackgroundVertImage);
// #endif

// #ifdef DELINEARIZE_TUBEFX
// 	out_color = HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
// #endif

// 	// If we have calculated an image then set -1 as a flag to show that we have
// 	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
// 		out_color.a = -1;

	FragColor = out_color;
	return;
}


/*

BG + Tube
Device
Layers in front of Tube

Reflection / Bounce Masks
1 per channel
Blurryness, Amount, Noise

Tube Diffuse & Tube Mask
Tube Highlight
Gel

*/
