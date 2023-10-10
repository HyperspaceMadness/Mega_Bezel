#version 450

/////////////// IMPORTS ///////////////
#include "common/globals-and-image-layers-params.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

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

#pragma format R32G32B32A32_SFLOAT

/////////////// DEFINES ///////////////

#define MASK_MODE_ALL 0
#define MASK_MODE_SCREEN 1
#define MASK_MODE_TUBE 2
#define MASK_MODE_INSIDE_BEZEL 3
#define MASK_MODE_BEZEL 4
#define MASK_MODE_OUTSIDE_TUBE 5
#define MASK_MODE_FRAME 6
#define MASK_MODE_OUTSIDE_BEZEL 7
#define MASK_MODE_OUTSIDE_FRAME 8

#define CUTOUT_MODE_OFF 0
#define CUTOUT_MODE_INSIDE 1
#define CUTOUT_MODE_OUTSIDE 2

vec2 VIEWPORT_COORD = vec2(0.5);

/////////////// Helper Functions ///////////////

// Return the mask for the specific mode
float GetMask(float mask_mode)
{
	float mask = 	(mask_mode == MASK_MODE_ALL) ? 1 :
					(mask_mode == MASK_MODE_SCREEN) ? TUBE_DIFFUSE_MASK :
					(mask_mode == MASK_MODE_TUBE) ? TUBE_MASK :
					(mask_mode == MASK_MODE_INSIDE_BEZEL) ? INSIDE_BEZEL_MASK :
					(mask_mode == MASK_MODE_BEZEL) ? BEZEL_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_TUBE) ? OUTSIDE_TUBE_MASK :
					(mask_mode == MASK_MODE_FRAME) ? FRAME_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_BEZEL) ? OUTSIDE_BEZEL_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_FRAME) ? OUTSIDE_FRAME_MASK : 0.5;
	return mask;
}

// Assumes Opacity is already encoded in alpha
vec4 BlendModeMaskLayerMix(vec4 color_under, vec4 color_over, float blend_mode, int precomp_index, float amount_of_add_mode_blend_in_base_precomp, float mask_mode, float cutout_mode, float dualscreen_mode, float layer_opacity)
{
	if ( blend_mode == 0 || (dualscreen_mode != SHOW_ON_DUALSCREEN_MODE_BOTH && dualscreen_mode != SCREEN_INDEX) )
		return color_under;

	bool is_additive_precomp = precomp_index == PRECOMP_INDEX_ADDITIVE;
	// Also do not composite multiply mode layers in the additive precomp
	if (is_additive_precomp && blend_mode == BLEND_MODE_MULTIPLY)
		return color_under;

	if (blend_mode == BLEND_MODE_OFF)
		return color_under;
	
	float cutout_mask = 1;
	if (cutout_mode == CUTOUT_MODE_INSIDE)
		cutout_mask = CUTOUT_MASK;
	if (cutout_mode == CUTOUT_MODE_OUTSIDE)
		cutout_mask = 1 - CUTOUT_MASK;
	color_over *= layer_opacity * GetMask(mask_mode) * cutout_mask;

	vec4 out_color = vec4(0);

	if (blend_mode == BLEND_MODE_NORMAL)
	{
		// color_over.rgb *= color_over.a;

		// Apply the normal layer as a black hold-out matte in the additive precomp
		// if (is_additive_precomp)
		// 	color_over.rgb *= 0;

		if (is_additive_precomp)
		{
			out_color = color_under * (1 - color_over.a);
		}
		else
			out_color = HSM_PreMultAlphaBlend(color_under, color_over);
	}

	if (blend_mode == BLEND_MODE_ADD)  	 		
	{
		float add_mode_amount = is_additive_precomp ? 1.0 - amount_of_add_mode_blend_in_base_precomp : amount_of_add_mode_blend_in_base_precomp;
		color_over.rgb *= add_mode_amount;
		color_over.a *= 0;
		out_color = HSM_PreMultAlphaBlend(color_under, color_over);
	}

	if (blend_mode == BLEND_MODE_MULTIPLY)
	{
		vec4 blend_color = color_under; 
		blend_color.rgb = color_under.rgb * color_over.rgb / (color_over.a + 0.0001);
		out_color.rgb = clamp(mix(color_under.rgb, blend_color.rgb, color_over.a), 0, 1);
		out_color.a = color_under.a;
	}

	return out_color;
}

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

	// VIEWPORT_COORD = vTexCoord * 1.0001;
	VIEWPORT_COORD = vTexCoord;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;
layout(location = 8) in vec3 BEZEL_FRAME_ORIGINAL_COLOR_RGB;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1)	uniform sampler2D CacheInfoPass;

//  Layers
layout(set = 0, binding = 2)	uniform sampler2D Background;
layout(set = 0, binding = 3)	uniform sampler2D BackgroundShadow;

layout(set = 0, binding = 4)	uniform sampler2D BackgroundLED;

layout(set = 0, binding = 5)	uniform sampler2D Device;
layout(set = 0, binding = 6)	uniform sampler2D DeviceShadow;

layout(set = 0, binding = 7)	uniform sampler2D DeviceBackgroundLED;
layout(set = 0, binding = 8)	uniform sampler2D Decal;
layout(set = 0, binding = 9)	uniform sampler2D CabinetGlass;
layout(set = 0, binding = 10)	uniform sampler2D TopLayer;

layout(set = 0, binding = 11)	uniform sampler2D FrameHighlight;
layout(set = 0, binding = 12)	uniform sampler2D ReflectionMask;

layout(set = 0, binding = 13) uniform sampler2D SceneLayersPassFeedback;
#define PassFeedback SceneLayersPassFeedback

// layout(set = 0, binding = 14)	uniform sampler2D BackgroundShadows;
// layout(set = 0, binding = 15)	uniform sampler2D BackgroundHeight;


// 15 is the max binding number which can be used

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    HSM_UpdateCacheInfoChanged(CacheInfoPass);

	// vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	if (HSM_CACHE_GRAPHICS_ON > 0.5 && !CACHE_INFO_CHANGED || HSM_AB_COMPARE_FREEZE_GRAPHICS == 1 && HSM_GetIsInABCompareArea(vTexCoord))
	{
		FragColor = texture(PassFeedback, vTexCoord);
		return;
	}

	vec2 vTexCoord_precomp = vTexCoord;

	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord_precomp);
	VIEWPORT_FLIPPED_UNZOOMED_COORD = HSM_GetViewportCoordWithFlip(vTexCoord_precomp);
	VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord_precomp);

	HSM_UpdateBezelAndTubeGlobalValuesAndMasks(TUBE_DIFFUSE_COORD, 
												TUBE_DIFFUSE_SCALE, 
												TUBE_SCALE, 
												TUBE_DIFFUSE_ASPECT, 
												BEZEL_OUTSIDE_SCALE,
												BEZEL_OUTSIDE_COORD, 
												BEZEL_OUTSIDE_CURVED_COORD, 
												FRAME_OUTSIDE_CURVED_COORD,
												OUTSIDE_BEZEL_MASK,
												OUTSIDE_FRAME_MASK,
												INSIDE_BEZEL_MASK,
												BEZEL_MASK,
												FRAME_MASK,
												OUTSIDE_TUBE_MASK
												);

	float output_aspect = global.OutputSize.x / global.OutputSize.y;
	vec4 bezel_layer = vec4(0);
	vec4 bg_image = vec4(0);
	vec4 device_image = vec4(0);
	vec4 deviceled_image = vec4(0);
	vec4 led_image = vec4(0);
	vec4 decal_image = vec4(0);
	vec4 top_image = vec4(0);
	vec4 cab_glass_image = vec4(0);
	vec4 tube_bg_layer = vec4(0);
	float static_shadow_multiply = 1;

	CUTOUT_MASK = 1;

	// ---------------------------------------------------------------------------------------
	// Generate Layer Coordinates and Sample s
	// ---------------------------------------------------------------------------------------
	{
		//----------------------------------------------------
		// Generated Bezel
		//----------------------------------------------------
		if (HSM_BZL_OPACITY > 0 || HSM_FRM_OPACITY > 0)
		{
			vec4 frame_highlight_color = HSM_Linearize(texture(FrameHighlight, FRAME_OUTSIDE_CURVED_COORD), DEFAULT_SRGB_GAMMA);
            bool generate_normals = false;
			bezel_layer = HSM_GetBezelLayer(generate_normals, frame_highlight_color, static_shadow_multiply, tube_bg_layer);
		}

		//-----------------------------------------------------------------------------------------
		// Background
		//-----------------------------------------------------------------------------------------
		vec2 BACKGROUND_SIZE = textureSize(Background, 0);
		BACKGROUND_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
													VIEWPORT_FLIPPED_UNZOOMED_COORD,
													TEXTURE_ASPECT_MODE_EXPLICIT,
													BACKGROUND_SIZE.x / BACKGROUND_SIZE.y,
													vec2(HSM_BG_POS_X, HSM_BG_POS_Y),
													vec2(HSM_BG_SCALE * HSM_BG_SCALE_X, HSM_BG_SCALE),
													HSM_BG_FOLLOW_LAYER,
													HSM_BG_FOLLOW_MODE,
													HSM_BG_FOLLOW_FULL_USES_ZOOM,
													HSM_BG_FILL_MODE,
													HSM_BG_SPLIT_PRESERVE_CENTER,
													HSM_BG_SPLIT_REPEAT_WIDTH,
													true,
													BACKGROUND_COORD,
													BACKGROUND_SCALE);

		// Tile Wrap the background
		if (HSM_BG_WRAP_MODE == 1)
			BACKGROUND_CURVED_COORD = mod(BACKGROUND_CURVED_COORD, 1.0);
		
		// Mirror Wrap the bBackground
		if (HSM_BG_WRAP_MODE == 2)
			BACKGROUND_CURVED_COORD = HSM_GetMirrorWrapCoord(BACKGROUND_CURVED_COORD);

		if (HSM_BG_OPACITY > 0 && BACKGROUND_SIZE.y > 16)
		{
			bg_image = HSM_GetMipmappedTexSample(Background, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);

			static_shadow_multiply *= HSM_GetMipmappedTexSample(BackgroundShadow, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS).r;

			if (textureSize(BackgroundShadow, 0).y > 16);
				static_shadow_multiply *= HSM_GetMipmappedTexSample(BackgroundShadow, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS).r;

			// Premultiply Alpha
			bg_image = HSM_GetPreMultipliedColorLinear(bg_image, HSM_BG_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

			// HSV Adjustments
			bg_image.rgb = HSM_ApplyHSVAdjustment(bg_image.rgb, HSM_BG_HUE, HSM_BG_SATURATION, HSM_BG_BRIGHTNESS, HSM_BG_COLORIZE_ON, HSM_BG_GAMMA);

			if (HSM_STATIC_LAYERS_GAMMA != 1)
				bg_image = HSM_ApplyGamma(bg_image, HSM_STATIC_LAYERS_GAMMA);
		}

		//----------------------------------------------------
		// Device 
		//----------------------------------------------------
		vec2 DEVICE_SIZE = textureSize(Device, 0);
		DEVICE_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
												VIEWPORT_FLIPPED_UNZOOMED_COORD,
												TEXTURE_ASPECT_MODE_EXPLICIT,
												DEVICE_SIZE.x / DEVICE_SIZE.y,
												vec2(HSM_DEVICE_POS_X, HSM_DEVICE_POS_Y),
												vec2(HSM_DEVICE_SCALE * HSM_DEVICE_SCALE_X, HSM_DEVICE_SCALE),
												HSM_DEVICE_FOLLOW_LAYER,
												HSM_DEVICE_FOLLOW_MODE,
												HSM_DEVICE_FOLLOW_FULL_USES_ZOOM,
												HSM_DEVICE_FILL_MODE,
												HSM_DEVICE_SPLIT_PRESERVE_CENTER,
												HSM_DEVICE_SPLIT_REPEAT_WIDTH,
												true,
												DEVICE_COORD,
												DEVICE_SCALE);

		if (HSM_DEVICE_OPACITY > 0 && DEVICE_SIZE.y > 16)
		{
			device_image = HSM_GetMipmappedTexSample(Device, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_DEVICE_MIPMAPPING_BLEND_BIAS);

			if (textureSize(DeviceShadow, 0).y > 16);
				static_shadow_multiply *= mix(1.0, HSM_GetMipmappedTexSample(DeviceShadow, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_DEVICE_MIPMAPPING_BLEND_BIAS).r, device_image.a);

			// Premultiply Alpha
			device_image = HSM_GetPreMultipliedColorLinear(device_image, HSM_DEVICE_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

			// HSV Adjustments
			device_image.rgb = HSM_ApplyHSVAdjustment(device_image.rgb, HSM_DEVICE_HUE, HSM_DEVICE_SATURATION, HSM_DEVICE_BRIGHTNESS, HSM_DEVICE_COLORIZE_ON, HSM_DEVICE_GAMMA);

			if (HSM_STATIC_LAYERS_GAMMA != 1)
				device_image = HSM_ApplyGamma(device_image, HSM_STATIC_LAYERS_GAMMA);
		}

		//----------------------------------------------------
		// DeviceLED 
		//----------------------------------------------------
		DEVICELED_SIZE = textureSize(DeviceBackgroundLED, 0);
		DEVICELED_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
												VIEWPORT_FLIPPED_UNZOOMED_COORD,
												TEXTURE_ASPECT_MODE_EXPLICIT,
												DEVICELED_SIZE.x / DEVICELED_SIZE.y,
												vec2(HSM_DEVICELED_POS_X, HSM_DEVICELED_POS_Y),
												vec2(HSM_DEVICELED_SCALE * HSM_DEVICELED_SCALE_X, HSM_DEVICELED_SCALE),
												HSM_DEVICELED_FOLLOW_LAYER,
												HSM_DEVICELED_FOLLOW_MODE,
												HSM_DEVICELED_FOLLOW_FULL_USES_ZOOM,
												HSM_DEVICELED_FILL_MODE,
												HSM_DEVICELED_SPLIT_PRESERVE_CENTER,
												HSM_DEVICELED_SPLIT_REPEAT_WIDTH,
												true,
												DEVICELED_COORD,
												DEVICELED_SCALE);

		if (HSM_DEVICELED_OPACITY > 0 && DEVICELED_SIZE.y > 16)
		{
			deviceled_image = HSM_GetMipmappedTexSample(DeviceBackgroundLED, DEVICELED_CURVED_COORD, DEVICELED_SCALE, HSM_DEVICELED_MIPMAPPING_BLEND_BIAS);

			// Premultiply Alpha
			deviceled_image = HSM_GetPreMultipliedColorLinear(deviceled_image, HSM_DEVICELED_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

			// HSV Adjustments
			deviceled_image.rgb = HSM_ApplyHSVAdjustment(deviceled_image.rgb, HSM_DEVICELED_HUE, HSM_DEVICELED_SATURATION, HSM_DEVICELED_BRIGHTNESS, HSM_DEVICELED_COLORIZE_ON, HSM_DEVICELED_GAMMA);

			if (HSM_STATIC_LAYERS_GAMMA != 1)
				deviceled_image = HSM_ApplyGamma(deviceled_image, HSM_STATIC_LAYERS_GAMMA);
		}

		//----------------------------------------------------
		// LED 
		//----------------------------------------------------
		if (HSM_LED_OPACITY > 0)
		{
			vec2 led_size = textureSize(BackgroundLED, 0);
			LED_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
												VIEWPORT_FLIPPED_UNZOOMED_COORD,
												TEXTURE_ASPECT_MODE_EXPLICIT,
												led_size.x / led_size.y, 
												vec2(HSM_LED_POS_X, HSM_LED_POS_Y), 
												vec2(HSM_LED_SCALE * HSM_LED_SCALE_X, HSM_LED_SCALE),
												HSM_LED_FOLLOW_LAYER,
												HSM_LED_FOLLOW_MODE,
												HSM_LED_FOLLOW_FULL_USES_ZOOM,
												HSM_LED_FILL_MODE,
												HSM_LED_SPLIT_PRESERVE_CENTER,
												HSM_LED_SPLIT_REPEAT_WIDTH,
												true,
												LED_COORD,
												LED_SCALE);

			if (HSM_LED_OPACITY > 0 && led_size.y > 16)
			{
				led_image = HSM_GetMipmappedTexSample(BackgroundLED, LED_CURVED_COORD, LED_SCALE, HSM_LED_MIPMAPPING_BLEND_BIAS);

				// Premultiply Alpha
				led_image = HSM_GetPreMultipliedColorLinear(led_image, HSM_LED_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

				// HSV Adjustments
				led_image.rgb = HSM_ApplyHSVAdjustment(led_image.rgb, HSM_LED_HUE, HSM_LED_SATURATION, HSM_LED_BRIGHTNESS, HSM_LED_COLORIZE_ON, HSM_LED_GAMMA);

				// STATIC GAMMA
				if (HSM_STATIC_LAYERS_GAMMA != 1)
					led_image = HSM_ApplyGamma(led_image, HSM_STATIC_LAYERS_GAMMA);
			}
		}

		//----------------------------------------------------
		// Decal 
		//----------------------------------------------------
		vec2 decal_size = textureSize(Decal, 0);
		DECAL_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
												VIEWPORT_FLIPPED_UNZOOMED_COORD,
												TEXTURE_ASPECT_MODE_EXPLICIT,
												decal_size.x / decal_size.y,
												vec2(HSM_DECAL_POS_X, HSM_DECAL_POS_Y), 
												vec2(HSM_DECAL_SCALE * HSM_DECAL_SCALE_X, HSM_DECAL_SCALE),
												HSM_DECAL_FOLLOW_LAYER,
												HSM_DECAL_FOLLOW_MODE,
												HSM_DECAL_FOLLOW_FULL_USES_ZOOM,
												HSM_DECAL_FILL_MODE,
												HSM_DECAL_SPLIT_PRESERVE_CENTER,
												HSM_DECAL_SPLIT_REPEAT_WIDTH,
												true,
												DECAL_COORD,
												DECAL_SCALE);
		if (HSM_DECAL_OPACITY > 0 && decal_size.y > 16)
		{
			decal_image = HSM_GetMipmappedTexSample(Decal, DECAL_CURVED_COORD, DECAL_SCALE, HSM_DECAL_MIPMAPPING_BLEND_BIAS);
			
			// Premultiply Alpha
			decal_image = HSM_GetPreMultipliedColorLinear(decal_image, HSM_DECAL_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

			// HSV Adjustments
			decal_image.rgb = HSM_ApplyHSVAdjustment(decal_image.rgb, HSM_DECAL_HUE, HSM_DECAL_SATURATION, HSM_DECAL_BRIGHTNESS, HSM_DECAL_COLORIZE_ON, HSM_DECAL_GAMMA);

			// STATIC GAMMA
			if (HSM_STATIC_LAYERS_GAMMA != 1)
				decal_image = HSM_ApplyGamma(decal_image, HSM_STATIC_LAYERS_GAMMA);
		}

		//----------------------------------------------------
		//  ADV Get Additional Layers and Composite 
		//----------------------------------------------------
		vec2 top_size = textureSize(TopLayer, 0);
		TOP_IMAGE_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
													VIEWPORT_FLIPPED_UNZOOMED_COORD,
													TEXTURE_ASPECT_MODE_EXPLICIT,
													top_size.x / top_size.y,
													vec2(HSM_TOP_POS_X, HSM_TOP_POS_Y), 
													vec2(HSM_TOP_SCALE * HSM_TOP_SCALE_X, HSM_TOP_SCALE),
													HSM_TOP_FOLLOW_LAYER,
													HSM_TOP_FOLLOW_MODE,
													HSM_TOP_FOLLOW_FULL_USES_ZOOM,
													HSM_TOP_FILL_MODE,
													HSM_TOP_SPLIT_PRESERVE_CENTER,
													HSM_TOP_SPLIT_REPEAT_WIDTH,
													true,
													TOP_IMAGE_COORD,
													TOP_IMAGE_SCALE);

		if (HSM_TOP_MIRROR_WRAP == 1)
			TOP_IMAGE_CURVED_COORD = HSM_GetMirrorWrapCoord(TOP_IMAGE_CURVED_COORD);

		if (HSM_TOP_OPACITY > 0 && top_size.y > 16)
		{
			// Get the top image color and masking values if needed
			top_image = HSM_GetMipmappedTexSample(TopLayer, TOP_IMAGE_CURVED_COORD, TOP_IMAGE_SCALE, HSM_TOP_MIPMAPPING_BLEND_BIAS);

			// Premultiply Alpha
			top_image = HSM_GetPreMultipliedColorLinear(top_image, HSM_TOP_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

			// HSV Adjustments
			top_image.rgb = HSM_ApplyHSVAdjustment(top_image.rgb, HSM_TOP_HUE, HSM_TOP_SATURATION, HSM_TOP_BRIGHTNESS, HSM_TOP_COLORIZE_ON, HSM_TOP_GAMMA);

			// STATIC GAMMA
			if (HSM_STATIC_LAYERS_GAMMA != 1)
				top_image = HSM_ApplyGamma(top_image, HSM_STATIC_LAYERS_GAMMA);
		}

		//----------------------------------------------------
		// Cabinet Glass 
		//----------------------------------------------------

		if (HSM_CAB_GLASS_OPACITY > 0)
		{
			vec2 cab_glass_size = textureSize(CabinetGlass, 0);
			CAB_GLASS_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
														VIEWPORT_FLIPPED_UNZOOMED_COORD,
														TEXTURE_ASPECT_MODE_EXPLICIT,
														cab_glass_size.x / cab_glass_size.y,
														vec2(HSM_CAB_GLASS_POS_X, HSM_CAB_GLASS_POS_Y), 
														vec2(HSM_CAB_GLASS_SCALE * HSM_CAB_GLASS_SCALE_X, HSM_CAB_GLASS_SCALE),
														HSM_CAB_GLASS_FOLLOW_LAYER,
														HSM_CAB_GLASS_FOLLOW_MODE,
														HSM_CAB_GLASS_FOLLOW_FULL_USES_ZOOM,
														HSM_CAB_GLASS_FILL_MODE,
														HSM_CAB_GLASS_SPLIT_PRESERVE_CENTER,
														HSM_CAB_GLASS_SPLIT_REPEAT_WIDTH,
														true,
														CAB_GLASS_COORD,
														CAB_GLASS_SCALE);

			if (HSM_CAB_GLASS_OPACITY > 0 && cab_glass_size.y > 16)
			{
				// Sample Texture
				cab_glass_image = HSM_GetMipmappedTexSample(CabinetGlass, CAB_GLASS_CURVED_COORD, CAB_GLASS_SCALE, HSM_CAB_GLASS_MIPMAPPING_BLEND_BIAS);

				// Premultiply Alpha
				cab_glass_image = HSM_GetPreMultipliedColorLinear(cab_glass_image, HSM_CAB_GLASS_SOURCE_MATTE_TYPE, DEFAULT_SRGB_GAMMA);

				// HSV Adjustments
				cab_glass_image.rgb = HSM_ApplyHSVAdjustment(cab_glass_image.rgb, HSM_CAB_GLASS_HUE, HSM_CAB_GLASS_SATURATION, HSM_CAB_GLASS_BRIGHTNESS, HSM_CAB_GLASS_COLORIZE_ON, HSM_CAB_GLASS_GAMMA);

				// STATIC GAMMA
				if (HSM_STATIC_LAYERS_GAMMA != 1)
					cab_glass_image = HSM_ApplyGamma(cab_glass_image, HSM_STATIC_LAYERS_GAMMA);
			}
		}


		//-----------------------------------------------------------------------------------------
		// CUTOUT MASK
		//-----------------------------------------------------------------------------------------
		vec2 temp_scale = vec2(0.5);
		vec2 temp_coord = vec2(0.5);

		vec2 cutout_base_scale_offset = vec2(0.5);

		if ( HSM_CUTOUT_FOLLOW_LAYER == FOLLOW_LAYER_VIEWPORT )
			cutout_base_scale_offset *= vec2(DEFAULT_UNCORRECTED_BEZEL_SCALE.y * TUBE_DIFFUSE_ASPECT / output_aspect, DEFAULT_BEZEL_SCALE.y);

		vec2 cutout_coord = HSM_GetScaledCoord(VIEWPORT_COORD,
												VIEWPORT_FLIPPED_UNZOOMED_COORD,
												HSM_CUTOUT_ASPECT_MODE,
												HSM_CUTOUT_EXPLICIT_ASPECT,
												vec2(HSM_CUTOUT_POS_X, HSM_CUTOUT_POS_Y),
												vec2(vec2(HSM_CUTOUT_SCALE * HSM_CUTOUT_SCALE_X, HSM_CUTOUT_SCALE) * cutout_base_scale_offset),
												HSM_CUTOUT_FOLLOW_LAYER,
												FOLLOW_MODE_SCALE_AND_POS,
												HSM_CUTOUT_FOLLOW_FULL_USES_ZOOM,
												FILL_MODE_STRETCH,
												0,
												0,
												false,
												temp_coord,
												temp_scale);
		
		CUTOUT_MASK = 1 - HSM_GetCornerMask(cutout_coord, TUBE_DIFFUSE_ASPECT, HSM_CUTOUT_CORNER_RADIUS, 0.8);

		//-----------------------------------------------------------------------------------------
		// Full Viewport Vignette
		//-----------------------------------------------------------------------------------------
		if (HSM_VIEWPORT_VIGNETTE_OPACITY > 0)
		{
			vec2 vignette_coord = HSM_GetScaledCoord(VIEWPORT_COORD,
													VIEWPORT_FLIPPED_UNZOOMED_COORD,
													TEXTURE_ASPECT_MODE_VIEWPORT,
													1,
													vec2(HSM_VIEWPORT_VIGNETTE_POS_X, HSM_VIEWPORT_VIGNETTE_POS_Y), 
													vec2(vec2(HSM_VIEWPORT_VIGNETTE_SCALE * HSM_VIEWPORT_VIGNETTE_SCALE_X, HSM_VIEWPORT_VIGNETTE_SCALE)), 
													HSM_VIEWPORT_VIGNETTE_FOLLOW_LAYER,
													FOLLOW_MODE_SCALE_AND_POS,
													FOLLOW_LAYER_VIEWPORT,
													FILL_MODE_STRETCH,
													0,
													0,
													false,
													temp_coord,
													temp_scale);
			vec4 vignette_layer = vec4(0);
			vignette_layer.a += 0.75 * HHLP_QuadraticBezier(1 - HSM_GetVignetteFactor(vignette_coord, HSM_VIEWPORT_VIGNETTE_OPACITY, 1), vec2(1, 0.5));

			// static_shadow_multiply = BlendModeMaskLayerMix(	static_shadow_multiply,
			// 												vignette_layer,
			// 												BLEND_MODE_NORMAL,
			// 												PRECOMP_INDEX_BACKGROUND,
			// 												HSM_BG_ADD_MODE_NIGHTLIGHTING_AMOUNT,
			// 												HSM_VIEWPORT_VIGNETTE_MASK_MODE, 
			// 												HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE,
			// 												SHOW_ON_DUALSCREEN_MODE_BOTH,
			// 												HSM_VIEWPORT_VIGNETTE_OPACITY);

			static_shadow_multiply *= 1 - vignette_layer.a;
		}
	}

	vec4 precomp_background = vec4(0);
	vec4 precomp_middleground = vec4(0);
	vec4 precomp_foreground = vec4(0);
	vec4 precomp_additive = vec4(0);

	// ---------------------------------------------------------------------------------------
	// COMPOSITE LAYERS INTO INDIVIDUAL PRE-COMPOSITES
	// ---------------------------------------------------------------------------------------
	int start_layer = 0;
	int end_layer = int(MAX_LAYER_ORDER);
	for (int precomp_index = 0; precomp_index < 4; precomp_index++)
	{
		// Make sure bezel layer order is not less than zero
		float bezel_layer_order = max(0, HSM_CRT_LAYER_ORDER + HSM_BEZEL_LAYER_ORDER_OFFSET_FROM_TUBE);
		// Make sure the middle layer group's first layer is not greater than the bezel layer order
		float middle_layer_group_first_layer = min(HSM_MIDDLE_LAYER_GROUP_FIRST_LAYER, bezel_layer_order);
		// Make sure the base layer is at least 1 less than the middle group first layer, and is not below 0
		float base_layer_group_first_layer = min(0, middle_layer_group_first_layer - 1);

		if (precomp_index == PRECOMP_INDEX_BACKGROUND)
		{
			start_layer = int(base_layer_group_first_layer);
			end_layer = int(middle_layer_group_first_layer - 1);
		}

		if (precomp_index == PRECOMP_INDEX_MIDDLEGROUND)		
		{
			start_layer = int(middle_layer_group_first_layer);
			end_layer = int(HSM_CRT_LAYER_ORDER);
		}

		if (precomp_index == PRECOMP_INDEX_FOREGROUND)
		{
			start_layer = int(HSM_CRT_LAYER_ORDER + 1);
			end_layer = int(MAX_LAYER_ORDER);
		}

		if (precomp_index == PRECOMP_INDEX_ADDITIVE)
		{
			start_layer = 0;
			end_layer = int(MAX_LAYER_ORDER);
		}

		// Setup for this Pre-Composite
		vec4 precomp_color = vec4(0);

		// Do this Pre-Composite by looping through the chosen layers
		for(int i=start_layer; i <= end_layer; i++)
		{
			// BACKGROUND
			if (HSM_BG_LAYER_ORDER == i)
			{
				precomp_color = BlendModeMaskLayerMix(	precomp_color,
														bg_image,
														HSM_BG_BLEND_MODE,
														precomp_index,
														HSM_BG_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_BG_MASK_MODE,
														HSM_BG_CUTOUT_MODE,
														HSM_BG_DUALSCREEN_VIS_MODE,
														HSM_BG_OPACITY);
			}


			// LED IMAGE
			if (HSM_LED_LAYER_ORDER == i && HSM_LED_OPACITY > 0)
			{		
				precomp_color = BlendModeMaskLayerMix(	precomp_color, 
														led_image, 
														HSM_LED_BLEND_MODE, 
														precomp_index,
														HSM_LED_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_LED_MASK_MODE, 
														HSM_LED_CUTOUT_MODE, 
														HSM_LED_DUALSCREEN_VIS_MODE,
														HSM_LED_OPACITY);
			}

			// DEVICE IMAGE
			if (HSM_DEVICE_LAYER_ORDER == i && HSM_DEVICE_OPACITY > 0)
			{
				precomp_color = BlendModeMaskLayerMix(precomp_color, 
														device_image, 
														HSM_DEVICE_BLEND_MODE, 
														precomp_index,
														HSM_DEVICE_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_DEVICE_MASK_MODE, 
														HSM_DEVICE_CUTOUT_MODE, 
														HSM_DEVICE_DUALSCREEN_VIS_MODE,
														HSM_DEVICE_OPACITY);
			}

			// DEVICELED IMAGE
			if (HSM_DEVICELED_LAYER_ORDER == i && HSM_DEVICELED_OPACITY > 0)
			{
				precomp_color = BlendModeMaskLayerMix(precomp_color, 
														deviceled_image, 
														HSM_DEVICELED_BLEND_MODE, 
														precomp_index,
														HSM_DEVICELED_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_DEVICELED_MASK_MODE, 
														HSM_DEVICELED_CUTOUT_MODE, 
														HSM_DEVICELED_DUALSCREEN_VIS_MODE,
														HSM_DEVICELED_OPACITY);
			}

			if (bezel_layer_order == i)
			{
				// Add background behind tube
				precomp_color = HSM_PreMultAlphaBlend(precomp_color, tube_bg_layer);

				// GENERATED BEZEL LAYER
				if (HSM_BZL_OPACITY > 0 || HSM_FRM_OPACITY > 0)
					precomp_color = BlendModeMaskLayerMix(precomp_color, 
															bezel_layer, 
															HSM_BZL_BLEND_MODE, 
															precomp_index,
															1,
															MASK_MODE_ALL, 
															0, 
															SHOW_ON_DUALSCREEN_MODE_BOTH,
															1);
			}

			// DECAL IMAGE
			if (HSM_DECAL_LAYER_ORDER == i && HSM_DECAL_OPACITY > 0)
			{
				precomp_color = BlendModeMaskLayerMix(precomp_color, 
														decal_image, 
														HSM_DECAL_BLEND_MODE, 
														precomp_index,
														HSM_DECAL_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_DECAL_MASK_MODE, 
														HSM_DECAL_CUTOUT_MODE, 
														HSM_DECAL_DUALSCREEN_VIS_MODE,
														HSM_DECAL_OPACITY);
			}

			// CABINET GLASS
			if (HSM_CAB_GLASS_LAYER_ORDER == i && HSM_CAB_GLASS_OPACITY > 0)
			{
				precomp_color = BlendModeMaskLayerMix(precomp_color, 
														cab_glass_image, 
														HSM_CAB_GLASS_BLEND_MODE,
														precomp_index,
														HSM_CAB_GLASS_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_CAB_GLASS_MASK_MODE, 
														HSM_CAB_GLASS_CUTOUT_MODE, 
														HSM_CAB_GLASS_DUALSCREEN_VIS_MODE,
														HSM_CAB_GLASS_OPACITY);
			}

			// Top Layer
			if (HSM_TOP_LAYER_ORDER == i && HSM_TOP_OPACITY > 0)
			{
				precomp_color = BlendModeMaskLayerMix(precomp_color, 
														top_image, 
														HSM_TOP_BLEND_MODE, 
														precomp_index,
														HSM_TOP_ADD_MODE_NIGHTLIGHTING_AMOUNT,
														HSM_TOP_MASK_MODE, 
														HSM_TOP_CUTOUT_MODE, 
														HSM_TOP_DUALSCREEN_VIS_MODE,
														HSM_TOP_OPACITY);
			}
		}

		if (precomp_index == PRECOMP_INDEX_BACKGROUND)
		{
			precomp_background = precomp_color;
		}

		if (precomp_index == PRECOMP_INDEX_MIDDLEGROUND)
		{
			precomp_middleground = precomp_color;
		}

		if (precomp_index == PRECOMP_INDEX_FOREGROUND)
		{
			precomp_foreground = precomp_color;
		}

		if (precomp_index == PRECOMP_INDEX_ADDITIVE)
		{
			precomp_additive = precomp_color;
		}
	}

	// This is the mask for all shadows which should always darken areas unless
	// they are illuminated by bounce light (or ambilight)
	// Shadow sources: user bg shadow image, height map.
	precomp_background.a = static_shadow_multiply;
	// precomp_background = static_shadow_multiply;


	// Store reflection mask in the alpha channel
	// Usually used to show the uneven brightness of a bumpy surface
	if (HSM_REFLECT_MASK_IMAGE_AMOUNT > 0)
	{
		vec2 reflect_mask_scale = vec2(0.5);
		vec2 reflect_mask_coord = HSM_GetLayerCoord(VIEWPORT_COORD, HSM_REFLECT_MASK_FOLLOW_LAYER, HSM_REFLECT_MASK_FOLLOW_MODE, reflect_mask_scale);
		float reflect_mask = HSM_GetMipmappedTexSample(ReflectionMask, reflect_mask_coord, reflect_mask_scale, HSM_REFLECT_MASK_MIPMAPPING_BLEND_BIAS).r;
		reflect_mask = clamp(HSM_REFLECT_MASK_BLACK_LEVEL * (reflect_mask - 1) + 1, 0, 1);
		reflect_mask *= HSM_REFLECT_MASK_BRIGHTNESS;

		precomp_additive.a = HSM_REFLECT_MASK_IMAGE_AMOUNT * reflect_mask + (1 - HSM_REFLECT_MASK_IMAGE_AMOUNT);
	}
	else
		precomp_additive.a = 1;


	//-----------------------------------------------------------------------------------------
	// MASK DEBUG DISPLAY
	//-----------------------------------------------------------------------------------------
	// Show a red overlay on the screen showing the mask for each mask mode
	if (HSM_LAYERING_DEBUG_MASK_MODE != -1)
	{
		float debug_mask = 1;
		if (HSM_LAYERING_DEBUG_MASK_MODE == -2)
			debug_mask = CUTOUT_MASK;
		else
			debug_mask = GetMask(HSM_LAYERING_DEBUG_MASK_MODE);

		precomp_foreground = HSM_PreMultAlphaBlend(precomp_foreground, vec4(1, 0, 0, 1) * 0.15 * debug_mask);
		precomp_foreground = HSM_PreMultAlphaBlend(precomp_foreground, vec4(0.05, 0.05, 0.05, 1) * 0.15 * (1 - debug_mask));
		precomp_foreground = clamp(precomp_foreground, 0, 1);
	}

	#ifdef ENABLE_CACHE_FALLBACK_CHECK
		vec2 viewport_coord_adjusted = vTexCoord_precomp;
		bool cache_info_changed = CACHE_INFO_CHANGED;
		bool show_update_indicator = cache_info_changed && ( HSM_CACHE_UPDATE_INDICATOR_MODE == 1 || HSM_CACHE_GRAPHICS_ON < 0.5 && HSM_CACHE_UPDATE_INDICATOR_MODE == 2);

		if (show_update_indicator)
		{
			float final_aspect = SCREEN_ASPECT;
			vec2 corner_offset = vec2(0.1, 0.1);
			vec2 center_coord = vec2(1 - corner_offset.x * final_aspect, 1 - corner_offset.y);
			vec2 test_coord = vec2(1 - (1 - vTexCoord_precomp.x) * final_aspect, vTexCoord_precomp.y);
			float distance =  length(test_coord - center_coord);

			if (distance < 0.04)
			{
				float modulation = clamp(0.4 + abs(mod(global.FrameCount, 90) / 90 - 0.5) * 1.2, 0, 1);
				modulation = HHLP_EasePowerInOut(modulation, 2);
				precomp_foreground *= 1 - modulation;
				precomp_foreground += modulation * vec4(1, 0, 0, 1);
			}
		}
	#endif

	// Need to encode final gamma in source pass since only 8bit
	// precomp_foreground = HSM_Delinearize(precomp_foreground, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_foreground = pow(precomp_foreground, vec4(1 / DEFAULT_SRGB_GAMMA));

	precomp_middleground = HSM_Delinearize(precomp_middleground, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_middleground = pow(precomp_middleground, vec4(1 / DEFAULT_SRGB_GAMMA));

	// precomp_background = HSM_Delinearize(precomp_background, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_background = pow(precomp_background, vec4(1 / DEFAULT_SRGB_GAMMA));

	// precomp_additive = HSM_Delinearize(precomp_additive, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_additive = pow(precomp_additive, vec4(1 / DEFAULT_SRGB_GAMMA));

	vec4 image_layers_packed = HSM_Pack_4_Rgba_To_1_Rgba(precomp_foreground
														,precomp_middleground
														,precomp_background
														,precomp_additive
														);

	FragColor = image_layers_packed;
	return;
}
