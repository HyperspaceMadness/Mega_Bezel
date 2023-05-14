#version 450

#include "common/globals-and-screen-scale-params.inc"
#include "common/params-5-intro.inc"

/*

*/

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
layout(set = 0, binding = 2) uniform sampler2D Source;

#ifndef IS_POTATO_PRESET
layout(set = 0, binding = 3) uniform sampler2D OriginalHistory1;
layout(set = 0, binding = 4) uniform sampler2D OriginalHistory2;
layout(set = 0, binding = 5) uniform sampler2D OriginalHistory3;
layout(set = 0, binding = 7) uniform sampler2D IntroImage;
#endif

layout(set = 0, binding = 8) uniform sampler2D CacheInfoPass;

layout(set = 0, binding = 10) uniform sampler2D TextPass;

#define eps 1e-3

#ifndef IS_POTATO_PRESET
vec4 AntiFlicker(vec2 in_coord, vec4 current)
{
	// sample the textures
	vec4 prev1 = texture(OriginalHistory1, in_coord);
	vec4 prev2 = texture(OriginalHistory2, in_coord);
	vec4 prev3 = texture(OriginalHistory3, in_coord);
	
	// get luma for comparison
	float cur_lum = dot(current.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev1_lum = dot(prev1.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev2_lum = dot(prev2.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev3_lum = dot(prev3.rgb, vec3(0.2125, 0.7154, 0.0721));
	
	// Test whether the luma difference between the pixel in the current frame and that of
	// the previous frame exceeds the threshold while the difference between the current frame
	// and 2 frames previous is below the threshold.
	// Repeat the process for the previous frame's pixel to reduce false-positives
	bool flicker = (abs(cur_lum - prev1_lum) > HSM_ANTI_FLICKER_THRESHOLD && abs(cur_lum - prev2_lum) < HSM_ANTI_FLICKER_THRESHOLD) &&
		(abs(prev1_lum - prev2_lum) > HSM_ANTI_FLICKER_THRESHOLD && abs(prev1_lum - prev3_lum) < HSM_ANTI_FLICKER_THRESHOLD);
	
	// Average the current frame with the previous frame in linear color space to avoid over-darkening
	vec4 blended = (pow(current, vec4(2.2)) + pow(prev1, vec4(2.2))) / 2.0;
	
	// delinearize the averaged result
	blended = pow(blended, vec4(1.0 / 2.2));
	
	return (!flicker) ? current : blended;
}

void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);

	// Flip the coordinate vertically if desired
   vec2 viewport_coord_adjusted = vTexCoord;
	viewport_coord_adjusted.x = HSM_FLIP_VIEWPORT_HORIZONTAL * HSM_FLIP_CORE_HORIZONTAL * (viewport_coord_adjusted.x - 0.5) + 0.5;
	viewport_coord_adjusted.y = HSM_FLIP_VIEWPORT_VERTICAL * HSM_FLIP_CORE_VERTICAL * (viewport_coord_adjusted.y - 0.5) + 0.5;

   FragColor = texture(Source, viewport_coord_adjusted);

   if (HSM_ANTI_FLICKER_ON == 1)
      FragColor = AntiFlicker(viewport_coord_adjusted, FragColor);

   bool cache_info_changed = CACHE_INFO_CHANGED;
   bool show_update_indicator = cache_info_changed && ( HSM_CACHE_UPDATE_INDICATOR_MODE == 1 || HSM_CACHE_GRAPHICS_ON < 0.5 && HSM_CACHE_UPDATE_INDICATOR_MODE == 2);
}

