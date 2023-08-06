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

float FILL_MODE_KEEP_TEXTURE_ASPECT = 0;
float FILL_MODE_SPLIT = 1;
float FILL_MODE_STRETCH = 2;

vec2 HSM_GetOuterBezelScale(vec2 tube_diffuse_scale, float screen_aspect)
{

	vec2 bezel_outer_scale_offset = vec2(HSM_BZL_WIDTH / screen_aspect, HSM_BZL_HEIGHT ) + vec2(1);
	return bezel_outer_scale_offset;
}

vec2 GetDefaultScreenScale()
{
	float output_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;
	vec2 out_placement_scale = DEFAULT_UNCORRECTED_SCREEN_SCALE;
	out_placement_scale.x /= output_aspect;
	return out_placement_scale;
}

vec2 GetDefaultBezelScale()
{
	float output_aspect = global.FinalViewportSize.x / global.FinalViewportSize.y;
	vec2 out_placement_scale = DEFAULT_UNCORRECTED_BEZEL_SCALE;
	out_placement_scale.x /= output_aspect;
	return out_placement_scale;
}

float HSM_GetSimpleBezelCoords(vec2 tube_diffuse_coord, 
								vec2 tube_diffuse_scale, 
								vec2 tube_scale, 
								float screen_aspect,
								inout vec2 reflection_outer_boundary_coord, 
								inout vec2 bezel_outside_coord, 
								inout vec2 frame_outside_coord)
{
	float output_aspect = global.OutputSize.x / global.OutputSize.y;

	vec2 bezel_outer_pos_offset = vec2(0, HSM_BZL_OUTER_POSITION_Y);
	vec2 bezel_outer_scale_offset = HSM_GetOuterBezelScale(tube_diffuse_scale, screen_aspect);

	bezel_outside_coord = tube_diffuse_coord + bezel_outer_pos_offset;

	vec2 black_edge_scale_offset = tube_scale / tube_diffuse_scale;

	bezel_outside_coord = HSM_GetInverseScaledCoord(bezel_outside_coord, black_edge_scale_offset * bezel_outer_scale_offset) + vec2(0, HSM_BZL_OUTER_POSITION_Y);
	frame_outside_coord = (bezel_outside_coord + 
							vec2(0, HSM_FRM_OUTER_POS_Y) - 0.5) / 
							vec2((HSM_FRM_THICKNESS * HSM_FRM_THICKNESS_SCALE_X) / 
							(tube_diffuse_scale.x / tube_diffuse_scale.y * output_aspect) + 1, 
							HSM_FRM_THICKNESS + 1) + 0.5;
	reflection_outer_boundary_coord = HSM_SCREEN_REFLECTION_ALLOW_REFLECTION * bezel_outside_coord + 
										(1 - HSM_SCREEN_REFLECTION_ALLOW_REFLECTION) * tube_diffuse_coord;
	return 0;
}

// Assumes that HSM_UpdateGlobalScreenValuesFromCache has already been called
bool HSM_IsOutsideReflectionBoundary()
{
	vec2 cache_bounds_coord = TUBE_COORD;
	if (HSM_SCREEN_REFLECTION_ALLOW_REFLECTION > 0.5)
	{
		// Have to get the scale of the coordinates so we can figure out the size of the onscreen rectangle of the area 
		vec2 reflection_outer_boundary_coord;
		vec2 bezel_outside_flat_coord;
		vec2 frame_outside_flat_coord;
		HSM_GetSimpleBezelCoords(TUBE_DIFFUSE_COORD, 
								TUBE_DIFFUSE_SCALE, 
								TUBE_SCALE, 
								SCREEN_ASPECT,
								reflection_outer_boundary_coord,
								bezel_outside_flat_coord, 
								frame_outside_flat_coord);
		cache_bounds_coord = (reflection_outer_boundary_coord - 0.5) * (1 / HSM_SCREEN_REFLECTION_SOURCE_BOUNDARY_SCALE) + 0.5;
	}
	return HHLP_IsOutsideCoordSpace(cache_bounds_coord);
}

float HSM_GetReflectionBoundaryMask()
{
	return float(!HSM_IsOutsideReflectionBoundary());
}

float HSM_GetBezelCoords(vec2 tube_diffuse_coord, 
							vec2 tube_diffuse_scale, 
							vec2 tube_scale, 
							float tube_diffuse_aspect, 
							bool curve_coords_on,
							inout vec2 bezel_outside_scale,
							inout vec2 bezel_outside_coord, 
							inout vec2 bezel_outside_curved_coord, 
							inout vec2 frame_outside_curved_coord)
{
	float output_aspect = global.OutputSize.x / global.OutputSize.y;

	vec2 bezel_outer_pos_offset = vec2(0, HSM_BZL_OUTER_POSITION_Y);
	vec2 bezel_outer_scale_offset = HSM_GetOuterBezelScale(tube_diffuse_scale, tube_diffuse_aspect);

	bezel_outside_coord = tube_diffuse_coord + bezel_outer_pos_offset;
	bezel_outside_curved_coord = bezel_outside_coord;

	// Only run curved coordinates if requested or we are using tilt
	if (curve_coords_on)
		if ((HSM_CURVATURE_MODE > 0) && ((HSM_CURVATURE_3D_TILT_ANGLE_X != 0) || (HSM_CURVATURE_3D_TILT_ANGLE_Y != 0)))
			bezel_outside_curved_coord = HSM_GetCurvedCoord(bezel_outside_curved_coord, 0, bezel_outer_scale_offset.x * output_aspect / bezel_outer_scale_offset.y);

	vec2 black_edge_scale_offset = tube_scale / tube_diffuse_scale;

	bezel_outside_scale.xy = tube_diffuse_scale * black_edge_scale_offset * bezel_outer_scale_offset;
	// bezel_outside_transform.zw = -vec2(HSM_SCREEN_POSITION_X, HSM_SCREEN_POSITION_Y) + bezel_outer_pos_offset * tube_diffuse_scale;

	bezel_outside_coord = HSM_GetInverseScaledCoord(bezel_outside_coord, black_edge_scale_offset * bezel_outer_scale_offset) + vec2(0, HSM_BZL_OUTER_POSITION_Y);
	bezel_outside_curved_coord = HSM_GetInverseScaledCoord(bezel_outside_curved_coord, black_edge_scale_offset * bezel_outer_scale_offset) + vec2(0, HSM_BZL_OUTER_POSITION_Y);

	frame_outside_curved_coord = (bezel_outside_curved_coord + 
							vec2(0, HSM_FRM_OUTER_POS_Y) - 0.5) / 
							vec2((HSM_FRM_THICKNESS * HSM_FRM_THICKNESS_SCALE_X) / 
							(tube_diffuse_scale.x / tube_diffuse_scale.y * output_aspect) + 1, 
							HSM_FRM_THICKNESS + 1) + 0.5;

	if (HSM_BZL_OUTER_CURVATURE_SCALE > 0)
	{
		if (curve_coords_on)
			bezel_outside_curved_coord = HSM_GetTubeCurvedCoord(bezel_outside_curved_coord, 
																HSM_BZL_OUTER_CURVATURE_SCALE, 
																tube_diffuse_scale, 
																tube_scale, 
																bezel_outer_scale_offset.x * global.OutputSize.x / global.OutputSize.y / bezel_outer_scale_offset.y, 
																0);

		if (curve_coords_on)
			if ( HSM_FRM_OUTER_CURVATURE_SCALE > 0)
				frame_outside_curved_coord = HSM_GetTubeCurvedCoord(frame_outside_curved_coord, 
																	HSM_BZL_OUTER_CURVATURE_SCALE * HSM_FRM_OUTER_CURVATURE_SCALE, 
																	tube_diffuse_scale, 
																	tube_scale, 
																	bezel_outer_scale_offset.x * global.OutputSize.x / global.OutputSize.y / bezel_outer_scale_offset.y, 
																	0);
	}
	
	DEFAULT_SCREEN_SCALE = GetDefaultScreenScale();
	DEFAULT_BEZEL_SCALE = GetDefaultBezelScale();

	return 0;
}

vec2 HSM_GetLayerCoord(vec2 in_viewport_coord, float layer_to_follow, float follow_mode, inout vec2 out_placement_scale)
{
	vec2 flat_coord = vec2(0.5);
	vec2 curved_coord = vec2(0.5);
	vec2 out_coord = vec2(0.5);

	if (layer_to_follow == FOLLOW_LAYER_VIEWPORT)
	{
		flat_coord = in_viewport_coord;
		curved_coord = in_viewport_coord;
	}
	else if (layer_to_follow == FOLLOW_LAYER_TUBE_DIFFUSE)
	{
		flat_coord = TUBE_DIFFUSE_COORD;
		curved_coord = TUBE_DIFFUSE_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_BEZEL_OUTSIDE)
	{
		flat_coord = BEZEL_OUTSIDE_COORD;
		curved_coord = BEZEL_OUTSIDE_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_BG)
	{
		flat_coord = BACKGROUND_COORD;
		curved_coord = BACKGROUND_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_DEVICE)
	{
		flat_coord = DEVICE_COORD;
		curved_coord = DEVICE_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_DECAL)
	{
		flat_coord = DECAL_COORD;
		curved_coord = DECAL_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_CAB_GLASS)
	{
		flat_coord = CAB_GLASS_COORD;
		curved_coord = CAB_GLASS_CURVED_COORD;
	}
	else if (layer_to_follow == FOLLOW_LAYER_TOP)
	{
		flat_coord = TOP_IMAGE_COORD;
		curved_coord = TOP_IMAGE_CURVED_COORD;
	}

	out_coord = follow_mode == FOLLOW_MODE_EXACT ? curved_coord : flat_coord;

	return  out_coord;
}

// Takes a viewport coordinate and gives a new coordinate scaled by the specific scale mode
// Takes into account the default sizes of each scale mode
vec2 HSM_GetScaledCoord(vec2 in_viewport_coord,
						vec2 in_viewport_coord_unscaled,
						float texture_aspect_mode,
						float explicit_texture_aspect,
						vec2 offset_pos,
						vec2 offset_scale,
						float layer_to_follow,
						float follow_mode,
						float scale_full_with_zoom,
						float image_fill_mode,
						float split_preserve_center,
						float split_repeat_width,
						bool apply_default_scale_offset,
						// float image_rotation_mode,
						// float explicit_image_rotation,
						inout vec2 out_placement_coord,
						inout vec2 out_placement_scale)
{
	float texture_aspect = HSM_GetAspectRatioFromMode(texture_aspect_mode, explicit_texture_aspect);

	/*
	bool is_rotated = false;
	// float image_rotation_mode = 1; // Modes, 0: explicit, 1: Rotate To Vertical Core Aspect, 2: Rotate To Vertical Viewport Aspect
	// // vert core aspect rotation, vert vertical aspect rotation
	// // rotate with tube ?
	float explicit_image_rotation = 0;
	float image_rotation = explicit_image_rotation;

	// if (image_rotation_mode == 1.0)
	// 	image_rotation = SCREEN_ASPECT < 1 ? 1 : 0;
	
	if (min(abs(image_rotation), 1) > 0.5)
	{
		texture_aspect = 1 / texture_aspect;
		offset_pos = vec2(offset_pos.y, -offset_pos.x);
		offset_scale = offset_scale.yx;
		is_rotated = true;
	}
	*/

	vec2 inherited_coord = in_viewport_coord / 0.5;
	vec2 inherited_placement_coord = in_viewport_coord / 0.5;
	vec2 inherited_final_coord = in_viewport_coord / 0.5;
	vec2 inherited_scale = vec2(0.5);
	vec2 default_offset_scale = vec2(0.5);

	if (layer_to_follow == FOLLOW_LAYER_VIEWPORT)
	{
		if (scale_full_with_zoom > 0.5)	
		{
			inherited_coord = in_viewport_coord;
			inherited_placement_coord = in_viewport_coord;
		}
		else
		{
			inherited_coord = in_viewport_coord_unscaled;
			inherited_placement_coord = in_viewport_coord_unscaled;
		}

		inherited_final_coord = inherited_coord;

		inherited_scale = vec2(1, 1);
		default_offset_scale = vec2(1);
	}
	else if (layer_to_follow == FOLLOW_LAYER_TUBE_DIFFUSE)
	{
	/*
		if (is_rotated)
		{
			inherited_coord = TUBE_DIFFUSE_COORD;
			inherited_placement_coord = TUBE_DIFFUSE_COORD;
			inherited_final_coord = TUBE_DIFFUSE_CURVED_COORD;

			inherited_scale = TUBE_DIFFUSE_SCALE;

			default_offset_scale = vec2(1) / DEFAULT_UNCORRECTED_SCREEN_SCALE.y;
			default_offset_scale.x *= texture_aspect / (1 / DEFAULT_SCREEN_ASPECT);
		}
		else
		{
	*/
			inherited_coord = TUBE_DIFFUSE_COORD;
			inherited_placement_coord = TUBE_DIFFUSE_COORD;
			inherited_final_coord = TUBE_DIFFUSE_CURVED_COORD;

			inherited_scale = TUBE_DIFFUSE_SCALE;

			default_offset_scale = vec2(1) / (DEFAULT_UNCORRECTED_SCREEN_SCALE.y);
			default_offset_scale.x *= texture_aspect / DEFAULT_SCREEN_ASPECT;
		// }

	}
	else if (layer_to_follow == FOLLOW_LAYER_BEZEL_OUTSIDE)
	{
	/*
		if (is_rotated)
		{
			inherited_coord = BEZEL_OUTSIDE_COORD;
			inherited_placement_coord = BEZEL_OUTSIDE_COORD;

			inherited_scale = BEZEL_OUTSIDE_SCALE;
			inherited_final_coord = BEZEL_OUTSIDE_CURVED_COORD;

			default_offset_scale = vec2(1) / DEFAULT_UNCORRECTED_BEZEL_SCALE.y;
			default_offset_scale.x *= texture_aspect / (1 / DEFAULT_BEZEL_ASPECT);
		}
		else
		{
	*/
			inherited_coord = BEZEL_OUTSIDE_COORD;
			inherited_placement_coord = BEZEL_OUTSIDE_COORD;

			inherited_scale = BEZEL_OUTSIDE_SCALE;
			inherited_final_coord = BEZEL_OUTSIDE_CURVED_COORD;

			default_offset_scale = vec2(1) / (DEFAULT_UNCORRECTED_BEZEL_SCALE.y);
			default_offset_scale.x *= texture_aspect / DEFAULT_SCREEN_ASPECT;
		// }
	}
	else if (layer_to_follow == FOLLOW_LAYER_BG)
	{
		inherited_coord = BACKGROUND_COORD;
		inherited_placement_coord = BACKGROUND_COORD;
		inherited_final_coord = BACKGROUND_CURVED_COORD;

		inherited_scale = BACKGROUND_SCALE;
		default_offset_scale = vec2(1);
	}
	else if (layer_to_follow == FOLLOW_LAYER_DEVICE)
	{
		inherited_coord = DEVICE_COORD;
		inherited_placement_coord = DEVICE_COORD;
		inherited_final_coord = DEVICE_CURVED_COORD;

		inherited_scale = DEVICE_SCALE;
		default_offset_scale = vec2(1);
	}
	else if (layer_to_follow == FOLLOW_LAYER_DECAL)
	{
		inherited_coord = DECAL_COORD;
		inherited_placement_coord = DECAL_COORD;
		inherited_final_coord = DECAL_CURVED_COORD;

		inherited_scale = DECAL_SCALE;
		default_offset_scale = vec2(1);
	}

	//--------------------------------
	// RETURN if we want to use the exact same coordinate of the layer we follow
	//--------------------------------
	if (follow_mode == FOLLOW_MODE_EXACT)
	{
		out_placement_coord = HSM_AddPosScaleToCoord(inherited_placement_coord, offset_pos, offset_scale);
		out_placement_scale = inherited_scale * offset_scale;
		return HSM_AddPosScaleToCoord(inherited_final_coord, offset_pos, offset_scale);
	}

	if (apply_default_scale_offset)
	{
		offset_scale *= default_offset_scale;
	}

	float output_aspect = global.OutputSize.x / global.OutputSize.y;
	float inherited_aspect = (inherited_scale.x / inherited_scale.y) * (default_offset_scale.x / default_offset_scale.y) * output_aspect;
	
	// Find the aspect difference so the image can be shown without distortion
	// This is before the user edited scale offset
	float inherited_aspect_difference = texture_aspect / inherited_aspect;

	// Get the overall scale for the placement of the texture (No Split/Fill Yet)
	out_placement_scale = inherited_scale;
	if ( image_fill_mode == FILL_MODE_KEEP_TEXTURE_ASPECT )
		out_placement_scale.x *= inherited_aspect_difference;
	out_placement_scale = out_placement_scale * offset_scale;

	// inherited_coord = out_placement_coord;
	out_placement_coord = HSM_AddPosScaleToCoord(inherited_placement_coord, offset_pos, out_placement_scale / inherited_scale);

	vec2 out_coord = vec2(0.5);

	vec2 drawing_scale = out_placement_scale;
	float slide_x = 0;

	if ( image_fill_mode == FILL_MODE_SPLIT )
	{
		float abs_ctr_coord_x = abs(out_placement_coord.x - 0.5);
		// Correct the aspect so it matches the texture and is never stretched
		float placement_aspect = out_placement_scale.x / out_placement_scale.y * output_aspect;
		float placement_aspect_difference = texture_aspect / placement_aspect;
		drawing_scale.x *= placement_aspect_difference;

		float center_width = split_preserve_center * placement_aspect_difference;
		if ( abs_ctr_coord_x > center_width)
		{
			slide_x = ((placement_aspect - texture_aspect) / placement_aspect) / 2.0;
		}

		float repeat_width = split_repeat_width * placement_aspect_difference;
		if (abs_ctr_coord_x > center_width && 
			abs_ctr_coord_x < center_width + slide_x && 
			repeat_width > 0)
		{
			if (clamp(split_repeat_width - 0.001, 0, 1) == 0)
				slide_x = (abs_ctr_coord_x - center_width);
			else
				slide_x = (abs_ctr_coord_x - 0.001 - center_width) - mod(clamp(abs_ctr_coord_x - 0.01 - center_width, 0, 1), repeat_width);
		}

		if ( out_placement_coord.x < 0.5 )
			slide_x *= -1;
		inherited_coord.x -= slide_x;
	}

	// The inherited_coord is already the coord from the inherited space
	// We only need to apply an offset from this
	out_coord = HSM_AddPosScaleToCoord(inherited_coord, offset_pos, drawing_scale / inherited_scale);

	/*
	if (is_rotated)
		out_coord = HSM_RotateCoordinate(out_coord, image_rotation);
	*/

	return out_coord;
}

float MAX_LAYER_ORDER = 12;
