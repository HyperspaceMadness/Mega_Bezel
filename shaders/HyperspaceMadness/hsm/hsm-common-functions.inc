/*   
hsm-screen-scale-params-functions

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL
Description:
	-Parameters to be used by Mega Screen Scale and Mega Bezel
*/

#include "hsm-helper-functions.inc"

#ifndef IS_POTATO_PRESET
#include "hsm-royale-geometry-functions.inc"
#endif

// vec2 HSM_GetViewportCoordWithZoomAndPan(vec2 viewport_coord)
// {
// 	vec2 out_coord = viewport_coord;
// 	out_coord.x += HSM_VIEWPORT_POSITION_X; 
// 	out_coord.y -= HSM_VIEWPORT_POSITION_Y; 
// 	out_coord = (out_coord - 0.5) / HSM_VIEWPORT_ZOOM + 0.5;
// 	return out_coord;
// }

float HSM_GetAspectRatioFromMode(float in_aspect_ratio_mode, float in_explicit_aspect)
{
	float out_explicit_aspect = in_explicit_aspect;

	if (in_aspect_ratio_mode == TEXTURE_ASPECT_MODE_VIEWPORT)
		out_explicit_aspect = global.OutputSize.x / global.OutputSize.y;
	if (in_aspect_ratio_mode == TEXTURE_ASPECT_MODE_4_3)
		out_explicit_aspect = 1.33333;
	if (in_aspect_ratio_mode == TEXTURE_ASPECT_MODE_3_4)
		out_explicit_aspect = 0.75;
	if (in_aspect_ratio_mode == TEXTURE_ASPECT_MODE_16_9)
		out_explicit_aspect = 1.7777;
	if (in_aspect_ratio_mode == TEXTURE_ASPECT_MODE_9_16)
		out_explicit_aspect = 0.5625;

	return out_explicit_aspect;
}

vec2 HSM_GetRotatedCoreOriginalSize()
{
	return HSM_ROTATE_CORE_IMAGE * global.OriginalSize.yx + (1 - HSM_ROTATE_CORE_IMAGE) * global.OriginalSize.xy;
}

vec2 HSM_GetDerezedSize()
{
	return global.DerezedPassSize.xy;
}

vec2 HSM_GetRotatedDerezedSize()
{
	return HSM_ROTATE_CORE_IMAGE * global.DerezedPassSize.yx + (1 - HSM_ROTATE_CORE_IMAGE) * global.DerezedPassSize.xy;
}

// Returns 1 for vertical split, 2 for horizontal split
float HSM_Get_Core_Image_Split_Direction()
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

float HSM_GetSwappedScreenIndex(float screen_index)
{
	float out_index = screen_index;

	if (HSM_DUALSCREEN_CORE_IMAGE_SWAP_SCREENS == 1)
	{
		if (screen_index == 1)
		{
			out_index = 2;
		}
		else
		{
			out_index = 1;
		}
	}

	return out_index;
}

vec2 HSM_GetCroppedSize(vec2 in_size, float screen_index){
	vec2 out_size = in_size;
	screen_index = HSM_GetSwappedScreenIndex(screen_index);
	float dual_screen_sides_crop = floor(out_size.x * HSM_DUALSCREEN_2ND_SCREEN_SIDES_CROP);

	if (HSM_DUALSCREEN_MODE > 0)
		if (HSM_Get_Core_Image_Split_Direction() == 1)
		{
			if (screen_index == 1)
				out_size.y += HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET;
			else
			{
				out_size.y -= HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET;
				out_size.x -= 2 * dual_screen_sides_crop;
			}
		}
		else
		{
			if (screen_index == 1)
				out_size.x += HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET;
			else
			{
				out_size.x -= HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET;
				out_size.y -= 2 * dual_screen_sides_crop;
			}
		}

	if (screen_index == 1)
		out_size -= vec2(floor(out_size.x * HSM_CROP_PERCENT_RIGHT) + floor(out_size.x * HSM_CROP_PERCENT_LEFT), floor(out_size.y * HSM_CROP_PERCENT_TOP) + floor(out_size.y * HSM_CROP_PERCENT_BOTTOM));
	return out_size;
}

vec2 HSM_GetSampleAreaStartPixelCoord(float screen_index)
{
	screen_index = HSM_GetSwappedScreenIndex(screen_index);
	vec2 raw_original_size = HSM_GetDerezedSize();
	vec2 sample_start_coord = vec2(0);
	float dual_screen_sides_crop = floor(raw_original_size.x * HSM_DUALSCREEN_2ND_SCREEN_SIDES_CROP);

	if (screen_index == 2)
	{
		if (HSM_Get_Core_Image_Split_Direction() == 1)
		{
			sample_start_coord = vec2(0, raw_original_size.y / 2 + HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET);
			sample_start_coord.x += dual_screen_sides_crop;
		}
		else
		{
			sample_start_coord = vec2(raw_original_size.x / 2 + HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET, 0);
			sample_start_coord.y += dual_screen_sides_crop;
		}
	}

	return sample_start_coord;
}

// Get the original size with split area added
vec2 HSM_GetScreenDerezedSize(float screen_index)
{
	vec2 original_size = global.DerezedPassSize.xy;

	if (HSM_DUALSCREEN_MODE > 0)
		if (HSM_Get_Core_Image_Split_Direction() == 1)
			original_size.y *= 0.5;
		else
			original_size.x *= 0.5;

	return original_size;
}

vec2 HSM_GetRotatedScreenDerezedSize(float screen_index)
{
	vec2 original_size = HSM_GetScreenDerezedSize(screen_index);
	return HSM_ROTATE_CORE_IMAGE * original_size.yx + (1 - HSM_ROTATE_CORE_IMAGE) * original_size.xy;
}

vec2 HSM_GetCroppedRotatedDerezedSize(float screen_index)
{
	return HSM_GetCroppedSize(HSM_GetRotatedScreenDerezedSize(screen_index), screen_index);
}

float HSM_GetIsDerezedSizeVertical(float screen_index)
{
	vec2 rotated_original_size = HSM_GetRotatedScreenDerezedSize(screen_index);
	float aspect_ratio = rotated_original_size.x / rotated_original_size.y;
	return aspect_ratio < 1 ? 1 : 0;
}

bool HSM_ResolutionIsEqual(vec2 in_res, vec2 match_res)
{
	return (in_res == match_res);
}

float HSM_GetScreenAspect(float screen_index)
{
	float epsilon = 0.002;
	vec2 original_size = HSM_GetScreenDerezedSize(screen_index);
	vec2 rotated_original_size = HSM_GetCroppedRotatedDerezedSize(screen_index);
	float core_aspect_ratio = rotated_original_size.x / rotated_original_size.y;
	float core_aspect_horizontal = (core_aspect_ratio < 1) ? 1 / core_aspect_ratio : core_aspect_ratio;
	float horizontal_aspect = 0;

	vec2 sony_psp = 						vec2(480, 272);
	vec2 nintendo_gameboy_advance_res = 	vec2(240, 160);
	vec2 nintendo_gameboy_res = 			vec2(160, 144);
	vec2 nintendo_ds_res = 					vec2(256, 192);
	vec2 nintendo_ds_top_bottom_res = 		vec2(256, 384);
	vec2 nintendo_ds_side_by_side_res = 	vec2(512, 192);
	vec2 nintendo_3ds_top_res = 			vec2(400, 240);
	vec2 nintendo_3ds_bottom_res = 			vec2(320, 240);
	vec2 nintendo_3ds_top_bottom_res = 		vec2(400, 480);
	vec2 nintendo_3ds_side_by_side_res = 	vec2(720, 240);
	vec2 atari_lynx_res = 					vec2(160, 102);
	vec2 sega_saturn_fmv_res = 				vec2(352, 480);
	vec2 atari_2600_res = 					vec2(160, 228);
	vec2 atari_2600_crop_res = 				vec2(152, 228);
	
	if (HSM_ASPECT_RATIO_MODE == 0)
	{
		// If the vertical res is larger than 580 is is probably a modern square pixel resolution
		// 576 seems to be PAL vertical resolution used sometimes
		if (original_size.y > 580) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(sony_psp, 						original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_gameboy_advance_res, 	original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_gameboy_res, 			original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_ds_res, 				original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_ds_top_bottom_res, 		original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_ds_side_by_side_res, 	original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_3ds_top_res, 			original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_3ds_bottom_res, 		original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_3ds_top_bottom_res, 	original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(nintendo_3ds_side_by_side_res, 	original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(atari_lynx_res, 					original_size)) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ResolutionIsEqual(atari_2600_res, 					original_size)) horizontal_aspect = 1.333;
		else if (HSM_ResolutionIsEqual(atari_2600_crop_res, 			original_size)) horizontal_aspect = 1.333;
		// Fall back to the explicit ratio
		else horizontal_aspect = HSM_ASPECT_RATIO_EXPLICIT;
	}
	else 
		if (HSM_ASPECT_RATIO_MODE == 1) horizontal_aspect = HSM_ASPECT_RATIO_EXPLICIT;
		else if (HSM_ASPECT_RATIO_MODE == 2) horizontal_aspect = 1.3333;
		else if (HSM_ASPECT_RATIO_MODE == 3) horizontal_aspect = 1.5;
		else if (HSM_ASPECT_RATIO_MODE == 4) horizontal_aspect = 1.7777;
		else if (HSM_ASPECT_RATIO_MODE == 5) horizontal_aspect = core_aspect_horizontal;
		else if (HSM_ASPECT_RATIO_MODE == 6) horizontal_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;
		else horizontal_aspect = 1.333;

	// Find what the vertical aspect would be, either the current horizontal_aspect (if it's already vertical)
	// Or changing the horizontal aspect to vertical by taking the reciprocal
	float vertical_aspect = 1 / horizontal_aspect;
	float final_orientation = HSM_ASPECT_RATIO_ORIENTATION;

	if (HSM_ASPECT_RATIO_ORIENTATION < 0.5)
	{
		// Catch for Atari 2600 - Stella Emulator which would otherwise show up as a vertical aspect ratio
		if (
				HSM_ResolutionIsEqual(atari_2600_res, original_size) || 
		   		HSM_ResolutionIsEqual(atari_2600_crop_res, original_size) ||
		   		HSM_ResolutionIsEqual(sega_saturn_fmv_res, original_size)
		   ) 
		{
		   final_orientation = 1;
		}
		else
		{
			final_orientation = (HSM_GetIsDerezedSizeVertical(screen_index) > 0.5) ? 2 : 1;
		}
	}
	
	float final_aspect_ratio = (final_orientation < 1.5) ? horizontal_aspect : vertical_aspect;

	return final_aspect_ratio;
}

float HSM_GetFinalViewportAspectRatio()
{
	return global.FinalViewportSize.x / global.FinalViewportSize.y;
}

float HSM_GetUseVerticalScanlines(float screen_aspect)
{
	float auto_use_vert_scanlines = screen_aspect < 1 ? 1 : 0;

	float scanline_direction = HSM_SCANLINE_DIRECTION < 2 ? 0 : 1;
	
	if (HSM_SCANLINE_DIRECTION < 1)
		if (HSM_DUALSCREEN_MODE < 1) 
			scanline_direction = auto_use_vert_scanlines;

	return scanline_direction;
}

bool HSM_GetIsInterlaceModeActive()
{
	float scan_res = USE_VERTICAL_SCANLINES > 0.5 ? CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT.x : CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT.y;
	return ( HSM_INTERLACE_MODE > 0.5 && HSM_INTERLACE_TRIGGER_RES < scan_res );
}


vec2 HSM_GetCoreSamplingResMult()
{
	// TODO when using target resolution
	// float epsilon = 0.25;
	// float original_res = 1464;
	// float target_res
	// float multiple = 1464 / 224;
	// float int_multiple = floor(multiple + epsilon);
	// float int_div_res_near_target_res = original_res / int_multiple;

	vec2 original_size = HSM_GetRotatedDerezedSize();
	float use_vert_scanlines = HSM_GetUseVerticalScanlines(original_size.x/original_size.y);
	vec2 sampling_mult = (1 - use_vert_scanlines) * vec2(HSM_CORE_RES_SAMPLING_MULT_SCANLINE_DIR, HSM_CORE_RES_SAMPLING_MULT_OPPOSITE_DIR) + 
							  use_vert_scanlines *  vec2(HSM_CORE_RES_SAMPLING_MULT_OPPOSITE_DIR, HSM_CORE_RES_SAMPLING_MULT_SCANLINE_DIR);
	return sampling_mult;
}

vec2 HSM_GetRotatedOriginalSizeWithCoreResMult(float screen_index)
{
	vec2 original_size = HSM_GetScreenDerezedSize(screen_index);
	vec2 sampling_mult = HSM_GetCoreSamplingResMult();

	vec2 sampling_size = vec2(floor(sampling_mult.x * original_size.x), floor(sampling_mult.y * original_size.y));

	return HSM_ROTATE_CORE_IMAGE * sampling_size.yx + (1 - HSM_ROTATE_CORE_IMAGE) * sampling_size.xy;
}

// TODO need to have aspect adjusted when in dual screen
vec2 HSM_GetCroppedRotatedOriginalSizeWithCoreResMult(float screen_index)
{
	vec2 cropped_original_size = HSM_GetCroppedSize(HSM_GetRotatedOriginalSizeWithCoreResMult(screen_index), screen_index);

	return cropped_original_size;
}

float HSM_GetScreenIndex(vec2 viewport_coord)
{
	float out_index = 1;

	if (HSM_DUALSCREEN_MODE == 0)
		out_index = 1;
	if (HSM_DUALSCREEN_MODE == 1)
		out_index = (viewport_coord.y < 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION) ? 1 : 2;
	if (HSM_DUALSCREEN_MODE == 2)
		out_index = (viewport_coord.x < 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION) ? 1 : 2;

	return out_index;
}

// HSM_SNAP_TO_INT_SCALE_TOLERANCE
vec2 HSM_GetScreenScale(float screen_aspect, float screen_height_from_image)
{
	if (HSM_ASPECT_RATIO_MODE > 5.5)
	{
		return vec2(1, 1);
	}
	else
	{
		float output_aspect_ratio = HSM_GetFinalViewportAspectRatio();

		// Get Original size with cropping applied as this affects the integer scale
		vec2 cropped_original_size = HSM_GetCroppedRotatedDerezedSize(1);

		bool viewport_is_vertical = (global.FinalViewportSize.x < global.FinalViewportSize.y);

		// If we are using non-integer scale return
		float screen_height = HSM_NON_INTEGER_SCALE / 100;

		if (HSM_DUALSCREEN_MODE > 0)
			screen_height *= 0.5;

		if (HSM_INT_SCALE_MODE == 0)
		{
			if (viewport_is_vertical || screen_aspect > output_aspect_ratio)
			{
				float diagonal_length = screen_height * output_aspect_ratio;
				screen_height = (diagonal_length * 1) / sqrt(screen_aspect * screen_aspect + 1 * 1);
			}

			if (USE_IMAGE_FOR_PLACEMENT == 1)
				screen_height = screen_height_from_image;

			screen_height *= HSM_NON_INTEGER_SCALE_OFFSET;

			// If the integer tolerance is greater than zero see if we can snap to the nearest integer multiple
			if (HSM_SNAP_TO_INT_SCALE_TOLERANCE > 0)
			{
				float integer_scale_multiple_vert = screen_height * global.FinalViewportSize.y / cropped_original_size.y;
				float int_scale_remainder = fract(integer_scale_multiple_vert);
				int_scale_remainder = (int_scale_remainder < 1 - int_scale_remainder) ? int_scale_remainder : 1 - int_scale_remainder;
				float remainder_percent_of_screen_height = (int_scale_remainder * cropped_original_size.y) / (screen_height * global.FinalViewportSize.y);
				if (remainder_percent_of_screen_height < HSM_SNAP_TO_INT_SCALE_TOLERANCE)
				{
					integer_scale_multiple_vert = round(integer_scale_multiple_vert);
					screen_height = integer_scale_multiple_vert * cropped_original_size.y / global.FinalViewportSize.y;
				}
			}

			return vec2(screen_aspect / output_aspect_ratio, 1) * screen_height;
		}

		// Get the maximum height that the integer scale needs to fit into
		float viewport_res_y_without_border = global.FinalViewportSize.y - 2 * (HSM_INT_SCALE_BORDER_MIN_HEIGHT / 100) * global.FinalViewportSize.y;
		float viewport_res_x_without_border = global.FinalViewportSize.x - 2 * (HSM_INT_SCALE_BORDER_MIN_HEIGHT / 100) * global.FinalViewportSize.x;

		if (HSM_DUALSCREEN_MODE == 1)
			viewport_res_y_without_border = global.FinalViewportSize.y / 2 - 0.5 * (HSM_INT_SCALE_BORDER_MIN_HEIGHT / 100) * global.FinalViewportSize.y;

		if (HSM_DUALSCREEN_MODE == 2)
			viewport_res_x_without_border *= global.FinalViewportSize.x / 2 - 0.5 * (HSM_INT_SCALE_BORDER_MIN_HEIGHT / 100) * global.FinalViewportSize.x;

		// If the viewport is taller than it is wide then get the height from the corresponding available width
		if (viewport_is_vertical) viewport_res_y_without_border = viewport_res_x_without_border / screen_aspect;

		// If the screen is too high
		if ((viewport_res_y_without_border * screen_aspect) > global.FinalViewportSize.x)
		{
			viewport_res_y_without_border = (1 - 2 * HSM_INT_SCALE_BORDER_MIN_HEIGHT / 100) * global.FinalViewportSize.x / screen_aspect; 
		}

		float integer_scale_multiple_vert = clamp(floor(viewport_res_y_without_border / cropped_original_size.y) + HSM_INT_SCALE_MULTIPLE_OFFSET, 1, 100);
		float integer_scale_vert = integer_scale_multiple_vert * cropped_original_size.y / global.FinalViewportSize.y;

		// Get the horizontal scale from the vertical scale and aspect ratio
		float integer_scale_horz_from_aspect = screen_aspect / output_aspect_ratio * integer_scale_vert;

		// Get the scale as a multiple of the original x-size
		float integer_scale_multiple_horz = integer_scale_horz_from_aspect * global.FinalViewportSize.x / cropped_original_size.x;

		// If we are using vertical scanlines or integer scale is set to both directions make the horizontal multiple an integer
		float final_int_scale_mode = HSM_INT_SCALE_MODE;
		if (HSM_INT_SCALE_MODE > 0.5)
		{
			if (HSM_GetUseVerticalScanlines(screen_aspect) == 1 || HSM_INT_SCALE_MODE == 2)
			{
				integer_scale_multiple_horz = round(integer_scale_multiple_horz);
				final_int_scale_mode = 2;
			}
		}

		float both_axes = clamp((final_int_scale_mode - 1) * 10000, 0, 1);
		integer_scale_multiple_vert += both_axes * abs(clamp((screen_aspect - 1) * 10000, -1, 0)) * HSM_INT_SCALE_MULTIPLE_OFFSET_LONG;
		integer_scale_multiple_horz += both_axes * abs(clamp((screen_aspect - 1) * 10000,  0, 1)) * HSM_INT_SCALE_MULTIPLE_OFFSET_LONG;

		integer_scale_vert = integer_scale_multiple_vert * cropped_original_size.y / global.FinalViewportSize.y;
		float integer_scale_horz = integer_scale_multiple_horz * cropped_original_size.x / global.FinalViewportSize.x;

		return vec2(integer_scale_horz, integer_scale_vert);
	}
}

vec2 HSM_GetScreenScaleFor2ndScreen(vec2 screen_scale, float screen_aspect)
{
	vec2 out_screen_scale = screen_scale;
	
	if (HSM_DUALSCREEN_2ND_SCREEN_ASPECT_RATIO_MODE == 1)
	{
		float output_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;
		out_screen_scale = vec2(screen_scale.y * screen_aspect / output_aspect, screen_scale.y);
	}

	if (HSM_DUALSCREEN_2ND_SCREEN_INDEPENDENT_SCALE == 1)
		out_screen_scale = (out_screen_scale / out_screen_scale.y) * DEFAULT_UNCORRECTED_SCREEN_SCALE.y * 0.5;

	out_screen_scale *= HSM_DUALSCREEN_2ND_SCREEN_SCALE_OFFSET;

	return out_screen_scale;
}

vec2 HSM_GetScreenSize(vec2 screen_scale)
{
	vec2 screen_size = global.FinalViewportSize.xy * screen_scale;
	// Round this so we get proper integers
	screen_size = vec2(ceil(screen_size.x), ceil(screen_size.y));
	return screen_size;
}

// Returns Diagonal Screen scale relative to the viewport height
float HSM_GetScreenDiagonalScale(vec2 screen_scale)
{
	float screen_scale_x_compensated = screen_scale.x / (global.FinalViewportSize.x/global.FinalViewportSize.y);
	return sqrt(screen_scale_x_compensated * screen_scale_x_compensated + screen_scale.y * screen_scale.y);
}

vec2 HSM_GetIntegerScaleMult(vec2 screen_size, float screen_index)
{
	float epsilon = 0.002;
	vec2 integer_scale = screen_size / HSM_GetCroppedRotatedDerezedSize(screen_index);
	integer_scale = vec2(fract(integer_scale.x + 0.001) < epsilon ? round(integer_scale.x) : integer_scale.x,
						 fract(integer_scale.y + 0.001) < epsilon ? round(integer_scale.y) : integer_scale.y);
	return integer_scale;
}

vec2 HSM_GetScreenScaleWithEdgeHeight(float edge_height, vec2 screen_scale)
// Edge Height is a 0 to 1 percentage of the screen height
{
	float output_aspect_ratio = global.FinalViewportSize.x / global.FinalViewportSize.y;
	
	float aspect_ratio = screen_scale.x / screen_scale.y;

	vec2 edge_width_height_as_scale = vec2(0, 0);
	edge_width_height_as_scale.x = 1 + (edge_height / screen_scale.y) / aspect_ratio / output_aspect_ratio;
	edge_width_height_as_scale.y = 1 + edge_height / screen_scale.y;

	return screen_scale * edge_width_height_as_scale;
}

vec2 HSM_GetInverseScaledCoord(vec2 in_coord, vec2 in_scale)
{
	vec2 middle = vec2(0.49999, 0.49999);
	vec2 diff = in_coord.xy - middle;
	vec2 screen_inverse_scale = 1.0 / in_scale;
	vec2 scaled_coord = middle + diff * screen_inverse_scale;

	return scaled_coord;
}

// Same as the HSM_GetScreenScale, but adds the width of the black edge
// Used for scaling the overlay outside the black edge
vec2 HSM_GetTubeScale(vec2 screen_scale, float image_placement_screen_height)
{
	// Add switch for independent scale
	if (HSM_BZL_USE_INDEPENDENT_SCALE == 1)
	{
		if (USE_IMAGE_FOR_PLACEMENT == 0)
			screen_scale = screen_scale / screen_scale.y * HSM_BZL_INDEPENDENT_SCALE;
		else
			screen_scale = screen_scale / screen_scale.y * image_placement_screen_height;
	}

	float normalized_screen_height = screen_scale.y / DEFAULT_UNCORRECTED_SCREEN_SCALE.y;
	return HSM_GetScreenScaleWithEdgeHeight(1.2 * HSM_TUBE_BLACK_EDGE_THICKNESS / 100.0 * normalized_screen_height, screen_scale * HSM_BZL_SCALE_OFFSET);
}

vec2 HSM_GetPositionOffset(vec2 placement_image_pos, vec2 screen_scale )
{
	vec2 screen_center_pos = vec2(0.5, 0.5);
	
	if (HSM_DUALSCREEN_MODE == 1)
	{
		if (HSM_DUALSCREEN_SHIFT_POSITION_WITH_SCALE == 1)
			screen_center_pos = vec2(0.5, 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION - screen_scale.y * 1.17 / 2);
		else
			screen_center_pos = vec2(0.5, 0.25);

		screen_center_pos.y -= HSM_DUALSCREEN_POSITION_OFFSET_BETWEEN_SCREENS;
	}

	if (HSM_DUALSCREEN_MODE == 2)
	{
		if (HSM_DUALSCREEN_SHIFT_POSITION_WITH_SCALE == 1)
			screen_center_pos = vec2(0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION - screen_scale.x * 1.17 / 2, 0.5);
		else
			screen_center_pos = vec2(0.25, 0.5);

		screen_center_pos.x -= HSM_DUALSCREEN_POSITION_OFFSET_BETWEEN_SCREENS;
	}

	vec2 center_offset = screen_center_pos - vec2(0.5, 0.5);

	if (USE_IMAGE_FOR_PLACEMENT == 0)
		placement_image_pos = vec2(0.5);

	float output_aspect_ratio = global.FinalViewportSize.x / global.FinalViewportSize.y;
	vec2 pos_offset = vec2(HSM_SCREEN_POSITION_X / output_aspect_ratio, HSM_SCREEN_POSITION_Y) + (placement_image_pos - 0.5);
	pos_offset += center_offset;

	return pos_offset;
}

vec2 HSM_GetPositionOffsetFor2ndScreen(vec2 screen_scale)
{
	float output_aspect_ratio = global.FinalViewportSize.x / global.FinalViewportSize.y;
	vec2 pos_offset = vec2(HSM_DUALSCREEN_2ND_SCREEN_POS_X / output_aspect_ratio, HSM_DUALSCREEN_2ND_SCREEN_POS_Y);

	vec2 screen_center_pos = vec2(0.5, 0.5);
	
	
	if (HSM_DUALSCREEN_MODE == 1)
	{
		if (HSM_DUALSCREEN_SHIFT_POSITION_WITH_SCALE == 1)
			screen_center_pos = vec2(0.5, 0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION + screen_scale.y * 1.17 / 2);
		else
			screen_center_pos = vec2(0.5, 0.75);

		screen_center_pos.y += HSM_DUALSCREEN_POSITION_OFFSET_BETWEEN_SCREENS;
	}

	if (HSM_DUALSCREEN_MODE == 2)
	{
		if (HSM_DUALSCREEN_SHIFT_POSITION_WITH_SCALE == 1)
			screen_center_pos = vec2(0.5 + HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION + screen_scale.x * 1.17 / 2, 0.5);
		else
			screen_center_pos = vec2(0.75, 0.5);

		screen_center_pos.x += HSM_DUALSCREEN_POSITION_OFFSET_BETWEEN_SCREENS;
	}

	vec2 center_offset = screen_center_pos - vec2(0.5, 0.5);
	pos_offset += center_offset;
	return pos_offset;
}

float HSM_GetAverageLuma(sampler2D Source, vec2 SourceSize)
{
   //////// Calculate Average Luminance ////////// 
   float m = max(log2(global.SourceSize.x), log2(global.SourceSize.y));
	m = max(m - 1.0, 1.0);
	
	float luma_total = 0.0;
	
	float num_samples = 5;
	float sample_dist = 1 / (num_samples - 1);
	vec4 tex_sample = vec4(0);
	for (float i = 0; i <= num_samples; i++)
	{
		for (float j = 0; j <= num_samples; j++)
		{
			tex_sample = textureLod(Source, vec2(sample_dist * i, sample_dist * j), m);
			luma_total += max(0.0, (tex_sample.r + tex_sample.g + tex_sample.g) / 3);
			// luma_total += max(0.0, length(tex_sample.rgb));
		}
	}
	luma_total = pow(0.577350269 * luma_total / (num_samples * num_samples), 0.6);
   return luma_total;
}

vec2 HSM_GetScreenPlacementYPosAndHeight(sampler2D in_sampler_2D, float num_samples)
{
	float screen_top_y_pos = 1;
	float screen_bottom_y_pos = 0;

	for (int i=0; i < num_samples; i++)
	{
		float y_pos = i * 1 / num_samples;
		float image_alpha = texture(in_sampler_2D, vec2(0.5, y_pos)).a;
		if (image_alpha < 0.5)
		{
			screen_top_y_pos = min(screen_top_y_pos, y_pos);
			screen_bottom_y_pos = max(screen_bottom_y_pos, y_pos);
		}
		
	}
   return vec2((screen_top_y_pos + screen_bottom_y_pos) / 2, screen_bottom_y_pos - screen_top_y_pos);
}

vec2 HSM_GetCoordWithPositionOffset(vec2 in_coord, vec2 position_offset)
{
	return in_coord - position_offset;
}

vec2 HSM_GetVTexCoordWithArgs(vec2 in_coord, vec2 in_scale, vec2 position_offset)
{
	return HSM_GetInverseScaledCoord(HSM_GetCoordWithPositionOffset(in_coord, position_offset), in_scale);
}

vec2 HSM_GetScreenVTexCoord(vec2 in_coord, vec2 in_screen_scale, vec2 position_offset)
{
	return HSM_GetVTexCoordWithArgs(in_coord, in_screen_scale, position_offset);
}

vec2 HSM_GetCurvatureScales(float screen_aspect)
{
	vec2 curvature_scales = screen_aspect < 1 ? vec2(HSM_CURVATURE_2D_SCALE_SHORT_AXIS, HSM_CURVATURE_2D_SCALE_LONG_AXIS)
											  : vec2(HSM_CURVATURE_2D_SCALE_LONG_AXIS,  HSM_CURVATURE_2D_SCALE_SHORT_AXIS);
	curvature_scales = (clamp(curvature_scales, 1, 5) - 1) + 1;
	return curvature_scales;
}

vec2 HSM_GetCurvatureValues(float screen_aspect)
{
	vec2 curvature_values = screen_aspect < 1 ? vec2(2 * HSM_CURVATURE_2D_SCALE_SHORT_AXIS * 2 / 100, HSM_CURVATURE_2D_SCALE_LONG_AXIS * 3 / 100)
											  : vec2(HSM_CURVATURE_2D_SCALE_LONG_AXIS * 3 / 100, 2 * HSM_CURVATURE_2D_SCALE_SHORT_AXIS * 2 / 100);

	return curvature_values;
}

//*
// CRT Geom Curvature
#define FIX(c) max(abs(c), 1e-5)

float intersect(vec2 in_coord , vec2 sinangle, vec2 cosangle, float in_radius, float in_distance)
{
  float A = dot(in_coord, in_coord) + in_distance.x * in_distance.x;
  float B = 2.0 * (in_radius * (dot(in_coord, sinangle) - in_distance.x * cosangle.x * cosangle.y) - in_distance.x * in_distance.x);
  float C = in_distance.x * in_distance.x + 2.0 * in_radius * in_distance.x * cosangle.x * cosangle.y;
  return (-B-sqrt(B * B - 4.0 * A * C)) / (2.0 * A);
}

vec2 bkwtrans(vec2 in_coord, vec2 sinangle, vec2 cosangle, float in_radius, float in_distance)
{
  float c = intersect(in_coord, sinangle, cosangle, in_radius, in_distance);
  vec2 pt = vec2(c) * in_coord;
  pt -= vec2(-in_radius) * sinangle;
  pt /= vec2(in_radius);
  vec2 tang = sinangle / cosangle;
  vec2 poc = pt / cosangle;
  float A = dot(tang, tang) + 1.0;
  float B = -2.0 * dot(poc, tang);
  float C = dot(poc,poc)-1.0;
  float a = (-B + sqrt(B * B - 4.0 * A * C)) / (2.0 * A);
  vec2 uv = (pt - a * sinangle) / cosangle;
  float r = FIX(in_radius * acos(a));
  return uv * r / sin(r / in_radius);
}

vec2 fwtrans(vec2 uv, vec2 sinangle, vec2 cosangle, float in_radius, float in_distance)
{
  float r = FIX(sqrt(dot(uv,uv)));
  uv *= sin(r/in_radius)/r;
  float x = 1.0-cos(r/in_radius);
  float D = in_distance/in_radius + x*cosangle.x*cosangle.y+dot(uv,sinangle);
  return in_distance*(uv*cosangle-x*sinangle)/D;
}

vec3 maxscale(vec2 sinangle, vec2 cosangle, float in_radius, float in_distance, float in_aspect)
{
  vec2 aspect_vec2 = vec2(1, 1/in_aspect);
  vec2 c = bkwtrans(-in_radius * sinangle / (1.0 + in_radius/in_distance*cosangle.x*cosangle.y), sinangle, cosangle, in_radius, in_distance);
  vec2 a = vec2(0.5,0.5)*aspect_vec2.xy;

  vec2 lo = vec2(	fwtrans(vec2(-a.x,c.y), sinangle, cosangle, in_radius, in_distance).x,
		 			fwtrans(vec2(c.x,-a.y), sinangle, cosangle, in_radius, in_distance).y)/aspect_vec2.xy;

  vec2 hi = vec2(	fwtrans(vec2(+a.x,c.y), sinangle, cosangle, in_radius, in_distance).x,
		 			fwtrans(vec2(c.x,+a.y), sinangle, cosangle, in_radius, in_distance).y)/aspect_vec2.xy;

  return vec3((hi+lo)*aspect_vec2.xy*0.5,max(hi.x-lo.x,hi.y-lo.y));
}

vec2 transform(vec2 coord, vec3 stretch, vec2 sinangle, vec2 cosangle, float in_radius, float in_distance, vec2 aspect)
{
  coord = (coord-vec2(0.5))*aspect.xy*stretch.z+stretch.xy;
  return (bkwtrans(coord, sinangle, cosangle, in_radius, in_distance)/aspect.xy+vec2(0.5));
}

vec2 HSM_GetGeomCurvedCoord(vec2 in_coord, float tilt_x, float tilt_y, float in_radius, float in_distance, float in_aspect)
{
	//default radius = 3.5
    //default distance = 2
	in_distance *= 1.4;
	vec2 ang = vec2(tilt_x, tilt_y);
	vec2 v_sinangle = sin(ang);
	vec2 v_cosangle = cos(ang);
	vec3 v_stretch = maxscale(v_sinangle, v_cosangle, in_radius, in_distance, in_aspect);
	vec2 aspect_vec2 = vec2(1, 1/in_aspect);
	vec2 curved_coord = transform(in_coord, v_stretch, v_sinangle, v_cosangle, in_radius, in_distance, aspect_vec2);

	return curved_coord;
}
//*/

/*
vec2 HSM_GetGuestCurvedCoord(vec2 in_coord, vec2 in_curvature)
{
	//Curvature Shape" 0.25 0.05 0.60 0.05
	float c_shape = 0.25;
	in_coord  = in_coord*2.0-1.0;    
	in_coord  = mix(in_coord, 
					vec2(in_coord.x*inversesqrt(1.0-c_shape*in_coord.y*in_coord.y), in_coord.y*inversesqrt(1.0-c_shape*in_coord.x*in_coord.x)), 
					vec2(in_curvature.x * 1.8, in_curvature.y * 1.8)/c_shape);
	return in_coord*0.5 + 0.5;
}
*/

vec2 HSM_GetCrtPiCurvedCoord(vec2 in_coord, vec2 in_curvature)
{
	// Barrel distortion shrinks the display area a bit, this will allow us to counteract that.
	in_curvature *= 5;
	vec2 barrelScale = 1.0 - (0.23 * in_curvature);
    in_coord -= vec2(0.5);
    float rsq = in_coord.x * in_coord.x + in_coord.y * in_coord.y;
    in_coord += in_coord * (in_curvature * rsq);
    in_coord *= barrelScale;
	in_coord += vec2(0.5);
    return in_coord;
}

vec2 HSM_GetTorridGristleCurvedCoord(vec2 in_coord, vec2 in_curvature){
	// default curvature is vec2(0.031, 0.041
	vec2 Distortion = in_curvature * 15;// * vec2(0.031, 0.041);

	vec2 curvedCoords = in_coord * 2.0 - 1.0;
	float curvedCoordsDistance = sqrt(curvedCoords.x*curvedCoords.x+curvedCoords.y*curvedCoords.y);

	curvedCoords = curvedCoords / curvedCoordsDistance;

	curvedCoords = curvedCoords * (1.0-pow(vec2(1.0-(curvedCoordsDistance/1.4142135623730950488016887242097)),(1.0/(1.0+Distortion*0.2))));

	curvedCoords = curvedCoords / (1.0-pow(vec2(0.29289321881345247559915563789515),(1.0/(vec2(1.0)+Distortion*0.2))));

	curvedCoords = curvedCoords * 0.5 + 0.5;
	return curvedCoords;
}

vec2 HSM_Get2DCurvedCoord(vec2 in_coord, vec2 curvature_values)
{
	vec2 ctr_curved_coord = vec2(0) ;

	// ctr_curved_coord = HSM_GetTorridGristleCurvedCoord(in_coord, curvature_values) - 0.5;

	// vec2 right_edge_curved_ctr_coord = HSM_GetTorridGristleCurvedCoord(vec2(1, 0.5), curvature_values) - 0.5;
	// ctr_curved_coord.x = ctr_curved_coord.x * 0.5 / right_edge_curved_ctr_coord.x;

	// vec2 bottom_edge_curved_ctr_coord =  HSM_GetTorridGristleCurvedCoord(vec2(0.5, 1), curvature_values) - 0.5;
	// ctr_curved_coord.y = ctr_curved_coord.y * 0.5 / bottom_edge_curved_ctr_coord.y;

	// if (abs(ctr_curved_coord.x) > 0.5 || abs(ctr_curved_coord.y) > 0.5)
	// {
		ctr_curved_coord = HSM_GetCrtPiCurvedCoord(in_coord, curvature_values) - 0.5;

		vec2 right_edge_curved_ctr_coord = HSM_GetCrtPiCurvedCoord(vec2(1, 0.5), curvature_values) - 0.5;
		ctr_curved_coord.x = ctr_curved_coord.x * 0.5 / right_edge_curved_ctr_coord.x;

		vec2 bottom_edge_curved_ctr_coord =  HSM_GetCrtPiCurvedCoord(vec2(0.5, 1), curvature_values) - 0.5;
		ctr_curved_coord.y = ctr_curved_coord.y * 0.5 / bottom_edge_curved_ctr_coord.y;
	// }

	return ctr_curved_coord + 0.5;
}

vec2 HSM_GetCurvedCoord(vec2 in_coord, float curvature_multiplier, float screen_aspect)
{
	if (HSM_CURVATURE_MODE == -1)
		return in_coord;
	
	float epsilon = 0.002;
	vec2 original_size = HSM_GetRotatedDerezedSize();

	vec2 adjusted_coord = in_coord;
	float tilt_angle_y = HSM_CURVATURE_3D_TILT_ANGLE_Y;
	float tilt_angle_x = HSM_CURVATURE_3D_TILT_ANGLE_X;
	float pin_inner_edge = 0;

	vec2 curved_coord = vec2(0);

#ifndef IS_POTATO_PRESET
	if (HSM_CURVATURE_MODE > 0.5)
	{
		// if (HSM_USE_GEOM > 0.5)
		// 	curved_coord = HSM_GetGeomCurvedCoord(in_coord, HSM_CURVATURE_3D_TILT_ANGLE_X, HSM_CURVATURE_3D_TILT_ANGLE_Y, HSM_CURVATURE_3D_RADIUS, HSM_CURVATURE_3D_VIEW_DIST, screen_aspect);
		// else
		// {
			float geom_radius_with_mult = HSM_CURVATURE_3D_RADIUS;
			if (HSM_CURVATURE_MODE == 1) geom_radius_with_mult -= 0.40;
			if (HSM_CURVATURE_MODE == 3) geom_radius_with_mult -= 1;
			geom_radius_with_mult *= (1 / (curvature_multiplier + epsilon));
			vec2 extra_curvature_mult = HSM_GetCurvatureScales(screen_aspect);
			mat2x2 pixel_to_video_uv;
			
			curved_coord = HRG_GetGeomCurvedCoord(	adjusted_coord, 
													HSM_CURVATURE_MODE, 
													geom_radius_with_mult,
													HSM_CURVATURE_3D_VIEW_DIST,
													tilt_angle_x,
													tilt_angle_y,
													screen_aspect,
													pin_inner_edge,
													global.SourceSize.xy,
													global.OutputSize.xy,
													pixel_to_video_uv);
		// }
	}
	else
	{
		vec2 curvature_values = curvature_multiplier * HSM_GetCurvatureValues(screen_aspect);
		curved_coord = HSM_Get2DCurvedCoord(adjusted_coord, curvature_values);
	}
#endif

#ifdef IS_POTATO_PRESET
	vec2 curvature_values = curvature_multiplier * HSM_GetCurvatureValues(screen_aspect);
	curved_coord = HSM_Get2DCurvedCoord(adjusted_coord, curvature_values);
#endif

	return curved_coord;
}

vec2 HSM_GetMirrorWrappedCoord(vec2 in_coord)
{	
	vec2 ctr_coord = in_coord - 0.5;
	if (abs(ctr_coord.x) > 0.5 || abs(ctr_coord.y) > 0.5 )
		in_coord = ctr_coord / HSM_SCREEN_REFLECTION_SCALE + 0.5 + vec2(HSM_SCREEN_REFLECTION_POS_X, HSM_SCREEN_REFLECTION_POS_Y);

	in_coord = mod(in_coord, 2);
	vec2 ctr_mirror_coord = in_coord - 0.5;

	float mirror_x = clamp(clamp(abs(ctr_mirror_coord.x) - 0.5, 0, 1) * 100000, 0, 1);
	float mirror_y = clamp(clamp(abs(ctr_mirror_coord.y) - 0.5, 0, 1) * 100000, 0, 1);

	ctr_mirror_coord.x = ctr_mirror_coord.x - mirror_x * 2 * sign(ctr_mirror_coord.x) * (abs(ctr_mirror_coord.x) - 0.5);
	ctr_mirror_coord.y = ctr_mirror_coord.y - mirror_y * 2 * sign(ctr_mirror_coord.y) * (abs(ctr_mirror_coord.y) - 0.5);

	return ctr_mirror_coord + 0.5;
}

// Borrowed from cgwg's crt-geom, under GPL
float HSM_GetCornerMask(vec2 in_coord, float screen_aspect, float corner_radius, float edge_sharpness)
// returns 0.0 - 1.0 value used for masking the corner so it looks round
{
	//(0.5 - abs(in_coord - 0.5)) * 2
	vec2 new_coord = min(in_coord, vec2(1.0) - in_coord) * vec2(screen_aspect, 1);
	vec2 corner_distance = vec2(max(corner_radius / 1000.0, (1.0 - edge_sharpness) * 0.01));
	new_coord = (corner_distance - min(new_coord, corner_distance));
	float distance = sqrt(dot(new_coord, new_coord));

	return clamp((corner_distance.x - distance) * (edge_sharpness * 500 + 100), 0.0, 1.0);
}

vec2 HSM_GetTubeCurvedCoord(vec2 screen_coord, float curvature_scale, vec2 screen_scale, vec2 tube_scale, float screen_aspect, float apply_black_edge_offset)
{
	vec2 black_edge_scale_offset = tube_scale / screen_scale;

	// Get the tube coord with the black edge added
	// vec2 tube_coord = HSM_GetInverseScaledCoord(screen_coord, black_edge_scale_offset);

	// Get the curved coordinate
	vec2 tube_curved_coord = vec2(0.5, 0.5);

	if (HSM_BZL_USE_INDEPENDENT_CURVATURE == 1)
	{
		vec2 curvature_values = screen_aspect < 1 ? vec2(2 * HSM_BZL_INDEPENDENT_CURVATURE_SCALE_SHORT_AXIS * 2 / 100, HSM_BZL_INDEPENDENT_CURVATURE_SCALE_LONG_AXIS * 3 / 100)
											: vec2(HSM_BZL_INDEPENDENT_CURVATURE_SCALE_LONG_AXIS * 3 / 100, 2 * HSM_BZL_INDEPENDENT_CURVATURE_SCALE_SHORT_AXIS * 2 / 100);
		curvature_values *= curvature_scale * HSM_BZL_INNER_CURVATURE_SCALE;
		tube_curved_coord = HSM_Get2DCurvedCoord(screen_coord, curvature_values);
	}
	else
	{
		tube_curved_coord = HSM_GetCurvedCoord(screen_coord, curvature_scale * HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE * HSM_BZL_INNER_CURVATURE_SCALE, screen_aspect);
	}
	if (apply_black_edge_offset == 1)
		tube_curved_coord = HSM_GetInverseScaledCoord(tube_curved_coord, black_edge_scale_offset);

	return tube_curved_coord;
}

// Light Illumination
// vec3 evaluateLight(in vec3 pos)
// {
//     vec3 lightPos = LPOS;
//     vec3 lightCol = LCOL;
//     vec3 L = lightPos-pos;
//     return lightCol * 1.0/dot(L,L);
// }

// vec3 evaluateLight(in vec3 pos, in vec3 normal)
// {
//     vec3 lightPos = LPOS;
//     vec3 L = lightPos-pos;
//     float distanceToL = length(L);
//     vec3 Lnorm = L/distanceToL;
//     return max(0.0,dot(normal,Lnorm)) * evaluateLight(pos);
// }

// vec4 HSM_ApplyBlackLevel(vec4 in_color, float color_is_linear)
// {
// 	vec4 out_color = in_color;

// 	// Linearize if color isn't linear
// 	if (color_is_linear == 0)
// 		out_color = HSM_Linearize(out_color, HSM_DEFAULT_CRT_GAMMA);

// 	// Black Level
// 	out_color += HSM_BRIGHTEN_BLACK / 100;
// 	out_color *= 1 / vec4(1.0 + HSM_BRIGHTEN_BLACK / 100);
// 	out_color = clamp(out_color, 0, 2);

// 	// If color isn't linear put default gamma back on
// 	if (color_is_linear == 0)
// 		out_color = HSM_Delinearize(out_color, HSM_DEFAULT_CRT_GAMMA);

// 	out_color.w = in_color.w;

// 	return clamp(out_color, 0, 1);
// }

vec2 HSM_RotateCoordinate(vec2 in_coord, float rotation_on)
{
	vec2 ctr_coord = in_coord - 0.5;
	ctr_coord = (1 - rotation_on) * ctr_coord + rotation_on * vec2(-ctr_coord.y, ctr_coord.x);
	return ctr_coord + 0.5;
}

vec2 HSM_GetMirrorWrapCoord(vec2 in_coord)
{
	vec2 ctr_coord = in_coord - 0.5;
	vec2 ctr_mirror_coord = vec2(0,0);

	float x_is_outside = clamp((clamp(abs(ctr_coord.x), 0.5, 1) - 0.5) * 100000, 0, 1);
	ctr_mirror_coord.x = (1 - x_is_outside) * ctr_coord.x + 
						x_is_outside * (ctr_coord.x - 2 * sign(ctr_coord.x) * (abs(ctr_coord.x) - 0.5));

	float y_is_outside = clamp((clamp(abs(ctr_coord.y), 0.5, 1) - 0.5) * 100000, 0, 1);
	ctr_mirror_coord.y = (1 - y_is_outside) * ctr_coord.y + 
						y_is_outside * (ctr_coord.y - 2 * sign(ctr_coord.y) * (abs(ctr_coord.y) - 0.5));

	return ctr_mirror_coord + 0.5;
}

float HSM_rand(inout float r)
{
	r = fract(3712.65 * r + 0.61432);
	return (r - 0.5) * 2.0;
}

vec4 HSM_GetStoichaicBlurredSample(sampler2D in_sampler, vec2 in_coord, float num_samples, float max_blur_size, float blur_ratio)
{
	if (num_samples < 1)
		return texture(in_sampler, in_coord);

	// Common value for max_blur_size is about 40
	float p = blur_ratio * max_blur_size / global.SourceSize.y;
	vec4 blurred_color = vec4(0.0);
	// srand
	float radius = sin(dot(in_coord, vec2(1233.224, 1743.335)));
	vec2 radius_vector;
	
	for(int i=0; i < num_samples; i++)
	{
		radius_vector.x = HSM_rand(radius);
		radius_vector.y = HSM_rand(radius);
		vec2 sample_coord = in_coord + radius_vector * p;

		blurred_color += texture(in_sampler, sample_coord) / num_samples;
	}
	return blurred_color;
}

int HSM_IsCoordIn2DRange(vec2 in_coord, vec4 in_2d_range)
{
	return (in_coord.x > in_2d_range.x && 
			in_coord.y > in_2d_range.y && 
			in_coord.x < in_2d_range.z && 
			in_coord.y < in_2d_range.w ) ? 1 : 0;
}

// Texture Sampler function which takes a coordinate in the cropped coordinate space
vec4 HSM_GetTexSampleFromSampleStartAndSize(sampler2D in_sampler, vec2 in_screen_coord, vec2 sample_start_pixel_coord, vec2 window_size)
{
	vec2 raw_original_size = HSM_GetRotatedDerezedSize();

	if ( HSM_DUALSCREEN_MODE > 0 )
		in_screen_coord.y = abs(HSM_FLIP_CORE_VERTICAL) * (1 - in_screen_coord.y) + (1 - abs(HSM_FLIP_CORE_VERTICAL)) * in_screen_coord.y;

	vec2 px_coord = SAMPLE_AREA_START_RAW_PIXEL_COORD + in_screen_coord * window_size;

	vec2 sample_coord = px_coord / raw_original_size;
	sample_coord =  HSM_RotateCoordinate(sample_coord, HSM_ROTATE_CORE_IMAGE);
	
	vec4 out_color = texture(in_sampler, sample_coord);

	return out_color;
}

// HSM_CROP_BLACK_ONLY
vec4 HSM_GetBlackOnlyCrop(sampler2D in_sampler_2D, vec2 sample_start_pixel_coord, vec2 window_size, float num_samples)
{
	// HSM_GetTexSampleFromSampleStartAndSize(sampler2D in_sampler, vec2 in_screen_coord, vec2 sample_start_pixel_coord, vec2 window_size)

	// Working except Bottom and Right edges jump back and forth and are not very accurate

	vec4 tex_sample = vec4(0);
	float brightness = 0;
	float test_crop = 0;

	float crop_left = floor(window_size.x * HSM_CROP_PERCENT_LEFT);
	float crop_top = floor(window_size.y * HSM_CROP_PERCENT_TOP);
	float crop_right = floor(window_size.x * HSM_CROP_PERCENT_RIGHT);
	float crop_bottom = floor(window_size.y * HSM_CROP_PERCENT_BOTTOM);

	// Should use as many samples as crop pixels
	float final_crop_left = crop_left;
	test_crop = crop_left;
	for (int i=0; i < crop_left; i++)
	{
		tex_sample = HSM_GetTexSampleFromSampleStartAndSize(in_sampler_2D, vec2((test_crop - 0.5) / window_size.x, 0.5), sample_start_pixel_coord, window_size);
		brightness = tex_sample.r + tex_sample.g + tex_sample.b;
		if (brightness > HSM_CROP_BLACK_THRESHOLD)
		{
			final_crop_left = min(final_crop_left, test_crop);
		}
		test_crop -= 1;
	}
	final_crop_left -= 1;

	float final_crop_top = crop_top;
	test_crop = crop_top;
	for (int i=0; i < crop_top; i++)
	{
		tex_sample = HSM_GetTexSampleFromSampleStartAndSize(in_sampler_2D, vec2(0.5, (test_crop - 0.5) / window_size.y), sample_start_pixel_coord, window_size);
		brightness = tex_sample.r + tex_sample.g + tex_sample.b;
		if (brightness > HSM_CROP_BLACK_THRESHOLD)
		{
			final_crop_top = test_crop;
		}
		test_crop -= 1;
	}
	final_crop_top -= 1;

	float final_crop_right = crop_right;
	test_crop = crop_right;
	for (int i=0; i < crop_right; i++)
	{
		tex_sample = HSM_GetTexSampleFromSampleStartAndSize(in_sampler_2D, vec2((window_size.x - test_crop + 0.5) / window_size.x, 0.5), sample_start_pixel_coord, window_size);
		brightness = tex_sample.r + tex_sample.g + tex_sample.b;
		if (brightness > HSM_CROP_BLACK_THRESHOLD)
		{
			final_crop_right = test_crop;
		}
		test_crop -= 1;
	}
	final_crop_right -= 2;

	float final_crop_bottom = crop_bottom;
	test_crop = crop_bottom;
	for (int i=0; i < crop_bottom; i++)
	{
		tex_sample = HSM_GetTexSampleFromSampleStartAndSize(in_sampler_2D, vec2(0.5, (window_size.y - test_crop + 0.5) / window_size.y), sample_start_pixel_coord, window_size);
		brightness = tex_sample.r + tex_sample.g + tex_sample.b;
		if (brightness > 0)
		{
			final_crop_bottom = test_crop;
		}
		test_crop -= 1;
	}
	final_crop_bottom -= 2;

	return clamp(vec4(final_crop_top, final_crop_bottom, final_crop_left, final_crop_right), 0, 512);
}

vec4 HSM_GetColorForScreenInfoCache(vec2 viewport_coord, sampler2D original_pass, sampler2D screen_placement_image)
{
	vec4 out_color = vec4(0);

	// First Screen
	vec2 screen_pos_and_height = HSM_GetScreenPlacementYPosAndHeight(screen_placement_image, 60);
	float screen_aspect = HSM_GetScreenAspect(1);
	vec2 screen_scale = HSM_GetScreenScale(screen_aspect, screen_pos_and_height.y);
	vec2 tube_scale = HSM_GetTubeScale(screen_scale, screen_pos_and_height.y);
	vec2 pos_offset = HSM_GetPositionOffset(vec2(0.5, screen_pos_and_height.x), screen_scale);

	vec2 cropped_original_size_with_core_res_mult = vec2(100);
	vec2 cropped_original_size = vec2(100);
	vec2 cropped_sample_area_start_pixel_coord = vec2(100);

	vec2 original_size = HSM_GetRotatedScreenDerezedSize(1);
	vec2 rotated_original_size = HSM_GetRotatedScreenDerezedSize(1);
	vec2 sample_area_start_pixel_coord = HSM_GetSampleAreaStartPixelCoord(1);

	if (HSM_CROP_MODE == 0)
	{
		cropped_original_size_with_core_res_mult = HSM_GetRotatedOriginalSizeWithCoreResMult(1);
		cropped_original_size = original_size;
		cropped_sample_area_start_pixel_coord = sample_area_start_pixel_coord;
	}
	if (HSM_CROP_MODE == 1)
	{
		vec4 cropping = HSM_GetBlackOnlyCrop(original_pass, sample_area_start_pixel_coord, rotated_original_size, 100);

		float crop_top = cropping.x;
		float crop_bottom = cropping.y;
		float crop_left = cropping.z;
		float crop_right = cropping.w;

		cropped_original_size = rotated_original_size - vec2(crop_left + crop_right, crop_top + crop_bottom);
		cropped_sample_area_start_pixel_coord = sample_area_start_pixel_coord + vec2(crop_left, crop_top);

		vec2 sampling_mult = HSM_GetCoreSamplingResMult();
		sampling_mult = HSM_ROTATE_CORE_IMAGE == 1 ? sampling_mult.yx : sampling_mult.xy;
		cropped_original_size_with_core_res_mult = ceil(cropped_original_size * sampling_mult.xy);
	}
	if (HSM_CROP_MODE == 2)
	{
		cropped_original_size_with_core_res_mult = HSM_GetCroppedRotatedOriginalSizeWithCoreResMult(1);
		cropped_original_size = HSM_GetCroppedRotatedDerezedSize(1);
		cropped_sample_area_start_pixel_coord = HSM_GetSampleAreaStartPixelCoord(1) + floor(vec2(rotated_original_size.x * HSM_CROP_PERCENT_LEFT, rotated_original_size.y * HSM_CROP_PERCENT_TOP));
	}

	// Second Screen
	vec2 cropped_original_size_with_core_res_mult_2nd_screen = HSM_GetCroppedRotatedOriginalSizeWithCoreResMult(2);
	vec2 cropped_original_size_2nd_screen = HSM_GetCroppedRotatedDerezedSize(2);
	vec2 sample_area_start_in_raw_pixels_2nd_screen = HSM_GetSampleAreaStartPixelCoord(2);

	float screen_aspect_2nd_screen = HSM_DUALSCREEN_2ND_SCREEN_ASPECT_RATIO_MODE == 0 ? screen_aspect : cropped_original_size_2nd_screen.x / cropped_original_size_2nd_screen.y;
	vec2 screen_scale_2nd_screen = HSM_GetScreenScaleFor2ndScreen(screen_scale, screen_aspect_2nd_screen);
	vec2 tube_scale_2nd_screen = HSM_GetTubeScale(screen_scale_2nd_screen, DEFAULT_UNCORRECTED_SCREEN_SCALE.y);
	vec2 pos_offset_2nd_screen = HSM_GetPositionOffsetFor2ndScreen(screen_scale_2nd_screen);


	vec4 sample_2d_range = vec4(0);
	sample_2d_range = vec4(0, 0, 0.25, 0.25);

	// 1st Sample
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.r = HSM_GetAverageLuma(original_pass, global.DerezedPassSize.xy);
	}

	// 2nd Sample
	sample_2d_range = vec4(0.25, 0, 0.5, 0.25);
	// sample_2d_range = HSM_GetInfoSampleRange(2);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{ 
		out_color.r = screen_aspect;
		out_color.ba = screen_scale;
	}

	// 3rd Sample
	sample_2d_range = vec4(0.5, 0, 0.75, 0.25);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = tube_scale;
		out_color.ba = pos_offset;
	}

	// 4th Sample
	sample_2d_range = vec4(0.75, 0, 1, 0.25);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_original_size_with_core_res_mult;
	}

	// 5th Sample
	sample_2d_range = vec4(0, 0.25, 0.25, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_original_size;
		out_color.ba = cropped_sample_area_start_pixel_coord;
	}

	// 6th Sample
	sample_2d_range = vec4(0.25, 0.25, 0.5, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.r = screen_aspect_2nd_screen;
		out_color.ba = screen_scale_2nd_screen;
	}

	// 7th Sample
	sample_2d_range = vec4(0.5, 0.25, 0.75, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = tube_scale_2nd_screen;
		out_color.ba = pos_offset_2nd_screen;
	}

	// 8th Sample
	sample_2d_range = vec4(0.75, 0.25, 1, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_original_size_with_core_res_mult_2nd_screen;
	}

	// 9th Sample
	sample_2d_range = vec4(0, 0.5, 0.25, 0.75);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_original_size_2nd_screen;
		out_color.ba = sample_area_start_in_raw_pixels_2nd_screen;
	}

	return out_color;
}

vec4 HSM_UpdateGlobalScreenValuesFromCache(sampler2D in_cache_pass, vec2 viewport_coord)
{
	SCREEN_INDEX = HSM_GetScreenIndex(viewport_coord);
	ROTATED_RAW_ORIGINAL_SIZE = HSM_GetRotatedDerezedSize();
	ROTATED_CORE_ORIGINAL_SIZE = HSM_GetRotatedCoreOriginalSize();
	vec2 sample_coord = vec2(0);
	vec4 texture_sample = vec4(0);

	// 1st Sample
	sample_coord = vec2(0.125, 0.125);
	texture_sample = texture(in_cache_pass, sample_coord);
	AVERAGE_LUMA = texture_sample.a;

	if (SCREEN_INDEX == 1)
	{
		// 2nd Sample
		sample_coord = vec2(0.375, 0.125);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_ASPECT = texture_sample.r;
		SCREEN_SCALE = texture_sample.ba;

		// 3rd Sample
		sample_coord = vec2(0.675, 0.125);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_SCALE = texture_sample.rg;
		SCREEN_POS_OFFSET = texture_sample.ba;

		// 4th Sample
		sample_coord = vec2(0.875, 0.125);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT = texture_sample.rg;

		// 5th Sample
		sample_coord = vec2(0.125, 0.375);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_ORIGINAL_SIZE = texture_sample.rg;
		SAMPLE_AREA_START_RAW_PIXEL_COORD = texture_sample.ba;
	}
	// If we are in the section of the viewport which is the second screen
	if (SCREEN_INDEX == 2)
	{
		// 6th Sample
		sample_coord = vec2(0.375, 0.375);
		texture_sample = texture(in_cache_pass, sample_coord);
		SCREEN_ASPECT = texture_sample.r;
		SCREEN_SCALE = texture_sample.ba;

		// 7th Sample
		sample_coord = vec2(0.675, 0.375);
		texture_sample = texture(in_cache_pass, sample_coord);
		TUBE_SCALE = 		texture_sample.rg;
		SCREEN_POS_OFFSET = texture_sample.ba;

		// 8th Sample
		sample_coord = vec2(0.875, 0.375);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT = texture_sample.rg;

		// 9th Sample
		sample_coord = vec2(0.125, 0.675);
		texture_sample = texture(in_cache_pass, sample_coord);
		CROPPED_ROTATED_ORIGINAL_SIZE = texture_sample.rg;
		SAMPLE_AREA_START_RAW_PIXEL_COORD = texture_sample.ba;
	}

	USE_VERTICAL_SCANLINES = HSM_GetUseVerticalScanlines(SCREEN_ASPECT);
	SCREEN_COORD = HSM_GetVTexCoordWithArgs(viewport_coord, SCREEN_SCALE, SCREEN_POS_OFFSET);

	return vec4(0);
}

// Texture Sampler function which takes a coordinate in the cropped coordinate space
vec4 HSM_GetCroppedTexSample(sampler2D in_sampler, vec2 in_screen_coord)
{
	vec2 raw_original_size = HSM_GetRotatedDerezedSize();

	vec4 out_color = HSM_GetTexSampleFromSampleStartAndSize(in_sampler, in_screen_coord, SAMPLE_AREA_START_RAW_PIXEL_COORD, CROPPED_ROTATED_ORIGINAL_SIZE);

	return out_color;
}

float HSM_GetVignetteFactor(vec2 coord, float amount, float size)
{
	float orig_mamehlsl_amount = amount;
	vec2 ctr_coord = coord - 0.5;

	float vignette_length = length(ctr_coord * vec2(0.5 / size * global.OutputSize.x/global.OutputSize.y + 0.5, 1));
	float vignette_blur = (orig_mamehlsl_amount * 0.75) + 0.25;

	// 0.5 full screen fitting circle
	float vignette_radius = 1.0 - (orig_mamehlsl_amount * 0.25);
	float vignette = smoothstep(vignette_radius, vignette_radius - vignette_blur, vignette_length);

	float vignette_multiplier = smoothstep(0, 0.05, amount);
	return 1 - vignette_multiplier + vignette * vignette_multiplier;
}

/*
float HSM_GetScreenVignetteFactor(vec2 coord, float amount)
{
	coord = (coord - 0.5) * 0.999 + 0.5;
    coord *=  1.0 - coord.yx;
    
    float vignette = coord.x * coord.y * 20; // multiply with sth for intensity
    
    vignette = pow(vignette, amount * 3); // change pow for modifying the extend of the  vignette
	float vignette_multiplier = smoothstep(0, 0.05, amount);
	if (amount < 0.01) return 1;
	else return vignette;
}
*/

float HSM_GetScreenVignetteFactor(vec2 in_coord)
{
	vec2 vpos = HSM_GetMirrorWrappedCoord(in_coord);
	vpos = (vpos - 0.5) / 1.01 + 0.5;
	vpos *= 1.0 - vpos.xy;
    float vig = vpos.x * vpos.y * HSM_SCREEN_VIGNETTE_STRENGTH;
    vig = min(pow(vig, HSM_SCREEN_VIGNETTE_POWER), 1.0);

	return vig;
}


// TODO need to remove calls to Get RotatedOriginalSize etc
// Torridgristle - ScanlineSimple pass - Public domain
vec4 HSM_ApplyScanlineMask(vec4 in_color, vec2 screen_scale, vec2 in_coord, vec2 in_curved_coord)
{
	//   Stuff to try implementing
    //   Option to try to match a integer division of the core resolution
    //   Curvature Multiplier
    //   Try mame hlsl darkening
    //   Check Lottes tone mapping
    //   Crawl

	in_coord = mix(in_coord, in_curved_coord, HSM_FAKE_SCANLINE_CURVATURE);

    /* Scanlines */
	float scanline_roll_offset = float(mod(global.FrameCount, 1280)) / 1280 * HSM_FAKE_SCANLINE_ROLL; 

    // float scans = clamp( 0.35+0.18*sin(6.0*time-curved_uv.y*resolution.y*1.5), 0.0, 1.0);
    // float s = pow(scans,0.9);
    // col = col * vec3(s);

	vec2 original_size = HSM_GetRotatedScreenDerezedSize(SCREEN_INDEX);
	float use_vert_scanlines = HSM_GetUseVerticalScanlines(original_size.x / original_size.y);
	vec2 sampling_res = HSM_GetRotatedOriginalSizeWithCoreResMult(SCREEN_INDEX);
	float scan_axis_res = use_vert_scanlines * sampling_res.x + (1 - use_vert_scanlines) * sampling_res.y;
	float scan_axis_pos = use_vert_scanlines * in_coord.x + (1 - use_vert_scanlines) * in_coord.y;
	scan_axis_pos += scanline_roll_offset;

	bool apply_scanlines = HSM_FAKE_SCANLINE_OPACITY > 0.001 && (HSM_FAKE_SCANLINE_MODE == 1 || (HSM_FAKE_SCANLINE_MODE == 2 && scan_axis_res > HSM_INTERLACE_TRIGGER_RES));

	if (!apply_scanlines)
		return in_color;

	float pi = 3.141592654;

	vec2 screen_size = global.OutputSize.xy * screen_scale;
	float scan_axis_screen_scale_res = use_vert_scanlines * screen_size.x + (1 - use_vert_scanlines) * screen_size.y;
	
	float simulated_scanline_res = HSM_FAKE_SCANLINE_RES;

	float scanline_size = scan_axis_screen_scale_res / simulated_scanline_res;

	if (HSM_FAKE_SCANLINE_INT_SCALE == 1)
		scanline_size = ceil(scanline_size);

	float scan = mod(scan_axis_pos * scan_axis_screen_scale_res, scanline_size) / scanline_size;

	// Alternate, modulating the scanline width depending on brightness
	//float scanline_mask = HHLP_EasePowerOut(1 - abs(scan - 0.5) * 2, 0.5 + 2 * smoothstep(0.4, 0.9, (in_color.r + in_color.g +  in_color.b) / 3));
	float color_brightness_modulation = HHLP_EasePowerOut(smoothstep(0.4, 0.99, (in_color.r + in_color.g +  in_color.b) / 3), 2);

	float scanline_mask = 1 - abs(scan - 0.5) * 2;
	scanline_mask = pow(1 - scanline_mask, 1);

	float final_scanline_mask = clamp(1 * scanline_mask, 0, 1);

	color_brightness_modulation = HHLP_EasePowerOut(smoothstep(0.4, HSM_FAKE_SCANLINE_BRIGHTNESS_CUTOFF + 1.5, (in_color.r + in_color.g +  in_color.b) / 3), 2);
	final_scanline_mask = clamp(mix(1, mix(final_scanline_mask, 1, color_brightness_modulation), HSM_FAKE_SCANLINE_OPACITY), 0, 1);

	vec4 masked_color = in_color;
	masked_color *= 1 + 0.5 * HSM_FAKE_SCANLINE_OPACITY;
	masked_color = clamp(final_scanline_mask * masked_color, 0, 1);
	masked_color.w = in_color.w;

	// Split Screen for debugging
	// masked_color = scan_opposite_axis_pos > 0.7 ? in_color : masked_color;

	return clamp(masked_color, 0, 1);
}

vec4 HSM_GetNightLightingMultiplyColor( vec2 VIEWPORT_COORD, in sampler2D NightLightingImage )
{
	vec2 night_lighting_coord = VIEWPORT_COORD;
	
	night_lighting_coord = (night_lighting_coord - 0.5) / HSM_NIGHT_SCALE + 0.5;

	if (HSM_NIGHT_MIRROR_HORZ == 1)
		night_lighting_coord.x = (night_lighting_coord.x - 0.5) * -1 + 0.5;

	// Dithering if needed to reduce banding
	float blur_max_size = 1;
	float blur_amount = 0.2;
	
	vec4 night_lighting_image = HSM_GetStoichaicBlurredSample(NightLightingImage, night_lighting_coord.xy, HSM_NIGHT_DITHERING_SAMPLES, blur_max_size, blur_amount);
	night_lighting_image = HSM_Linearize(night_lighting_image, HSM_DEFAULT_SRGB_GAMMA);

	// Do HSV alterations on the night lighting image
	if (HSM_NIGHT_HUE != 0 || HSM_NIGHT_SATURATION != 1 || HSM_NIGHT_VALUE != 1)
	{
		vec3 night_lighting_image_hsv = HSM_RGBtoHSV(night_lighting_image.rgb);
		night_lighting_image_hsv.x += HSM_NIGHT_HUE;
		night_lighting_image_hsv.y *= HSM_NIGHT_SATURATION;
		night_lighting_image_hsv.z *= HSM_NIGHT_VALUE;
		night_lighting_image = vec4(HSM_HSVtoRGB(night_lighting_image_hsv), night_lighting_image.a);
	}

	night_lighting_image.rgb = mix( vec3(1), night_lighting_image.rgb, HSM_NIGHT_OPACITY );

	return night_lighting_image;
}

vec4 HSM_ApplyMonochrome(vec4 in_color)
{
	vec4 out_color = in_color;
	out_color.rgb = pow(out_color.rgb, HSM_MONOCHROME_GAMMA.xxx);
	float luma = dot(out_color.rgb, vec3(0.299, 0.587, 0.114));
	vec3 mcolor = vec3(1.0);
	if (HSM_MONOCHROME_MODE > 1.5) mcolor = (HSM_MONOCHROME_MODE > 2.5) ? vec3(0.2549, 1.0, 0.0) : vec3(1.0, 0.749, 0.0);

	if (HSM_MONOCHROME_HUE_OFFSET != 0 || HSM_MONOCHROME_SATURATION != 0)
	{
		vec3 mcolor_hsv = HSM_RGBtoHSV(mcolor);
		mcolor_hsv.x += HSM_MONOCHROME_HUE_OFFSET;
		mcolor_hsv.y *= HSM_MONOCHROME_SATURATION;
		mcolor = HSM_HSVtoRGB(mcolor_hsv);
	}

	out_color.rgb = pow(luma, 1.0/HSM_MONOCHROME_GAMMA) * mcolor;
	return out_color;
}

float HSM_Get_Screen_Black_Edge_Mask(vec2 screen_coord, bool apply_to_mirror)
{
	if (apply_to_mirror)
		screen_coord = HSM_GetMirrorWrappedCoord(screen_coord);

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	SCREEN_BLACK_EDGE_CURVED_COORD = HSM_GetCurvedCoord(screen_coord, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, SCREEN_ASPECT);
	SCREEN_MASK = HSM_GetCornerMask(((SCREEN_BLACK_EDGE_CURVED_COORD - 0.5) * 1.001) + 0.5, SCREEN_ASPECT, HSM_SCREEN_CORNER_RADIUS, HSM_SCREEN_EDGE_SHARPNESS);

	return SCREEN_MASK;
}

vec4 HSM_Get_Tube_Highlight(vec2 tube_curved_coord, bool apply_to_mirror, in sampler2D TubeStaticReflectionImage)
{
	vec4 out_color = vec4(0);
	
	vec2 mirrored_tube_coord = tube_curved_coord;
	if (apply_to_mirror)
		mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	// float outside_tube_mask = 1 - HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	// TUBE_MASK = 1 - outside_tube_mask;
	// out_color *= TUBE_MASK;

	if (HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY > 0)
	{
		vec2 tube_highlight_coord = (mirrored_tube_coord - 0.5) / HSM_TUBE_STATIC_REFLECTION_IMAGE_SCALE + 0.5;

		// TODO need to update this for flip horizontal
		tube_highlight_coord = mix( tube_highlight_coord, -1 * (tube_highlight_coord - 0.5) + 0.5 , HSM_FLIP_VIEWPORT_VERTICAL);
/*
		// Dithering for the glass reflection if needed to reduce banding
		float blur_max_size = 1;
		float blur_amount = 0.2;
		vec4 tube_highlight_image = HSM_GetStoichaicBlurredSample(TubeStaticReflectionImage, tube_highlight_coord, HSM_TUBE_STATIC_REFLECTION_IMAGE_DITHERING_SAMPLES, blur_max_size, blur_amount);
*/
		// Mask the very outer edge of the tube for a very small shadowing effect
		float tube_highlight_mask = HSM_GetCornerMask((tube_curved_coord - 0.5) * 0.995 + 0.5 , SCREEN_ASPECT, HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS, 0.05);

		vec4 tube_highlight_image = texture(TubeStaticReflectionImage, tube_highlight_coord);
		tube_highlight_image = HSM_Linearize(tube_highlight_image, HSM_DEFAULT_SRGB_GAMMA) * tube_highlight_mask;

		out_color = HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY * tube_highlight_image * tube_highlight_image.a;
	}
	return out_color;
}

#ifdef IS_POTATO_PRESET
vec4 HSM_GetPostCrtPreppedColorPotato(vec4 in_color_with_gamma, vec2 VIEWPORT_COORD, vec2 screen_curved_coord, in sampler2D BackgroundImage, in sampler2D BackgroundVertImage)
{ 
	if (HSM_MONOCHROME_MODE > 0.5)
		in_color_with_gamma = HSM_ApplyMonochrome(in_color_with_gamma);

	vec4 out_color = HSM_Linearize(in_color_with_gamma, HSM_DEFAULT_SRGB_GAMMA);
	
	out_color *= HSM_POST_CRT_BRIGHTNESS;

	float screen_mask = HSM_GetCornerMask((screen_curved_coord - 0.5) * 0.999 + 0.5, SCREEN_ASPECT, HSM_SCREEN_CORNER_RADIUS, 0.9);
	out_color *= screen_mask;

	if (HSM_BG_OPACITY > 0)
	{
		//-----------------------------------------------------------------------------------------
		// Background
		//-----------------------------------------------------------------------------------------
		BACKGROUND_CURVED_COORD = VIEWPORT_COORD;
		// Correct image coordinate for image aspect ratio
		BACKGROUND_CURVED_COORD.x = (BACKGROUND_CURVED_COORD.x - 0.5) / (HSM_GetAspectRatioFromMode(HSM_BG_TEXTURE_ASPECT_MODE, HSM_BG_EXPLICIT_TEXTURE_ASPECT) / (global.FinalViewportSize.x/global.FinalViewportSize.y)) + 0.5;

		vec4 bg_layer = vec4(0);
		if (SCREEN_ASPECT > 1) 
			bg_layer = texture(BackgroundImage, BACKGROUND_CURVED_COORD);
		else
			bg_layer = texture(BackgroundVertImage, BACKGROUND_CURVED_COORD);
		bg_layer = HSM_Linearize(bg_layer, HSM_DEFAULT_SRGB_GAMMA);

		bg_layer.rgb *= HSM_BG_BRIGHTNESS;

		if (HSM_STANDARD_BG_SHOW_OVER_SCREEN == 1)
			out_color = HSM_BlendModeLayerMix(out_color, bg_layer, HSM_BG_BLEND_MODE, HSM_BG_OPACITY);
		else
			out_color = HSM_BlendModeLayerMix(bg_layer, out_color, HSM_CRT_SCREEN_BLEND_MODE, 1);
	}

	// Apply the final Gamma
	out_color = HSM_Delinearize(out_color, HSM_DEFAULT_SRGB_GAMMA);
	return out_color;
}
#endif

vec4 HSM_GetPostCrtPreppedColor(vec4 in_color_with_gamma, vec2 VIEWPORT_COORD, vec2 screen_curved_coord, in sampler2D source_pass, bool source_is_linear, in sampler2D ColoredGelImage, in sampler2D TubeStaticReflectionImage, in sampler2D BackgroundImage, in sampler2D BackgroundVertImage, in sampler2D NightLightingImage)
{
	if (HSM_MONOCHROME_MODE > 0.5)
		in_color_with_gamma = HSM_ApplyMonochrome(in_color_with_gamma);

	vec4 out_color = HSM_Linearize(in_color_with_gamma, HSM_DEFAULT_SRGB_GAMMA);
	out_color *= HSM_POST_CRT_BRIGHTNESS;

	if (HSM_SCREEN_VIGNETTE_ON > 0.5)
		out_color *= HSM_GetScreenVignetteFactor(screen_curved_coord);

	vec4 night_lighting_image = HSM_GetNightLightingMultiplyColor( VIEWPORT_COORD, NightLightingImage );

	// float screen_edge_mask = HSM_GetCornerMask(screen_curved_coord, SCREEN_ASPECT, HSM_SCREEN_CORNER_RADIUS, HSM_SCREEN_EDGE_SHARPNESS);
	// screen_edge_mask = 1 - ((1 - screen_edge_mask) * screen_mask);

	// out_color *= screen_edge_mask;

	vec2 mirrored_screen_coord = HSM_GetMirrorWrappedCoord(screen_curved_coord);

	if (HSM_CRT_ORIGINAL_AB_COMPARE_ON == 1)
	{
		vec4 source_color = HSM_GetCroppedTexSample(source_pass, mirrored_screen_coord);
		if (!source_is_linear)
			source_color = HSM_Linearize(source_color, HSM_GAMMA_IN_LINEARIZE);
		source_color = HSM_Delinearize(source_color, HSM_GAMMA_OUT_CRT);
		source_color = HSM_Linearize(source_color, HSM_DEFAULT_SRGB_GAMMA);

		out_color = mix(out_color, source_color, mirrored_screen_coord.x < HSM_CRT_ORIGINAL_AB_COMPARE_SPLIT_POSITION ? 1 : 0);
	}

/*
	float vignette_factor = HSM_GetScreenVignetteFactor(mirrored_screen_coord, HSM_SCREEN_VIGNETTE);

	float vignette_factor_outside_screen = HSM_SCREEN_VIGNETTE_IN_REFLECTION * vignette_factor + (1 - HSM_SCREEN_VIGNETTE_IN_REFLECTION) * 1;
	vignette_factor = screen_mask * vignette_factor + (1 - screen_mask) * vignette_factor_outside_screen;

	out_color *= vignette_factor;
*/

	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);
	vec2 mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);

	if (HSM_FAKE_SCANLINE_OPACITY > 0.001 && HSM_FAKE_SCANLINE_MODE > 0.5)
	{
		vec4 scanline_masked_color = HSM_ApplyScanlineMask(out_color, SCREEN_SCALE, SCREEN_COORD, screen_curved_coord);
		
		// Darken the outside image a bit
		out_color = mix(out_color, out_color * 0.9, HSM_FAKE_SCANLINE_OPACITY);

		// Show scanlines only in the tube area
		float tube_highlight_mask = HSM_GetCornerMask((tube_curved_coord - 0.5) * 0.995 + 0.5 , SCREEN_ASPECT, HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS, 0.05);
		out_color = mix(out_color, scanline_masked_color, tube_highlight_mask);
	}

	// Add a Colored Gel image on top of the screen, this is what colors games like space invaders or battlezone
	if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0 || HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0 || HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
	{
		// TODO need to update this for flip horizontal
		vec2 gel_coord = mix( mirrored_tube_coord, -1 * (mirrored_tube_coord - 0.5) + 0.5 , HSM_FLIP_VIEWPORT_VERTICAL);
		vec4 gel_image = texture(ColoredGelImage, mirrored_tube_coord);
		gel_image = HSM_Linearize(gel_image, HSM_DEFAULT_SRGB_GAMMA);

		if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0)
			out_color = mix(out_color, out_color * gel_image, HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT);

		gel_image.rgb *= night_lighting_image.rgb;
		if (HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0)
			out_color = mix(out_color, out_color + gel_image, HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT);

		gel_image.a = clamp( gel_image.a - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD, 0, 1 ) / (1 - HSM_TUBE_COLORED_GEL_IMAGE_TRANSPARENCY_THRESHOLD);
		gel_image.rgb *= HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_BRIGHTNESS;

		if (HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
			if ( HSM_SHOW_CRT_ON_TOP_OF_COLORED_GEL == 0 )
				out_color = HSM_BlendModeLayerMix(out_color, gel_image, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
			if ( HSM_SHOW_CRT_ON_TOP_OF_COLORED_GEL == 1 )
			{
				vec4 normal_blended = HSM_BlendModeLayerMix(vec4(0, 0, 0, 1), gel_image, BLEND_MODE_NORMAL, HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT);
				out_color = normal_blended + out_color;
			}
	}

	out_color.a = in_color_with_gamma.a;

	#ifdef IS_NOREFLECT_PRESET
	out_color.rgb *= HSM_Get_Screen_Black_Edge_Mask(SCREEN_COORD, false);

	// TUBE HIGHLIGHT
	if (HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_NONE)
		if (HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY > 0)
			out_color += HSM_Get_Tube_Highlight(tube_curved_coord, false, TubeStaticReflectionImage);
	#endif

	#ifndef IS_NOREFLECT_PRESET
	// Apply the final Gamma
	out_color = HSM_Delinearize(out_color, HSM_DEFAULT_SRGB_GAMMA);
	#endif

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	TUBE_MASK = HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);

	if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_TUBE )
		out_color = HSM_PreMultAlphaBlend(out_color, vec4(0, 0, 0, 1) * (1 - TUBE_MASK));

	if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_TUBE || HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_SCREEN )
		out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (1 - TUBE_MASK));

	return out_color;
}


