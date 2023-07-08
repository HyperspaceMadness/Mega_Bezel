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

layout(set = 0, binding = 2) uniform sampler2D CacheInfoPass;

// #ifdef USE_TUBEFX

layout(set = 0, binding = 6) uniform sampler2D TubeDiffuseImage;
layout(set = 0, binding = 7) uniform sampler2D TubeShadowImage;
layout(set = 0, binding = 8) uniform sampler2D TubeColoredGelImage;
layout(set = 0, binding = 9) uniform sampler2D TubeStaticReflectionImage;
layout(set = 0, binding = 10) uniform sampler2D NightLightingImage;
layout(set = 0, binding = 11) uniform sampler2D NightLighting2Image;
layout(set = 0, binding = 12) uniform sampler2D SceneLayersPass;


// #endif

layout(set = 0, binding = 13) uniform sampler2D TubeFxPassFeedback;
#define PassFeedback TubeFxPassFeedback


// Generating height & normals from a shadertoy by granito
const vec3 GrayscaleWeights = vec3(0.2125,0.7154,0.0721);

// Sample map to generate Normal map and Height map
vec4 GenerateNormalHeight (sampler2D tex, vec2 uv, vec2 res, float width)
{
    vec2 texelSize = 1. / (res * width);
    vec4 h;
    h[0] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 0,-1)) ).rgb);
    h[1] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2(-1, 0)) ).rgb);
    h[2] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 1, 0)) ).rgb);
    h[3] = dot(GrayscaleWeights, texture(tex, uv + vec2(texelSize * vec2( 0, 1)) ).rgb);
    vec3 n;
    n.y = h[0] - h[3];
    n.x = h[1] - h[2];
    n.z = .25;
    float height = dot(GrayscaleWeights, texture(tex, uv).rgb);
    return vec4(normalize(n), height);
}

// Multipass version of above
vec4 GenerateNormalHeightMultipass (sampler2D tex, vec2 uv, vec2 res)
{
    float dist = 0.5;
    vec4 multi;
	multi += GenerateNormalHeight(tex, uv, res, 1. * dist);
	multi += GenerateNormalHeight(tex, uv, res, 2.5 * dist);
	multi += GenerateNormalHeight(tex, uv, res, 5. * dist);
	multi += GenerateNormalHeight(tex, uv, res, 10. * dist);    
    multi *= 0.25;
    return multi;
}

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
	{
		FragColor = vec4(0);
		return;
	}

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
			precomp_tube_diffuse.rgb = HSM_GLOBAL_GRAPHICS_BRIGHTNESS * HSM_GetPreMultipliedColorLinear(vec4(precomp_tube_diffuse.rgb, 1.0), SOURCE_MATTE_PREMULTIPLIED, DEFAULT_SRGB_GAMMA).rgb;
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
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb * gel_image.rgb * clamp(HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_BRIGHTNESS * gel_image.rgb, 0, 1.0), HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT);

		// Tube Diffuse apply Gel Additive
		if (HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0)
			precomp_tube_diffuse.rgb = mix(precomp_tube_diffuse.rgb, precomp_tube_diffuse.rgb + gel_image.rgb, gel_image.a * HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT);

		// Prepare Gel precomp for application later
		precomp_gel = gel_image;
		// if (HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
		// {
		
		// TODO, Colored Gel Multiply is clipping because of transparency threshold on AMD
		precomp_gel.a = clamp( precomp_gel.a - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD, 0, 1.0 ) / (1.0 - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD);
		precomp_gel.rgb *= HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_BRIGHTNESS;

		if (HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT > 0.001)
			precomp_gel.rgb = HSM_ApplyScanlineMask(precomp_gel, SCREEN_SCALE, SCREEN_COORD, screen_curved_coord, tube_curved_coord, HSM_TUBE_COLORED_GEL_IMAGE_FAKE_SCANLINE_AMOUNT).rgb;

		precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT_LIGHTING);
		precomp_gel.rgb = HSM_ApplyAmbientImage(precomp_gel.rgb, ambient2_rgb, HSM_TUBE_COLORED_GEL_IMAGE_AMBIENT2_LIGHTING);

		// Multiply needs to take into account premultiplied color
		precomp_gel = HSM_BlendModeLayerMix(precomp_gel, vec4(vec3(vignette_factor), 1), BLEND_MODE_MULTIPLY, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_VIGNETTE);

		// Create a precomp for the normal layering
		precomp_gel = HSM_BlendModeLayerMix(precomp_gel, vec4(0, 0, 0, 0), BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
		precomp_gel = clamp(precomp_gel, 1.0/255.0, 1.0);
		// }
	}
// #endif

// #ifdef USE_TUBEFX
	vec4 precomp_tube_highlight = vec4(0);
	if (HSM_GetUseTubeStaticReflection())
	{
		precomp_tube_highlight.rgb = HSM_GetTubeHighlight(tube_curved_coord, false, TubeStaticReflectionImage);
		precomp_tube_highlight.rgb = HSM_ApplyAmbientImage(precomp_tube_highlight.rgb, ambient_rgb, HSM_TUBE_STATIC_AMBIENT_LIGHTING);
		precomp_tube_highlight.rgb = HSM_ApplyAmbientImage(precomp_tube_highlight.rgb, ambient2_rgb, HSM_TUBE_STATIC_AMBIENT2_LIGHTING);

		precomp_tube_highlight.rgb *= HSM_TUBE_STATIC_SHADOW_OPACITY * tube_shadow + (1 - HSM_TUBE_STATIC_SHADOW_OPACITY);
		precomp_tube_highlight.rgb = clamp(HSM_TUBE_STATIC_BLACK_LEVEL * (precomp_tube_highlight.rgb - 1.0) + 1.0, 0, 1.0);
		precomp_tube_highlight.rgb *= 1 - HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY + HSM_TUBE_STATIC_OPACITY_DIFFUSE_MULTIPLY * adjusted_tube_diffuse_amount;
		precomp_tube_highlight.rgb *= HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY;
	}

	// 0.25 is added to avoid clipping issues with AMD GPUs
	precomp_tube_highlight.a = vignette_factor + 0.25;
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

	vec4 precomp_lighting_mask = vec4(0);
	// TODO fill in with lighting shadow masks
	precomp_lighting_mask.b = 1; //crt_mask;
	precomp_lighting_mask.a = tube_shadow;

	vec4 image_layers_packed = HSM_Pack_4_Rgba_To_1_Rgba(precomp_tube_diffuse
														,precomp_gel
														,precomp_tube_highlight
														,precomp_lighting_mask
														);

	vec4 out_color = image_layers_packed;

// #ifdef ADD_SIMPLE_BG
// 	out_color = HSM_ApplySimpleBG(out_color, vTexCoord, BackgroundImage, BackgroundVertImage);
// #endif

	// If we have calculated an image then set -1 as a flag to show that we have
	// if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
	// 	out_color.a = -1;

	FragColor = out_color;
	return;
}