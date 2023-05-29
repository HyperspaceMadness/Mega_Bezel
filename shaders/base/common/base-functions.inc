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


#define PRECOMP_INDEX_BOTTOM 0
#define PRECOMP_INDEX_MIDDLE 1
#define PRECOMP_INDEX_UPPER 2
#define PRECOMP_INDEX_ADDITIVE 3

#define PRECOMP_INDEX_TUBE 0
#define PRECOMP_INDEX_GEL 1
#define PRECOMP_INDEX_AMBIENT_LIGHT_1 2
#define PRECOMP_INDEX_AMBIENT_LIGHT_2 3
#define PRECOMP_INDEX_STATIC_HIGHLIGHT 4


vec4 HSM_GetImagePrecompRange(float in_precomp_index)
{
	vec4 out_range = vec4(0.25, 0.25, 0.5, 0.5);
	if (in_precomp_index == PRECOMP_INDEX_BOTTOM)
		out_range = vec4(0, 0, 0.5, 0.5);

	if (in_precomp_index == PRECOMP_INDEX_MIDDLE)
		out_range = vec4(0.5, 0, 1, 0.5);

	if (in_precomp_index == PRECOMP_INDEX_UPPER)
		out_range = vec4(0, 0.5, 0.5, 1);

	if (in_precomp_index == PRECOMP_INDEX_ADDITIVE)
		out_range = vec4(0.5, 0.5, 1, 1);
	
	return out_range;
}

vec4 HSM_GetTubeFxPrecompRange(float in_precomp_index)
{
	vec4 out_range = vec4(0.25, 0.25, 0.5, 0.5);
	if (in_precomp_index == PRECOMP_INDEX_TUBE)
		out_range = vec4(0, 0, 0.5, 0.5);

	if (in_precomp_index == PRECOMP_INDEX_GEL)
		out_range = vec4(0.5, 0, 1, 0.5);

	if (in_precomp_index == PRECOMP_INDEX_AMBIENT_LIGHT_1)
		out_range = vec4(0, 0.5, 0.5, 1);

	if (in_precomp_index == PRECOMP_INDEX_AMBIENT_LIGHT_2)
		out_range = vec4(0.5, 0.5, 1, 1);

	if (in_precomp_index == PRECOMP_INDEX_STATIC_HIGHLIGHT)
		out_range = vec4(0.5, 0.5, 1, 1);

	return out_range;
}

vec4 HSM_GetPrecompRGBA(sampler2D in_sampler_2D, vec2 in_coord, float in_precomp_index)
{
	vec4 precomp_range = HSM_GetImagePrecompRange(in_precomp_index);
	vec2 sample_coord = precomp_range.xy + in_coord * vec2(precomp_range.z - precomp_range.x, precomp_range.w - precomp_range.y);
	return texture(in_sampler_2D, sample_coord);
}

float HSM_GetPrecompIndexFromCoord(vec2 in_coord)
{
	float out_index = -1;

	for (float i = 0; i < 4; i++)
	{
		if (HHLP_IsInsideRange(in_coord, HSM_GetImagePrecompRange(i)))
		{
			out_index = i;
			break;
		}
	} 

	return out_index;
}

// Returns 1 for vertical split, 2 for horizontal split
float HSM_GetCoreImageSplitDirection()
{
	float core_image_split_direction = 1;

	if (HSM_DUALSCREEN_CORE_IMAGE_SPLIT_MODE == 0)
	{
		if (HSM_DUALSCREEN_MODE == 1)
			core_image_split_direction = 1;
		if (HSM_DUALSCREEN_MODE == 2)
			core_image_split_direction = 2;
	}
	else
	{
		core_image_split_direction = HSM_DUALSCREEN_CORE_IMAGE_SPLIT_MODE;
	}
	return core_image_split_direction;
}

vec2 HSM_GetCoordWithPositionOffset(vec2 in_coord, vec2 position_offset)
{
	return in_coord - position_offset;
}

vec2 HSM_GetInverseScaledCoord(vec2 in_coord, vec2 in_scale)
{
	vec2 middle = vec2(0.49999, 0.49999);
	vec2 diff = in_coord.xy - middle;
	vec2 screen_inverse_scale = 1.0 / in_scale;
	vec2 scaled_coord = middle + diff * screen_inverse_scale;

	return scaled_coord;
}

vec2 HSM_GetVTexCoordWithArgs(vec2 in_coord, vec2 in_scale, vec2 position_offset)
{
	return HSM_GetInverseScaledCoord(HSM_GetCoordWithPositionOffset(in_coord, position_offset), in_scale);
}

vec2 HSM_GetCacheSampleRangeSize()
{
	vec2 cache_image_resolution = global.DerezedPassSize.xy;

	float sample_block_num_pixels = 3;
	vec2 pixel_size = 1 / cache_image_resolution;
	vec2 range_size = sample_block_num_pixels * pixel_size;

	return range_size;
}

bool HSM_IsOutsideCacheFullRange(vec2 in_coord)
{
	float max_samples = 8;
	vec4 cache_full_range = vec4(0);
	cache_full_range.zw = vec2(max_samples, max_samples) * HSM_GetCacheSampleRangeSize();
	vec2 test_coord = in_coord / cache_full_range.zw;
	return HHLP_IsOutsideCoordSpace(test_coord);
}

// Rows and Columns are 1 based
vec4 HSM_GetCacheSampleRange(vec2 in_cell)
{
	float row_index = in_cell.x;
	float column_index = in_cell.y;
	vec2 range_size = HSM_GetCacheSampleRangeSize();

	vec4 out_sample_range = vec4(0);
	out_sample_range.xy = vec2(column_index - 1, row_index - 1) * range_size;
	out_sample_range.zw = vec2(column_index, row_index) * range_size;

	return out_sample_range;
}

vec2 HSM_GetCacheSampleCoord(vec2 in_cell)
{
	vec4 sample_range = HSM_GetCacheSampleRange(in_cell);
	vec2 sample_coord = sample_range.xy + (sample_range.zw - sample_range.xy) / 2;
	return sample_coord;
}

vec2 HSM_GetViewportCoordWithFlip(vec2 viewport_coord)
{
	vec2 out_coord = viewport_coord;

	// out_coord.y = HSM_FLIP_VIEWPORT_VERTICAL * (out_coord.y - 0.5) + 0.5;
	// out_coord.x = HSM_FLIP_VIEWPORT_HORIZONTAL * (out_coord.x - 0.5) + 0.5;

	if (HSM_FLIP_VIEWPORT_VERTICAL == -1)
		out_coord.y = 1 - out_coord.y;
	
	if (HSM_FLIP_VIEWPORT_HORIZONTAL == -1)
		out_coord.x =  1 - out_coord.x;

	return out_coord;
}

vec2 HSM_GetViewportCoordWithZoomAndPan(vec2 viewport_coord)
{
	vec2 out_coord = HSM_GetViewportCoordWithFlip(viewport_coord);

	out_coord = (out_coord - 0.5) / HSM_VIEWPORT_ZOOM + 0.5;
	out_coord.x += HSM_VIEWPORT_POSITION_X;
	out_coord.y -= HSM_VIEWPORT_POSITION_Y; 

	return out_coord;
}

float HSM_GetScreenIndex(vec2 viewport_coord)
{
	float out_index = 1;
	float output_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;

	if (HSM_DUALSCREEN_MODE == 0)
		out_index = 1;
	if (HSM_DUALSCREEN_MODE == 1)
		out_index = (viewport_coord.y < 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION / output_aspect) ? 1 : 2;
	if (HSM_DUALSCREEN_MODE == 2)
		out_index = (viewport_coord.x < 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION / output_aspect) ? 1 : 2;

	return out_index;
}

void HSM_UpdateCacheInfoChanged(sampler2D in_cache_pass)
{
	vec2 sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CACHE_CHANGED);
	vec4 texture_sample = texture(in_cache_pass, sample_coord);
	CACHE_INFO_CHANGED = texture_sample.r > 0.5 ? true : false;
}

vec4 HSM_UpdateGlobalScreenValuesFromCache(sampler2D in_cache_pass, vec2 vTexCoord)
{
	// TODO need to set a value to tell us if this has been updated this frame
	
	float output_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;
	vec2 flipped_viewport_coord = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);
	SCREEN_INDEX = HSM_GetScreenIndex(flipped_viewport_coord);
	vec2 sample_coord = vec2(0);
	vec4 texture_sample = vec4(0);

	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_AVERAGE_LUMA);
	texture_sample = texture(in_cache_pass, sample_coord);
	AVERAGE_LUMA = texture_sample.a;
	SAMPLING_SCANLINE_DIR_MULT = texture_sample.r;
	SAMPLING_OPPOSITE_DIR_MULT = texture_sample.g;

	float res_mult_size_sum = 0;
	float res_mult_size2_sum = 0;

	if (SCREEN_INDEX == 1)
	{
		// r SCREEN_ASPECT
		// ba SCREEN_SCALE
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_SCREEN_SCALE);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_ASPECT = texture_sample.r;
		SCREEN_SCALE = texture_sample.ba;

		// rg TUBE_SCALE 
		// ba SCREEN_POS_OFFSET
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_SCALE);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_SCALE = texture_sample.rg;
		SCREEN_POS_OFFSET = texture_sample.ba;

		// rg TUBE_DIFFUSE_SCALE 
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_DIFFUSE_SCALE);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_DIFFUSE_SCALE = texture_sample.rg;
		TUBE_DIFFUSE_ASPECT = TUBE_DIFFUSE_SCALE.x / TUBE_DIFFUSE_SCALE.y * output_aspect;

		// rg CROPPED_ROTATED_SIZE_WITH_RES_MULT
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CROPPED_ROTATED_SIZE_WITH_RES_MULT);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_SIZE_WITH_RES_MULT = texture_sample.rg;
		res_mult_size_sum = CROPPED_ROTATED_SIZE_WITH_RES_MULT.x + CROPPED_ROTATED_SIZE_WITH_RES_MULT.y;
		ROTATED_CORE_PREPPED_SIZE = texture_sample.ba;

		// rg CROPPED_ROTATED_SIZE
		// ba SAMPLE_AREA_START_PIXEL_COORD
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CROPPED_ROTATED_SIZE);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_SIZE = texture_sample.rg;
		SAMPLE_AREA_START_PIXEL_COORD = texture_sample.ba;

		// rg screen size first screen
		// ba screen size second screen
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_SCREEN_SIZE);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_SIZE = texture_sample.rg;
	}
	// If we are in the section of the viewport which is the second screen
	if (SCREEN_INDEX == 2)
	{
		// 2nd Screen
		// r SCREEN_ASPECT
		// ba SCREEN_SCALE
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_SCREEN_SCALE_2ND_SCREEN);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_ASPECT = texture_sample.r;
		SCREEN_SCALE = texture_sample.gb;

		// 2nd Screen
		// rg TUBE_SCALE
		// ba SCREEN_POS_OFFSET
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_SCALE_2ND_SCREEN);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_SCALE = 		texture_sample.rg;
		SCREEN_POS_OFFSET = texture_sample.ba;

		// TODO need to add TUBE_DIFFUSE_ASPECT & deal with 2nd Screen
		// ba TUBE_DIFFUSE_SCALE 
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_DIFFUSE_SCALE);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_DIFFUSE_SCALE = texture_sample.ba;
		TUBE_DIFFUSE_ASPECT = TUBE_DIFFUSE_SCALE.x / TUBE_DIFFUSE_SCALE.y * output_aspect;

		// rg CROPPED_ROTATED_SIZE_WITH_RES_MULT
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CROPPED_ROTATED_SIZE_WITH_RES_MULT_2ND_SCREEN);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_SIZE_WITH_RES_MULT = texture_sample.rg;
		res_mult_size2_sum = CROPPED_ROTATED_SIZE_WITH_RES_MULT.x + CROPPED_ROTATED_SIZE_WITH_RES_MULT.y;

		// rg CROPPED_ROTATED_SIZE
		// ba SAMPLE_AREA_START_PIXEL_COORD
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CROPPED_ROTATED_SIZE_2ND_SCREEN);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_SIZE = texture_sample.rg;
		SAMPLE_AREA_START_PIXEL_COORD = texture_sample.ba;

		// rg screen size first screen
		// ba screen size second screen
		sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_SCREEN_SIZE);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_SIZE = texture_sample.ba;
	}

	// rg TUBE_SCALE 
	// ba SCREEN_POS_OFFSET
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_SCALE);
	texture_sample = texture(in_cache_pass, sample_coord);
	// TUBE_SCALE_1ST_SCREEN = texture_sample.rg;
	SCREEN_POS_OFFSET_1ST_SCREEN = texture_sample.ba;

	// rg TUBE_DIFFUSE_SCALE_1ST_SCREEN 
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_DIFFUSE_SCALE);
	texture_sample = texture(in_cache_pass, sample_coord);
	TUBE_DIFFUSE_SCALE_1ST_SCREEN = texture_sample.rg;
	// TUBE_DIFFUSE_ASPECT_1ST_SCREEN = TUBE_DIFFUSE_SCALE_1ST_SCREEN.x / TUBE_DIFFUSE_SCALE_1ST_SCREEN.y * output_aspect;

	// 2nd Screen
	// rg TUBE_SCALE_2ND_SCREEN
	// ba SCREEN_POS_OFFSET_2ND_SCREEN
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_SCALE_2ND_SCREEN);
	texture_sample = texture(in_cache_pass, sample_coord);
	// TUBE_SCALE_2ND_SCREEN = 		texture_sample.rg;
	SCREEN_POS_OFFSET_2ND_SCREEN = texture_sample.ba;

	// TODO need to add TUBE_DIFFUSE_ASPECT & deal with 2nd Screen
	// ba TUBE_DIFFUSE_SCALE_2ND_SCREEN 
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_TUBE_DIFFUSE_SCALE);
	texture_sample = texture(in_cache_pass, sample_coord);
	TUBE_DIFFUSE_SCALE_2ND_SCREEN = texture_sample.ba;

	// rg CORE_SIZE
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CORE_SIZE);
	texture_sample = texture(in_cache_pass, sample_coord);
	CORE_SIZE = texture_sample.rg;
	ROTATED_CORE_ORIGINAL_SIZE = texture_sample.ba;

	// rg VIEWPORT_SCALE
	// ba VIEWPORT_POS
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_VIEWPORT_SCALE);
	texture_sample = texture(in_cache_pass, sample_coord);
	VIEWPORT_SCALE = texture_sample.rg;
	VIEWPORT_POS = texture_sample.ba;

	// rg SCREEN_SCALE_2ND_SCREEN
	// ba SCREEN_POS_OFFSET_2ND_SCREEN
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_VIEWPORT_SCALE_2ND_SCREEN);
	texture_sample = texture(in_cache_pass, sample_coord);
	SCREEN_SCALE_2ND_SCREEN = texture_sample.rg;
	SCREEN_POS_OFFSET_2ND_SCREEN = texture_sample.ba;

	// Sample 1, 4
	// g CURRENT_FRAME_FROM_CACHE_INFO
	// b ROTATED_DEREZED_SIZE
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_PARAMETER_SUM);
	texture_sample = texture(in_cache_pass, sample_coord);
	CURRENT_FRAME_FROM_CACHE_INFO = texture_sample.g;
	ROTATED_DEREZED_SIZE = texture_sample.ba;

	// Sample 2, 4 
	// r NEGATIVE_CROP_EXPAND_MULTIPLIER
	// g MAX_NEGATIVE_CROP
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_VERTICAL_SCANLINES);
	texture_sample = texture(in_cache_pass, sample_coord);
	NEGATIVE_CROP_EXPAND_MULTIPLIER = texture_sample.r;
	MAX_NEGATIVE_CROP = texture_sample.g;
	USE_VERTICAL_SCANLINES = texture_sample.b;

	// Sample 8, 8
	// r CACHE_INFO_CHANGED
	sample_coord = HSM_GetCacheSampleCoord(SAMPLE_CELL_CACHE_CHANGED);
	texture_sample = texture(in_cache_pass, sample_coord);
	CACHE_INFO_CHANGED = texture_sample.r > 0.5 ? true : false;

	SCREEN_SCALE_WITH_ZOOM = SCREEN_SCALE * HSM_VIEWPORT_ZOOM;
	SCREEN_COORD = HSM_GetVTexCoordWithArgs(flipped_viewport_coord, SCREEN_SCALE, SCREEN_POS_OFFSET);
	TUBE_DIFFUSE_COORD = HSM_GetVTexCoordWithArgs(flipped_viewport_coord, TUBE_DIFFUSE_SCALE, SCREEN_POS_OFFSET);
	TUBE_DIFFUSE_COORD_MIXED_POS = HSM_GetVTexCoordWithArgs(flipped_viewport_coord, TUBE_DIFFUSE_SCALE_1ST_SCREEN, (SCREEN_POS_OFFSET_1ST_SCREEN + SCREEN_POS_OFFSET_2ND_SCREEN) / 2.0);

	return vec4(0);
}