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

vec4 GetBezelLayer( vec4 frame_highlight_color, inout float static_shadow_multiply, inout vec4 tube_bg_layer)
{
	vec4 bezel_layer = vec4(0);

	TUBE_DIFFUSE_CURVED_COORD = HSM_GetCurvedCoord(SCREEN_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, TUBE_DIFFUSE_ASPECT);

	// TODO this should probably just use TUBE_COORD
	// TODO should probably use TUBE_ASPECT
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);
	vec2 tube_curved_coord_ctr = tube_curved_coord - 0.5;
	vec2 edge_mask_coord = tube_curved_coord_ctr * (1 - (HSM_BZL_INNER_EDGE_THICKNESS / vec2(TUBE_DIFFUSE_ASPECT, 1))) + 0.5;

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;
	
	float edge_mask =  HSM_GetCornerMask(edge_mask_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, HSM_BZL_INNER_EDGE_SHARPNESS);

	TUBE_MASK = HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);

	// Shrink the mask by 0.001 to clip off outer edge
	TUBE_DIFFUSE_MASK = HSM_GetCornerMask(((TUBE_DIFFUSE_CURVED_COORD - 0.5) * 1.001) + 0.5, TUBE_DIFFUSE_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE, HSM_TUBE_BLACK_EDGE_SHARPNESS);

	//----------------------------------------------------
	//  Calculate Outside mapping Coords
	//----------------------------------------------------

	/* This first big chunk is to get a mapping of the space outside of the screen which is continuous
	This is more complicated than you would expect because since we are using curved coordinates 
	there are discontinuities outside the normal screen corners, e.g. where x > 1 and y > 1
	So instead of trying to use the coordinates from the screen/tube we use a larger space 
	and subtract the screen space to see how far we are outside of the sreen
	*/

	// Additional scale to be applied to the tube scale to create an expanded mapping area 
	float outermap_scale = 2.3;

	// Get a range width from the outer tube edge to the outer edge of the outermap
	float outermap_range = 0.5 * outermap_scale * 0.7;
	vec2 outermap_warped_outside_screen_vector = (tube_curved_coord_ctr - clamp(tube_curved_coord_ctr, -0.490, 0.490)) * vec2(1 / TUBE_DIFFUSE_ASPECT, 1);
	float output_aspect = global.OutputSize.x / global.OutputSize.y;
	vec2 outermap_screen_corner_ctr_coord = vec2(0.5, -0.5);

	// Get a coordinate offset so it is centered around the corner
	vec2 outermap_coord_warped_ctr_at_screen_corner = abs(tube_curved_coord_ctr) - vec2(0.5);

	// ---------------------------------------------------------------------------------------
	// Generate Color with Shading for Bezel & Frame
	// ---------------------------------------------------------------------------------------
	vec3 bezel_and_frame_rgb = vec3(0);

	// #ifdef LAYERS_UNDER_CRT
	//----------------------------------------------------
	//  Calculate Corner Highlight Mask
	//----------------------------------------------------
	const float pi = 3.1415;

	// Get amount to shift the point at the outer corner to match the overall position offset
	vec2 pos_shift_offset = vec2(0, HSM_BZL_OUTER_POSITION_Y) * TUBE_DIFFUSE_SCALE.y / outermap_scale;
	pos_shift_offset *= tube_curved_coord.y > 0.5 ? 1 : -1;

	// Get the direction vector from the inner corner of the bezel pointing at the outer corner 
	vec2 corner_crease_dir = (outermap_screen_corner_ctr_coord + pos_shift_offset) / vec2(HSM_BZL_HEIGHT + 1, HSM_BZL_WIDTH + 1) - (outermap_screen_corner_ctr_coord) ;
	corner_crease_dir *= vec2(TUBE_DIFFUSE_ASPECT, 1);

	float aspect_corner_length_scale_offset = TUBE_DIFFUSE_ASPECT > 1 ? 0.9 : 1.5;
	float corner_crease_length = length(corner_crease_dir * aspect_corner_length_scale_offset);

	// A hack to adjust the angle offset, because without it the corner angle isn't pointing exactly at the corner
	// This offset is the opposite direction for vertical and horizontal aspect ratio
	float corner_rotation_offset = (SCREEN_COORD.y < 0.5) ? -HSM_REFLECT_CORNER_ROTATION_OFFSET_TOP : -HSM_REFLECT_CORNER_ROTATION_OFFSET_BOTTOM;

	if (HSM_CURVATURE_MODE == 0)
		// If we are using a 3d Curvature no offset is necessary
		corner_rotation_offset += (TUBE_DIFFUSE_ASPECT > 1) ? 2 : 3;

	// Convert direction vector to an angle so we can rotate the corner crease direction
	float corner_angle_degrees = atan(corner_crease_dir.y / corner_crease_dir.x) / (2 * pi) * 360;

	corner_angle_degrees += corner_rotation_offset;
	float corner_angle_radians = corner_angle_degrees / 360 * 2 * pi;
	corner_crease_dir = vec2(cos(corner_angle_radians), sin(corner_angle_radians));

	// Get the distance perpendicular to the crease direction so we can use it to fade later
	float distance_from_crease = HHLP_GetDistanceToLine(outermap_coord_warped_ctr_at_screen_corner.x, outermap_coord_warped_ctr_at_screen_corner.y, 1, corner_crease_dir.y / corner_crease_dir.x, 0 );

	float fade_out_to_corner = HHLP_QuadraticBezier(clamp(length(outermap_warped_outside_screen_vector) / (corner_crease_length * 2), 0, 1), vec2(0.5, HSM_REFLECT_CORNER_SPREAD_FALLOFF / 100));

	float corner_fade_width_inner = HSM_REFLECT_CORNER_INNER_SPREAD * (TUBE_DIFFUSE_SCALE.x + TUBE_DIFFUSE_SCALE.y) * bezel_corner_radius / 10 / 250 * 1.2;
	float corner_fade_width_outer = HSM_REFLECT_CORNER_OUTER_SPREAD * (TUBE_DIFFUSE_SCALE.x + TUBE_DIFFUSE_SCALE.y) * HSM_GLOBAL_CORNER_RADIUS * HSM_BZL_OUTER_CORNER_RADIUS_SCALE / 10 / 250 * 1.6;
	float corner_fade_width = (corner_fade_width_inner + fade_out_to_corner * (corner_fade_width_outer - corner_fade_width_inner));

	// Get a vector perpendicular to the crease that we can shift the crease to blend between bottom/top and sides
	vec2 corner_crease_perp_dir = normalize(vec2(corner_crease_dir.y, corner_crease_dir.x));
	vec2 corner_coord_shifted = outermap_coord_warped_ctr_at_screen_corner - corner_crease_perp_dir * corner_fade_width / 2;
	vec2 corner_crease_dir_shifted = corner_crease_dir - corner_crease_perp_dir * corner_fade_width / 2;

	// Get the distance to this shifted crease
	float distance_from_crease_shifted = HHLP_GetDistanceToLine(corner_coord_shifted.x, corner_coord_shifted.y, 1, corner_crease_dir_shifted.y / corner_crease_dir_shifted.x, 0 );

	float top_half_mask = smoothstep(0.55, 0.5, tube_curved_coord.y);
	float left_half_mask = smoothstep(0.55, 0.5, tube_curved_coord.x);

	// Get a mask which transitions between sides and top/bottom at the corner crease  
	float top_bottom_vs_sides_mask = dot(normalize(corner_coord_shifted), normalize(corner_crease_dir_shifted)) > 0 ? 1 - smoothstep(0, corner_fade_width / 2, distance_from_crease_shifted) : 1;

	// Masks isolating specific parts
	float sides_mask = 1 - top_bottom_vs_sides_mask;
	float top_mask = top_half_mask * top_bottom_vs_sides_mask;
	float bottom_mask = (1 -top_half_mask) * top_bottom_vs_sides_mask;

	float corner_mask = smoothstep(corner_fade_width / 2, 0, distance_from_crease);

	float top_corner_mask = corner_mask * top_half_mask;
	float bottom_corner_mask = corner_mask * (1 - top_half_mask);

	float frame_inner_edge_mask = (HSM_FRM_INNER_EDGE_THICKNESS == 0) ? 0 : 1 - HSM_GetCornerMask(	(BEZEL_OUTSIDE_CURVED_COORD - 0.5) * (1 + (HSM_FRM_INNER_EDGE_THICKNESS / vec2(TUBE_DIFFUSE_ASPECT, 1))) + 0.5, 
																										TUBE_DIFFUSE_ASPECT, 
																										HSM_BZL_OUTER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS, 
																										0.9);
	float outside_tube_mask_wider = 1 - HSM_GetCornerMask(tube_curved_coord_ctr * 0.996 + 0.5, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.9);
	float tube_shadow_mask = HSM_GetCornerMask(tube_curved_coord_ctr + 0.5, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0);
	float tube_edge_shadow_mult = HSM_BZL_INNER_EDGE_SHADOW * (tube_shadow_mask) + (1 - HSM_BZL_INNER_EDGE_SHADOW);

	float edge_highlight_mask = 0;

	// ----------------------------------------------------
	// Generated Bezel
	// ----------------------------------------------------

	/* This first bit is to get a mapping of the space outside of the screen which is continuous
	This is more complicated than you would expect because since we are using curved coordinates 
	there are discontinuities outside the normal screen corners, e.g. where x > 1 and y > 1
	So instead of trying to use the coordinates from the screen/tube we use a larger space 
	and subtract the screen space to see how far we are outside of the sreen
	*/

	float hmbz_bezel_highlight_edge = 0.9;
	float hmbz_bezel_highlight_top = 0.2;
	float hmbz_bezel_highlight_bottom = 0.3;
	float hmbz_bezel_highlight_sides = 0.2;
	
	float hmbz_bezel_highlight_falloff_speed = 0.5;
	float hmbz_bezel_highlight_width = 0.25;

	float hmbz_bezel_edge_highlight_width = 0.8;
	
	float hmbz_bezel_brightness_frame_outer_edge = 0.0;
	float hmbz_brightness_shadow = 0;
	float hmbz_frame_brightness = 100;

	// Not sure why we need linearize this but it seems to have a smoother range this way
	vec3 base_color = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), DEFAULT_SRGB_GAMMA).rgb;
	float noise_mask = clamp(fract(sin(dot(tube_curved_coord_ctr + vec2(0.5, 0.5) + 1, vec2(12.9898, 78.233))) * 43758.5453), 0, 1);
	vec3 base_color_with_noise = mix(base_color, 1.5 * base_color * noise_mask, HSM_BZL_NOISE);
	vec3 top_color = HSM_BZL_BRIGHTNESS_MULT_TOP * HSM_BZL_BRIGHTNESS * base_color_with_noise;
	vec3 bottom_color = HSM_BZL_BRIGHTNESS_MULT_BOTTOM * HSM_BZL_BRIGHTNESS * base_color_with_noise;
	vec3 sides_color = mix(HSM_BZL_BRIGHTNESS_MULT_SIDES * HSM_BZL_BRIGHTNESS_MULT_SIDE_RIGHT * HSM_BZL_BRIGHTNESS * base_color_with_noise,
							HSM_BZL_BRIGHTNESS_MULT_SIDES * HSM_BZL_BRIGHTNESS_MULT_SIDE_LEFT * HSM_BZL_BRIGHTNESS * base_color_with_noise,
							left_half_mask);

	vec3 frame_base_color = base_color;
	vec3 frame_base_color_with_noise = base_color_with_noise;
	if (HSM_FRM_USE_INDEPENDENT_COLOR > 0)
	{
		frame_base_color = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_FRM_COLOR_HUE, HSM_FRM_COLOR_SATURATION, HSM_FRM_COLOR_VALUE)), 1), DEFAULT_SRGB_GAMMA).rgb;
		frame_base_color_with_noise = mix(frame_base_color, 1.5 * frame_base_color * noise_mask, HSM_FRM_NOISE);
	}

	vec3 frame_color = hmbz_frame_brightness / 100 * mix(frame_base_color, 1.5 * frame_base_color * noise_mask, 0.6 * HSM_FRM_NOISE);
	vec3 outside_frame_color = hmbz_brightness_shadow * base_color_with_noise;

	vec3 bezel_diffuse_color = mix(sides_color, top_color, top_mask);
	bezel_diffuse_color = mix(bezel_diffuse_color, bottom_color, bottom_mask);

	float top_center_highlight_mask 	= hmbz_bezel_highlight_top * top_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_highlight_width, 0, abs(tube_curved_coord_ctr.x)), vec2(0.5, hmbz_bezel_highlight_falloff_speed));
	float bottom_center_highlight_mask 	= hmbz_bezel_highlight_bottom * bottom_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_highlight_width, 0, abs(tube_curved_coord_ctr.x)), vec2(0.5, hmbz_bezel_highlight_falloff_speed));
	float sides_highlight_mask 			= hmbz_bezel_highlight_sides * sides_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_highlight_width, 0, abs(tube_curved_coord_ctr.y)), vec2(0.5, hmbz_bezel_highlight_falloff_speed));

	float edge_top_center_highlight_mask 		= hmbz_bezel_highlight_top * top_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_edge_highlight_width, 0, abs(tube_curved_coord_ctr.x)), vec2(0.8, 0));
	float edge_bottom_center_highlight_mask 	= hmbz_bezel_highlight_bottom * bottom_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_edge_highlight_width, 0, abs(tube_curved_coord_ctr.x)), vec2(0.8, 0));
	float edge_sides_highlight_mask 			= hmbz_bezel_highlight_sides * sides_mask * HHLP_QuadraticBezier(smoothstep(hmbz_bezel_edge_highlight_width, 0, abs(tube_curved_coord_ctr.y)), vec2(0.8, 0));

	edge_highlight_mask = hmbz_bezel_highlight_edge * edge_mask * (edge_top_center_highlight_mask + edge_bottom_center_highlight_mask + edge_sides_highlight_mask);
	edge_highlight_mask *= HSM_BZL_INNER_EDGE_HIGHLIGHT;

	// Combine all the individual highlights into one mask
	float combined_highlight_mask = (1 + 2.5 * HSM_BZL_NOISE) * (1 - noise_mask * 2.5 * HSM_BZL_NOISE) * (top_center_highlight_mask + bottom_center_highlight_mask + sides_highlight_mask);
	float bezel_highlight_multiplier = HSM_BZL_HIGHLIGHT * combined_highlight_mask + HSM_BZL_HIGHLIGHT * edge_highlight_mask;
	vec3 bezel_color = bezel_diffuse_color * (1 + 15 * bezel_highlight_multiplier) + 1 * bezel_highlight_multiplier;

	// Add the inner edge highlight on top of the bezel color which has it's own highlight
	float inner_edge_highlight_multiplier = HSM_FRM_INNER_EDGE_HIGHLIGHT + HSM_BZL_HIGHLIGHT * 10 * HSM_FRM_INNER_EDGE_HIGHLIGHT;
	vec3 frame_inner_edge_color = frame_base_color * (1 + 15 * inner_edge_highlight_multiplier) +	0.5 * inner_edge_highlight_multiplier;

	bezel_color = mix(bezel_color, frame_inner_edge_color, frame_inner_edge_mask);

	vec2 frame_outside_coord_ctr = FRAME_OUTSIDE_CURVED_COORD - 0.5;
	float dist_inside_outer_edge = min(0.50 - abs(frame_outside_coord_ctr.x), 0.50 - abs(frame_outside_coord_ctr.y));
	float frame_outer_edge_width = HSM_FRM_OUTER_EDGE_THICKNESS;
	vec3 frame_diffuse_color = mix(frame_color, 0.2 * frame_color, HSM_FRM_OUTER_EDGE_SHADING * smoothstep(frame_outer_edge_width, 0, dist_inside_outer_edge));

	float outside_frame_mask_for_shadow = 1 - HSM_GetCornerMask(frame_outside_coord_ctr * 1.01 + 0.5, TUBE_DIFFUSE_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);

	if (HSM_FRM_TEXTURE_OPACITY > 0)
	{
		// TODO need to do Mipmapping sample?
		float outside_frame_mask_for_texture = HSM_GetCornerMask(frame_outside_coord_ctr * 1.001 + 0.5, TUBE_DIFFUSE_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);
		frame_highlight_color *= outside_frame_mask_for_texture;
		frame_diffuse_color = HSM_BlendModeLayerMix(vec4(frame_diffuse_color, 1), frame_highlight_color, HSM_FRM_TEXTURE_BLEND_MODE, HSM_FRM_TEXTURE_OPACITY).rgb;
	}
	
	// Composite in color from outside the bezel
	bezel_and_frame_rgb = mix(bezel_color, frame_diffuse_color, OUTSIDE_BEZEL_MASK);

	// Create image of bezel & frame with outside of frame transparent
	vec4 bezel_and_frame_rgba = vec4(bezel_and_frame_rgb, 1);

	// Cut out Tube Area
	if (HSM_STATIC_LAYERS_GAMMA != 1)
		bezel_and_frame_rgba = HSM_ApplyGamma(bezel_and_frame_rgba, HSM_STATIC_LAYERS_GAMMA);

	// Contract the tube mask to leave a little extra black ring at the edge of the bezel
	// otherwise this will show slivers of the background otherwise
	float tube_mask_contracted = HSM_GetCornerMask((tube_curved_coord - 0.5) * 1.004 + 0.5, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	float FRAME_AND_BEZEL_MASK = (1 - tube_mask_contracted) * (1 - OUTSIDE_FRAME_MASK);

	if (HSM_BZL_OPACITY > 0 || HSM_FRM_OPACITY > 0)
		bezel_layer = clamp(bezel_and_frame_rgba * FRAME_AND_BEZEL_MASK, 0, 1);

	// Shadow of Frame onto background
	if (HSM_FRM_SHADOW_OPACITY > 0 && HSM_FRM_OPACITY > 0.001)
	{
		// Get masks on side of frame to multiply together to get a shadow around the frame
		// Get vector from the screen edge outward
		float frame_edge = 0.495;
		float outside_frame_mask_for_shadow = 1 - HSM_GetCornerMask(frame_outside_coord_ctr * 1.01 + 0.5, TUBE_DIFFUSE_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);
		float dist_outside_frame = length(clamp(abs(frame_outside_coord_ctr * 1.01) - frame_edge, 0, 1) * vec2(TUBE_DIFFUSE_ASPECT, 1));
		float frame_shadow_alpha = OUTSIDE_FRAME_MASK * HSM_FRM_SHADOW_OPACITY * outside_frame_mask_for_shadow * HHLP_QuadraticBezier(smoothstep(HSM_FRM_SHADOW_WIDTH, 0, dist_outside_frame), vec2(1, 0));
		static_shadow_multiply *= 1 - frame_shadow_alpha;

		// TODO Need to add extra dark shadow on bottom
	}

	float bezel_opacity_mult = HSM_BZL_OPACITY + OUTSIDE_BEZEL_MASK * (1 - HSM_BZL_OPACITY);
	float frame_opacity_mult = HSM_FRM_OPACITY + (1 - OUTSIDE_BEZEL_MASK) * (1 - HSM_FRM_OPACITY);

	bezel_layer *= bezel_opacity_mult * frame_opacity_mult;

	float TUBE_MASK_EXPAND = HSM_GetCornerMask((tube_curved_coord - 0.5) * 0.997 + 0.5, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);

	// Need to fill center with opaque black if tube is not transparent
	// This will matte out the Self Illuminated pass
	tube_bg_layer = vec4(0, 0, 0, TUBE_MASK_EXPAND * HSM_GetTubeOpacity());

	return bezel_layer;
}
							
void UpdateBezelAndTubeGlobalValuesAndMasks(vec2 tube_diffuse_coord, 
											vec2 tube_diffuse_scale, 
											vec2 tube_scale, 
											float tube_diffuse_aspect,
											inout vec2 bezel_outside_scale,
											inout vec2 bezel_outside_coord, 
											inout vec2 bezel_outside_curved_coord, 
											inout vec2 frame_outside_curved_coord, 
											inout float outside_bezel_mask, 
											inout float outside_frame_mask, 
											inout float inside_bezel_mask, 
											inout float bezel_mask, 
											inout float frame_mask, 
											inout float outside_tube_mask)
{
	bool curve_coords_on = true;
	HSM_GetBezelCoords(tube_diffuse_coord, 
						tube_diffuse_scale, 
						tube_scale, 
						tube_diffuse_aspect, 
						curve_coords_on,
						bezel_outside_scale,
						bezel_outside_coord, 
						bezel_outside_curved_coord, 
						frame_outside_curved_coord);

	outside_bezel_mask = 1 - HSM_GetCornerMask(bezel_outside_curved_coord, tube_diffuse_aspect, HSM_GLOBAL_CORNER_RADIUS * HSM_BZL_OUTER_CORNER_RADIUS_SCALE, 0.9);

	// Get color for the frame area outside of the bezel
	vec2 frame_outside_coord_ctr = frame_outside_curved_coord - 0.5;

	outside_frame_mask = 1 - HSM_GetCornerMask(frame_outside_coord_ctr + 0.5, tube_diffuse_aspect, HSM_FRM_OUTER_CORNER_RADIUS, 1);
	// Get masks for shadows, from frame as well as sides and top and bottom of viewport
	inside_bezel_mask = 1 - outside_bezel_mask;
	bezel_mask = inside_bezel_mask * (1 - TUBE_MASK);
	frame_mask = outside_bezel_mask * (1 - outside_frame_mask);
	// Used in the layer blend mix function
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(tube_diffuse_coord, 1, tube_diffuse_scale, tube_scale, tube_diffuse_aspect, 1);
	vec2 tube_curved_coord_ctr = tube_curved_coord - 0.5;
	vec2 edge_mask_coord = tube_curved_coord_ctr * (1 - (HSM_BZL_INNER_EDGE_THICKNESS / vec2(tube_diffuse_aspect, 1))) + 0.5;

	outside_tube_mask = 1 - HSM_GetCornerMask(tube_curved_coord_ctr * 1.003 + 0.5, tube_diffuse_aspect, HSM_FRM_OUTER_CORNER_RADIUS, 1);
}

float MAX_LAYER_ORDER = 12;
