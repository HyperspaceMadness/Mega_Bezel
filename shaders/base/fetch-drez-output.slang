#version 450

/*
   hsm-fetch-drez-output
   Mega Bezel, Used after the cache pass, to fetch the Dres pass (which is before the cache pass)
   to pass it down the chain
*/ 

#include "common/globals-and-screen-scale-params.inc"
#include "common/helper-functions.inc"
#include "common/base-functions.inc"

#pragma name FetchDrezPass

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
} params;

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
layout(set = 0, binding = 1) uniform sampler2D CacheInfoPass;
layout(set = 0, binding = 2) uniform sampler2D DerezedPass;
layout(set = 0, binding = 3) uniform sampler2D DerezedPassFeedback;
layout(set = 0, binding = 4) uniform sampler2D TextPass;


// vec4 GetCropOverlay(vec2 viewport_coord)
// {
//    vec4 out_color = vec4(0);
//    vec4 screen_color = vec4(1, 0, 0, 1);
   
//    for (int i=1; i < 3; i++)
//    {
//       vec2 cropped_rotated_size = vec2(0);
//       vec2 cropped_rotated_size_with_res_mult = vec2(0);
//       vec2 cropped_sample_area_start_pixel_coord = vec2(0);
//       HSM_GetCroppedRotatedSizeAndPixelSampleAreaStart(i, Source, cropped_rotated_size, cropped_rotated_size_with_res_mult, cropped_sample_area_start_pixel_coord);

//       vec2 crop_scale = cropped_rotated_size / ROTATED_CORE_PREPPED_SIZE;
//       vec2 mask_coord = viewport_coord - cropped_sample_area_start_pixel_coord / ROTATED_CORE_PREPPED_SIZE;
//       mask_coord = mask_coord / crop_scale;
      
//       if (i == 2)
//          screen_color = vec4(0, 1, 0, 1);

//       out_color += screen_color * HSM_GetCornerMask(mask_coord, cropped_rotated_size.x/cropped_rotated_size.y, 0, 0.90);
//    }

//    return clamp(out_color, 0, 1);
// }

void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);

	// Flip the coordinate vertically if desired
	vec2 viewport_coord_adjusted = vTexCoord;
	viewport_coord_adjusted.x = HSM_FLIP_VIEWPORT_HORIZONTAL * HSM_FLIP_CORE_HORIZONTAL * (viewport_coord_adjusted.x - 0.5) + 0.5;
	viewport_coord_adjusted.y = HSM_FLIP_VIEWPORT_VERTICAL * HSM_FLIP_CORE_VERTICAL * (viewport_coord_adjusted.y - 0.5) + 0.5;

	vec4 in_pass_color = texture(DerezedPass, viewport_coord_adjusted);
	vec4 in_pass_color_feedback = texture(DerezedPassFeedback, viewport_coord_adjusted);
	FragColor = in_pass_color;

   bool cache_info_changed = CACHE_INFO_CHANGED;
   bool show_update_indicator = cache_info_changed && ( HSM_CACHE_UPDATE_INDICATOR_MODE == 1 || HSM_CACHE_GRAPHICS_ON < 0.5 && HSM_CACHE_UPDATE_INDICATOR_MODE == 2);

	if (show_update_indicator)
	{
		float final_aspect = SCREEN_ASPECT;
		// vec2 corner_offset = vec2(MAX_NEGATIVE_CROP * 2 * 0.8, MAX_NEGATIVE_CROP * 2 * 0.8);
		vec2 corner_offset = vec2(0.1, 0.1);
		vec2 center_coord = vec2(1 - corner_offset.x * final_aspect, 1 - corner_offset.y);
		vec2 test_coord = vec2(1 - (1 - viewport_coord_adjusted.x) * final_aspect, viewport_coord_adjusted.y);
		float distance =  length(test_coord - center_coord);

		if (distance < 0.04)
		{
			float modulation = clamp(0.4 + abs(mod(global.FrameCount, 90) / 90 - 0.5) * 1.2, 0, 1);
			modulation = HHLP_EasePowerInOut(modulation, 2);
			FragColor.rgb *= 1 - modulation;
			FragColor.rgb += modulation * vec3(1, 0, 0);
		}
	}

	//////// Draw text to show resolutions ////////// 
	if (HSM_RESOLUTION_DEBUG_ON == 1)
	{
		vec2 ps = global.OutputSize.zw;
		vec4 text_rgba = vec4(0);
		text_rgba = texture(TextPass, vTexCoord);
		text_rgba.rgb *= vec3(1, 1, 0);
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(1, 0)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(0, 1)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(1, 1)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(-1, 1)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(-1, 0)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(0, -1)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(-1, -1)).a;
		text_rgba.a += texture(TextPass, vTexCoord + ps * vec2(1, -1)).a;
		text_rgba = clamp(text_rgba, 0, 1);
		FragColor.rgb = HSM_PreMultAlphaBlend(FragColor, text_rgba).rgb;
	}

	// // Put a 0 or 1 value in the alpha to indicate if the pixel has changed since last frame
	// vec3 pixel_diff = abs(in_pass_color.rgb - in_pass_color_feedback.rgb);
	// float pixel_changed = max(max(pixel_diff.r, pixel_diff.g), pixel_diff.b ) > 1 / (256.0 * 2.0) ? 1 : 0;
	// FragColor.a = pixel_changed;

	if (abs(vTexCoord.x - 0.8) < 0.1 && abs(vTexCoord.y - 0.3) < 0.1)
		FragColor = vec4(0, 1, 1, 1);

	return;
}