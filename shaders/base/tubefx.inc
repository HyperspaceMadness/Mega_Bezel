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

#pragma name TubeFxPass

// Float format so we can avoid banding
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

// layout(set = 0, binding = 1) uniform sampler2D CrtPass;

layout(set = 0, binding = 2) uniform sampler2D CacheInfoPass;
layout(set = 0, binding = 3) uniform sampler2D DerezedPass;

// layout(set = 0, binding = 4) uniform sampler2D BackgroundImage;
// layout(set = 0, binding = 5) uniform sampler2D BackgroundVertImage;

// #ifdef USE_TUBEFX

layout(set = 0, binding = 6) uniform sampler2D TubeDiffuseImage;
layout(set = 0, binding = 7) uniform sampler2D TubeShadowImage;
layout(set = 0, binding = 8) uniform sampler2D TubeColoredGelImage;
layout(set = 0, binding = 9) uniform sampler2D TubeStaticReflectionImage;
layout(set = 0, binding = 10) uniform sampler2D NightLightingImage;
layout(set = 0, binding = 11) uniform sampler2D NightLighting2Image;

// #endif

layout(set = 0, binding = 13) uniform sampler2D TubeFxPassFeedback;
#define PassFeedback TubeFxPassFeedback

// Torridgristle - ScanlineSimple pass - Public domain

// #ifdef USE_TUBEFX
vec3 HSM_GetTubeHighlight(vec2 tube_curved_coord, bool apply_to_mirror, in sampler2D TubeStaticReflectionImage)
{
	tube_curved_coord = HSM_GetViewportCoordWithFlip(tube_curved_coord);

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

	vec4 tube_highlight_image = vec4(0);

	// Dithering for the glass reflection if needed to reduce banding
	if (HSM_TUBE_STATIC_DITHER_SAMPLES > 0)
		tube_highlight_image = HSM_GetStoichaicBlurredSample(TubeStaticReflectionImage, 
															tube_highlight_coord, 
															HSM_TUBE_STATIC_DITHER_SAMPLES, 
															HSM_TUBE_STATIC_DITHER_DISTANCE, 
															HSM_TUBE_STATIC_DITHER_AMOUNT);
	else
		tube_highlight_image = HSM_GetMipmappedTexSample(TubeStaticReflectionImage, tube_highlight_coord, TUBE_SCALE, 0);

	// Mask the very outer edge of the tube for a very small shadowing effect
	float tube_highlight_mask = HSM_GetCornerMask((tube_curved_coord - 0.5) + 0.5, TUBE_DIFFUSE_ASPECT, HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS, 0.9);
	vec3 tube_highlight = 	HSM_Linearize(tube_highlight_image, DEFAULT_SRGB_GAMMA).rgb *
							tube_highlight_image.a *
							tube_highlight_mask *
							HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY * 
							HSM_GLOBAL_GRAPHICS_BRIGHTNESS;

	return tube_highlight;
}
// #endif

void main()
{
	HSM_UpdateCacheInfoChanged(CacheInfoPass);
	if (HSM_CACHE_GRAPHICS_ON > 0.5 && !CACHE_INFO_CHANGED ||
		HSM_AB_COMPARE_FREEZE_CRT_TUBE == 1 && HSM_GetIsInABCompareArea(vTexCoord))
	{
		FragColor = texture(PassFeedback, vTexCoord);
		return;
	}

	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);
	
	if (HSM_IsOutsideReflectionBoundary())
		return;

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

	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);

	// TODO we see artifacts of the reflection at the edge of the screen when there is extra tube thickness and the reflection is scaled
	float screen_mask = HSM_GetCornerMask((screen_curved_coord - 0.5) * 0.999 + 0.5, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_SCREEN_CORNER_RADIUS_SCALE, 0.9);
						
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);
	vec2 mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);
	
	float vignette_factor = HSM_GetScreenVignetteFactor(mirrored_tube_coord);
	if (HSM_GetUseScreenVignette())
	{
		float vignette_factor_outside_screen = HSM_SCREEN_VIGNETTE_IN_REFLECTION * vignette_factor + (1 - HSM_SCREEN_VIGNETTE_IN_REFLECTION) * 1;
		vignette_factor = screen_mask * vignette_factor + (1 - screen_mask) * vignette_factor_outside_screen;
	}

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	float tube_mask =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	float screen_to_tube_mask_invert = 1 - (tube_mask - screen_mask);

	float black_edge_corner_radius = HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	vec2 tube_diffuse_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 0);
	float tube_diffuse_mask =  HSM_GetCornerMask(tube_diffuse_curved_coord, TUBE_DIFFUSE_ASPECT, black_edge_corner_radius, 0.99);

	// TODO do we need to use HSM_TUBE_BLACK_EDGE_SHARPNESS here to mitigate aliasing on black edge

	float black_edge_mask_invert = 1 - (tube_mask - tube_diffuse_mask);

// #ifdef USE_TUBEFX
	// AMBIENT LIGHTING IMAGES
	vec3 ambient_rgb = vec3(1);
	vec3 ambient2_rgb = vec3(1);
	HSM_Fill_Ambient_Images(vTexCoord, 
							HSM_GetViewportCoordWithFlip(vTexCoord), 
							TUBE_DIFFUSE_COORD_MIXED_POS, 
							TUBE_DIFFUSE_SCALE_1ST_SCREEN, 
							HSM_AMBIENT_LIGHTING_SWAP_IMAGE_MODE, 
							NightLightingImage, 
							NightLighting2Image, 
							ambient_rgb, 
							ambient2_rgb);

	float tube_shadow = 1.0;
	if (HSM_TUBE_SHADOW_IMAGE_ON > 0.5 && (HSM_GetUseTubeDiffuseImage() || HSM_GetUseTubeColoredGelImage() || HSM_GetUseTubeStaticReflection()))
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

		tube_shadow = (1 - HSM_TUBE_SHADOW_IMAGE_OPACITY) + HSM_TUBE_SHADOW_IMAGE_OPACITY * max(max(tube_shadow_sample.r, tube_shadow_sample.g), tube_shadow_sample.b);
	}

	// If the crt blend mode is multiply then use the diffuse image
	vec4 precomp_tube_diffuse = vec4(0, 0, 0, tube_mask);
	float adjusted_tube_diffuse_amount = HSM_TUBE_DIFFUSE_MODE == 0 ? 0 : HSM_TUBE_DIFFUSE_IMAGE_AMOUNT;

	// When Tube is Black
	if (HSM_TUBE_DIFFUSE_MODE == 0)
	{
		precomp_tube_diffuse = vec4(0, 0, 0, 1);
	}
	// When tube uses texture
	if (HSM_TUBE_DIFFUSE_MODE == 1)
	{
		// TubeDiffuseImage
		if (adjusted_tube_diffuse_amount > 0 || HSM_CRT_BLEND_MODE == 2)
		{
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

			precomp_tube_diffuse.rgb *= tube_shadow;
		}

		// If CRT Blend Mode is Multiply (2) then the tube must be fully opaque
		precomp_tube_diffuse.rgb = HSM_ApplyAmbientImage(precomp_tube_diffuse.rgb, ambient_rgb, HSM_TUBE_DIFFUSE_IMAGE_AMBIENT_LIGHTING);
		precomp_tube_diffuse.rgb = HSM_ApplyAmbientImage(precomp_tube_diffuse.rgb, ambient2_rgb, HSM_TUBE_DIFFUSE_IMAGE_AMBIENT2_LIGHTING);
	}
	if (HSM_TUBE_DIFFUSE_MODE == 2)
	{
		precomp_tube_diffuse = vec4(0, 0, 0, 0);
	}

	// Outside the tube, it is transparent and applied additively
	precomp_tube_diffuse *= HSM_GetTubeOpacity();
	precomp_tube_diffuse.a *= tube_mask;

	precomp_tube_diffuse = HSM_Apply_Sinden_Lightgun_Border(precomp_tube_diffuse, tube_diffuse_curved_coord, tube_diffuse_mask, black_edge_corner_radius);

	// Add a Colored Gel image on top of the screen, this is what colors games like space invaders or battlezone
	vec4 precomp_gel;
	if (HSM_GetUseTubeColoredGelImage())
	{
		vec2 gel_coord = HSM_GetMirrorWrappedCoord(tube_diffuse_curved_coord);
		gel_coord.x = HSM_TUBE_COLORED_GEL_IMAGE_FLIP_HORIZONTAL * (gel_coord.x - 0.5) + 0.5;
		gel_coord.y = HSM_TUBE_COLORED_GEL_IMAGE_FLIP_VERTICAL * (gel_coord.y - 0.5) + 0.5;
		gel_coord = (gel_coord - 0.5) / HSM_TUBE_COLORED_GEL_IMAGE_SCALE + 0.5;

		vec4 gel_image = HSM_GetMipmappedTexSample(TubeColoredGelImage, gel_coord, TUBE_SCALE, HSM_TUBE_COLORED_GEL_MIPMAPPING_BLEND_BIAS);
		gel_image = HSM_Linearize(gel_image, DEFAULT_SRGB_GAMMA) * HSM_GLOBAL_GRAPHICS_BRIGHTNESS;

		// Tube Diffuse apply Gel Multiply
		if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0)
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb * gel_image.rgb * clamp(HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_BRIGHTNESS * gel_image.rgb, 0, 1), HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT);

		// Tube Diffuse apply Gel Additive
		if (HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0)
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb + gel_image.rgb, gel_image.a * HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT);

		// Prepare Gel precomp for application later
		precomp_gel = gel_image;
		// if (HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
		// {
		precomp_gel.a = clamp( precomp_gel.a - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD, 0, 1 ) / (1 - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD);
		precomp_gel.rgb *= HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_BRIGHTNESS;

		if (HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT > 0.001)
			precomp_gel.rgb = HSM_ApplyScanlineMask(precomp_gel, SCREEN_SCALE, SCREEN_COORD, screen_curved_coord, tube_curved_coord, HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT).rgb;

		precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT_LIGHTING);
		precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient2_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT2_LIGHTING);

		// Multiply needs to take into account premultiplied color
		precomp_gel = HSM_BlendModeLayerMix(precomp_gel, vec4(vec3(vignette_factor), 1), BLEND_MODE_MULTIPLY, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_VIGNETTE);

		// Create a precomp for the normal layering
		precomp_gel = HSM_BlendModeLayerMix(precomp_gel, vec4(0, 0, 0, 0), BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
		// }
	}
// #endif

	// if (HSM_SCREEN_REFLECTION_ALLOW_REFLECTION > 0.5)
	// {
	// 	precomp_gel.rgb  *= black_edge_mask_invert;
	// }
	// else
	// {
	// 	precomp_gel *= tube_mask;
	// }

// #ifdef USE_TUBEFX
	vec4 precomp_tube_highlight = vec4(0);
	if (HSM_GetUseTubeStaticReflection())
	{
		precomp_tube_highlight.rgb = HSM_GetTubeHighlight(tube_curved_coord, false, TubeStaticReflectionImage);
		precomp_tube_highlight.rgb = HSM_ApplyAmbientImage(precomp_tube_highlight.rgb, ambient_rgb, HSM_TUBE_STATIC_AMBIENT_LIGHTING);
		precomp_tube_highlight.rgb = HSM_ApplyAmbientImage(precomp_tube_highlight.rgb, ambient2_rgb, HSM_TUBE_STATIC_AMBIENT2_LIGHTING);

		precomp_tube_highlight.rgb *= HSM_TUBE_STATIC_SHADOW_OPACITY * tube_shadow + (1 - HSM_TUBE_STATIC_SHADOW_OPACITY);
		precomp_tube_highlight.rgb = clamp(HSM_TUBE_STATIC_BLACK_LEVEL * (precomp_tube_highlight.rgb - 1) + 1, 0, 1);
		precomp_tube_highlight.rgb *= 1 - HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY + HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY * adjusted_tube_diffuse_amount;
		precomp_tube_highlight.rgb *= HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY;
	}
	precomp_tube_highlight.a = vignette_factor;
// #endif

	float crt_mask = screen_to_tube_mask_invert;

	// Clip everything in the black edge area, perhaps this only needs to happen in the composite stage?
	if (HSM_SCREEN_REFLECTION_ALLOW_REFLECTION > 0.5)
	{
		precomp_gel.rgb  *= black_edge_mask_invert;
		precomp_tube_diffuse.rgb  *= black_edge_mask_invert;
		crt_mask *= black_edge_mask_invert;
	}
	// Clip everything outside of the tube
	else
	{
		precomp_tube_diffuse.rgb *= tube_diffuse_mask;
		precomp_tube_diffuse *= tube_mask;
		precomp_gel.rgb *= tube_diffuse_mask;
		precomp_gel *= tube_mask;
		crt_mask *= tube_diffuse_mask;
	}

	// Need to encode final gamma in source pass since only 8bit
	precomp_tube_diffuse = HSM_Delinearize(precomp_tube_diffuse, DEFAULT_SRGB_GAMMA);
	precomp_gel = HSM_Delinearize(precomp_gel, DEFAULT_SRGB_GAMMA);
	precomp_tube_highlight = HSM_Delinearize(precomp_tube_highlight, DEFAULT_SRGB_GAMMA);

	vec4 precomp_nightlight = vec4(ambient_rgb, 1);
	precomp_nightlight = HSM_Delinearize(precomp_nightlight, DEFAULT_SRGB_GAMMA);
	precomp_nightlight.b = crt_mask;
	precomp_nightlight.a = tube_shadow;

	vec4 image_layers_packed = HSM_Pack_4_Rgba_To_1_Rgba(precomp_tube_diffuse
														,precomp_gel
														,precomp_tube_highlight
														,precomp_nightlight
														);

//------------------------------------------------------------------------------------------
// Do the Final Composite
//------------------------------------------------------------------------------------------

	// vec4 crt_color = texture(CrtPass, UNFLIPPED_VIEWPORT_COORD);

	// // #ifdef USE_TUBEFX
	// if (HSM_AB_COMPARE_SHOW_MODE == 1 && HSM_GetIsInABCompareArea(vTexCoord))
	// {
	// 	vec2 ab_screen_coord = HSM_CRT_CURVATURE_SCALE * screen_curved_coord + (1 - HSM_CRT_CURVATURE_SCALE) * SCREEN_COORD;
	// 	ab_screen_coord = HSM_GetMirrorWrappedCoord(ab_screen_coord);

	// 	vec4 drezzed_color = HSM_GetCroppedTexSample(DerezedPass, ab_screen_coord);
	// 	crt_color = HSM_Linearize(drezzed_color, GAMMA_INPUT);
	// }

	// vec4 crt_color = vec4(0);
	// vec4 out_color = HSM_ApplyPackedTubeLayers(crt_color, image_layers_packed);

	vec4 out_color = image_layers_packed;

// 	vec4 unpacked_precomp_tube_diffuse = vec4(0);
// 	vec4 unpacked_precomp_gel = vec4(0);
// 	vec4 unpacked_precomp_tube_highlight = vec4(0);
// 	vec4 unpacked_precomp_nightlight = vec4(0);

// 	HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
// 								unpacked_precomp_tube_diffuse, 
// 								unpacked_precomp_gel, 
// 								unpacked_precomp_tube_highlight, 
// 								unpacked_precomp_nightlight);

// 	unpacked_precomp_tube_diffuse = HSM_Linearize(unpacked_precomp_tube_diffuse, DEFAULT_SRGB_GAMMA);
// 	unpacked_precomp_gel = HSM_Linearize(unpacked_precomp_gel, DEFAULT_SRGB_GAMMA);
// 	unpacked_precomp_tube_highlight = HSM_Linearize(unpacked_precomp_tube_highlight, DEFAULT_SRGB_GAMMA);
// 	float unpacked_vignette = unpacked_precomp_tube_highlight.a;

// 	unpacked_precomp_nightlight = HSM_Linearize(unpacked_precomp_nightlight, DEFAULT_SRGB_GAMMA);
// 	float unpacked_shadow = unpacked_precomp_nightlight.a;


// 	vec4 out_color = unpacked_precomp_tube_diffuse;

// 	vec4 crt_color = texture(CrtPass, UNFLIPPED_VIEWPORT_COORD);

// 	if (HSM_MONOCHROME_MODE > 0.5 && HSM_GetUseOnCurrentScreenIndex(HSM_MONOCHROME_DUALSCREEN_VIS_MODE))
// 		crt_color = HSM_ApplyMonochrome(crt_color);

// 	crt_color *= HSM_POST_CRT_BRIGHTNESS;

// // #ifdef USE_TUBEFX
// 	if (HSM_AB_COMPARE_SHOW_MODE == 1 && HSM_GetIsInABCompareArea(vTexCoord))
// 	{
// 		vec2 ab_screen_coord = HSM_CRT_CURVATURE_SCALE * screen_curved_coord + (1 - HSM_CRT_CURVATURE_SCALE) * SCREEN_COORD;
// 		ab_screen_coord = HSM_GetMirrorWrappedCoord(ab_screen_coord);

// 		vec4 drezzed_color = HSM_GetCroppedTexSample(DerezedPass, ab_screen_coord);
// 		crt_color = HSM_Linearize(drezzed_color, GAMMA_INPUT);
// 	}
// // #endif

// // #ifndef USE_TUBEFX
// // 	out_color = crt_color;
// // #endif

// 	crt_color.rgb *= screen_to_tube_mask_invert;

// 	if (HSM_SCREEN_REFLECTION_ALLOW_REFLECTION > 0.5)
// 	{
// 		crt_color.rgb *= black_edge_mask_invert;
// 	}
// 	else
// 	{
// 		crt_color *= tube_diffuse_mask;
// 	}

// 	// Multiply CRT image by Vignette
// 	if (HSM_GetUseScreenVignette())
// 		crt_color.rgb *= unpacked_vignette;

// 	// Multiply CRT image by gel
// 	if (HSM_GetUseTubeColoredGelImage())
// 	{
// 		// Add Gel with Over/Normal blending on top of tube before we add the CRT
// 		if ( HSM_TUBE_COLORED_GEL_LAYERING_ORDER == 1 )
// 		{
// 			// out_color += HSM_BlendModeLayerMix(vec4(0, 0, 0, 1), unpacked_precomp_gel, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
// 			precomp_gel.rgb *= tube_shadow;
// 			out_color = HSM_BlendModeLayerMix(out_color, unpacked_precomp_gel, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
// 		}

// 		crt_color.rgb *= HSM_TUBE_COLORED_GEL_CRT_BRIGHTNESS;
// 		if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0)
// 			crt_color.rgb = mix(crt_color.rgb, crt_color.rgb * clamp(HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_BRIGHTNESS * unpacked_precomp_gel.rgb, 0, 1), HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT);

// 	}

// 	// Apply the CRT image Additive Blend
// 	if (HSM_CRT_BLEND_MODE == 1)
// 		out_color = HSM_BlendModeLayerMix(out_color, crt_color, BLEND_MODE_ADD, HSM_CRT_BLEND_AMOUNT);
// 	// Apply the CRT image with Multiply Blend
// 	else if (HSM_CRT_BLEND_MODE == 2)
// 	{	
// 		out_color = clamp(out_color, 0, 1);
// 		out_color = HSM_BlendModeLayerMix(out_color, crt_color, BLEND_MODE_MULTIPLY, HSM_CRT_BLEND_AMOUNT);
// 	}
// 	// Apply the CRT image with "Over" Blend, probably useful for something like Gameboy DMG
// 	else if (HSM_CRT_BLEND_MODE == 3)
// 	{	
// 		out_color = clamp(out_color, 0, 1);
// 		out_color = HSM_BlendModeLayerMix(out_color, crt_color, BLEND_MODE_NORMAL, HSM_CRT_BLEND_AMOUNT);
// 	}

// 	if (HSM_GetUseTubeColoredGelImage())
// 	{
// 		// Put Gel on top of CRT (Default situation)
// 		precomp_gel.rgb *= tube_shadow;
// 		if ( HSM_TUBE_COLORED_GEL_LAYERING_ORDER == 0 )
// 			out_color = HSM_BlendModeLayerMix(out_color, unpacked_precomp_gel, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
// 	}

// 	// Add Tube Highlight
// 	out_color.rgb += unpacked_precomp_tube_highlight.rgb;

// #ifdef ADD_SIMPLE_BG
// 	out_color = HSM_ApplySimpleBG(out_color, vTexCoord, BackgroundImage, BackgroundVertImage);
// #endif

	#ifdef DELINEARIZE_TUBEFX
		out_color = HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
	#endif

	// If we have calculated an image then set -1 as a flag to show that we have
	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
		out_color.a = -1;


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
