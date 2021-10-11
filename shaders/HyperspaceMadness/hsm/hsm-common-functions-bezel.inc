/*   
hsm-screen-scale-params-functions

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL
*/

vec2 HSM_GetOuterBezelScale(vec2 screen_scale, float screen_aspect)
{
	vec2 bezel_outer_scale_offset = vec2(HSM_BZL_WIDTH / screen_aspect + 1, HSM_BZL_HEIGHT + 1);
	return bezel_outer_scale_offset;
}

// vec2 HSM_GetFrameScale(vec2 screen_scale, vec2 tube_scale, float screen_aspect)
// {
// 	vec2 bezel_outer_scale_offset = HSM_GetOuterBezelScale(screen_scale, screen_aspect);
// 	vec2 black_edge_scale_offset = tube_scale / screen_scale;
// 	vec2 frame_scale = screen_scale * black_edge_scale_offset * bezel_outer_scale_offset;
// 	return frame_scale;
// }

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

float HSM_GetBezelCoords(vec2 screen_coord, 
							vec2 screen_scale, 
							vec2 tube_scale, 
							float screen_aspect, 
							bool curve_coords_on,
							inout vec2 bezel_outside_scale,
							inout vec2 bezel_outside_coord, 
							inout vec2 bezel_outside_curved_coord, 
							inout vec2 frame_outside_coord)
{
	float output_aspect = global.OutputSize.x / global.OutputSize.y;

	vec2 bezel_outer_pos_offset = vec2(0, HSM_BZL_OUTER_POSITION_Y);
	vec2 bezel_outer_scale_offset = HSM_GetOuterBezelScale(screen_scale, screen_aspect);

	bezel_outside_coord = screen_coord + bezel_outer_pos_offset;
	bezel_outside_curved_coord = bezel_outside_coord;

	// Only run curved coordinates if requested or we are using tilt
	if (curve_coords_on)
		if ((HSM_CURVATURE_MODE > 0) && ((HSM_CURVATURE_3D_TILT_ANGLE_X != 0) || (HSM_CURVATURE_3D_TILT_ANGLE_Y != 0)))
			bezel_outside_curved_coord = HSM_GetCurvedCoord(bezel_outside_curved_coord, 0, bezel_outer_scale_offset.x * output_aspect / bezel_outer_scale_offset.y);

	vec2 black_edge_scale_offset = tube_scale / screen_scale;

	bezel_outside_scale.xy = screen_scale * black_edge_scale_offset * bezel_outer_scale_offset;
	// bezel_outside_transform.zw = -vec2(HSM_SCREEN_POSITION_X, HSM_SCREEN_POSITION_Y) + bezel_outer_pos_offset * screen_scale;

	bezel_outside_coord = HSM_GetInverseScaledCoord(bezel_outside_curved_coord, black_edge_scale_offset * bezel_outer_scale_offset) + vec2(0, HSM_BZL_OUTER_POSITION_Y);
	bezel_outside_curved_coord = HSM_GetInverseScaledCoord(bezel_outside_curved_coord, black_edge_scale_offset * bezel_outer_scale_offset) + vec2(0, HSM_BZL_OUTER_POSITION_Y);

	frame_outside_coord = (bezel_outside_curved_coord + 
							vec2(0, HSM_FRM_OUTER_POS_Y) - 0.5) / 
							vec2((HSM_FRM_THICKNESS * HSM_FRM_THICKNESS_SCALE_X) / 
							(screen_scale.x / screen_scale.y * output_aspect) + 1, 
							HSM_FRM_THICKNESS + 1) + 0.5;

	if (HSM_BZL_OUTER_CURVATURE_SCALE > 0)
	{
		if (curve_coords_on)
			bezel_outside_curved_coord = HSM_GetTubeCurvedCoord(bezel_outside_curved_coord, HSM_BZL_OUTER_CURVATURE_SCALE, screen_scale, tube_scale, bezel_outer_scale_offset.x * global.OutputSize.x / global.OutputSize.y / bezel_outer_scale_offset.y, 0);

		if (curve_coords_on)
			if ( HSM_FRM_OUTER_CURVATURE_SCALE > 0)
				frame_outside_coord = HSM_GetTubeCurvedCoord(frame_outside_coord, HSM_BZL_OUTER_CURVATURE_SCALE * HSM_FRM_OUTER_CURVATURE_SCALE, screen_scale, tube_scale, bezel_outer_scale_offset.x * global.OutputSize.x / global.OutputSize.y / bezel_outer_scale_offset.y, 0);
	}
	
	DEFAULT_SCREEN_SCALE = GetDefaultScreenScale();
	DEFAULT_BEZEL_SCALE = GetDefaultBezelScale();

	return 0;
}

// vec4 HSM_AddTransform(vec4 in_transform_base, vec4 in_transform_add)
// {
// 	vec4 out_transform = in_transform_base;
// 	out_transform.xy *= in_transform_add.xy;
// 	// Multiply the second position because it should be scaled by the base transform
// 	out_transform.zw += in_transform_base.xy * in_transform_add.zw;
// 	return out_transform;
// }

// Change Transforms for Scale

// Applies the position before scaling from the center
// Should allow having an image where the center of the tube in the graphic is off center
// the position offset moves it so the screen is centered, then the graphic is scaled from the center 
vec2 HSM_AddPosScaleToCoord(vec2 in_base_coord, vec2 in_base_scale, vec2 in_pos, vec2 in_scale)
{
	// out_coord += in_base_scale * in_transform.zw;
	vec2 positioned_coord = in_base_coord + in_pos;
	vec2 out_coord = HSM_GetInverseScaledCoord(positioned_coord, in_scale);
	return out_coord;
}

float MAX_LAYER_ORDER = 12;

// Takes a viewport coordinate and gives a new coordinate scaled by the specific scale mode
// Takes into account the default sizes of each scale mode
vec2 HSM_GetScaledCoord(vec2 in_viewport_coord,
						float texture_aspect_mode, 
						float explicit_texture_aspect, 
						vec2 offset_pos,
						vec2 offset_scale,
						float inherited_scale_mode, 
						float image_aspect_mode,
						float image_fill_mode,
						bool apply_default_scale_offset,
						float use_inherited_curved_coord,
						inout vec2 out_placement_coord,
						inout vec2 out_placement_scale)
{
	explicit_texture_aspect = HSM_GetAspectRatioFromMode(texture_aspect_mode, explicit_texture_aspect);

	vec2 inherited_coord = in_viewport_coord / 0.5;
	vec2 inherited_placement_coord = in_viewport_coord / 0.5;
	vec2 inherited_scale = vec2(0.5);
	vec2 default_offset_scale = vec2(0.5);

	if (inherited_scale_mode == INHERITED_SCALE_MODE_VIEWPORT)
	{
		inherited_coord = in_viewport_coord;
		inherited_placement_coord = in_viewport_coord;
		inherited_scale = vec2(1, 1);
		default_offset_scale = vec2(1);
	}
	else if (inherited_scale_mode == INHERITED_SCALE_MODE_SCREEN)
	{
		inherited_scale = SCREEN_SCALE;
		inherited_coord = use_inherited_curved_coord == 1 ? SCREEN_BLACK_EDGE_CURVED_COORD : SCREEN_COORD;
		inherited_placement_coord = SCREEN_COORD;

		default_offset_scale = vec2(1) / DEFAULT_UNCORRECTED_SCREEN_SCALE.y;
		default_offset_scale.x *= explicit_texture_aspect / DEFAULT_SCREEN_ASPECT;

	}
	else if (inherited_scale_mode == INHERITED_SCALE_MODE_BEZEL_OUTSIDE)
	{
		inherited_scale = BEZEL_OUTSIDE_SCALE;
		inherited_coord = use_inherited_curved_coord == 1 ? BEZEL_OUTSIDE_CURVED_COORD : BEZEL_OUTSIDE_COORD;
		inherited_placement_coord = BEZEL_OUTSIDE_COORD;

		default_offset_scale = vec2(1) / DEFAULT_UNCORRECTED_BEZEL_SCALE.y;
		default_offset_scale.x *= explicit_texture_aspect / DEFAULT_BEZEL_ASPECT;
	}
	else if (inherited_scale_mode == INHERITED_SCALE_MODE_BG)
	{
		inherited_coord = use_inherited_curved_coord == 1 ? BACKGROUND_CURVED_COORD : BACKGROUND_COORD;
		inherited_placement_coord = BACKGROUND_COORD;
		inherited_scale = BACKGROUND_SCALE;
		default_offset_scale = vec2(1);
	}
	else if (inherited_scale_mode == INHERITED_SCALE_MODE_DEVICE)
	{
		inherited_coord = use_inherited_curved_coord == 1 ? DEVICE_CURVED_COORD : DEVICE_COORD;
		inherited_placement_coord = DEVICE_COORD;
		inherited_scale = DEVICE_SCALE;
		default_offset_scale = vec2(1);
	}
	else if (inherited_scale_mode == INHERITED_SCALE_MODE_DECAL)
	{
		inherited_coord = use_inherited_curved_coord == 1 ? DECAL_CURVED_COORD : DECAL_COORD;
		inherited_placement_coord = DECAL_COORD;
		inherited_scale = DECAL_SCALE;
		default_offset_scale = vec2(1);
	}

	if (apply_default_scale_offset)
	{
		offset_scale *= default_offset_scale;
	}

	float output_aspect = global.OutputSize.x / global.OutputSize.y;
	float inherited_aspect = (inherited_scale.x / inherited_scale.y) * (default_offset_scale.x / default_offset_scale.y) * output_aspect;
	
	// Find the aspect difference so the image can be shown without distortion
	// This is before the user edited scale offset
	float inherited_aspect_difference = explicit_texture_aspect / inherited_aspect;

	// Get the overall scale for the placement of the texture (No Split/Fill Yet)
	out_placement_scale = inherited_scale;
	if ( image_aspect_mode == IMAGE_ASPECT_MODE_KEEP_ASPECT )
		out_placement_scale.x *= inherited_aspect_difference;
	out_placement_scale = out_placement_scale * offset_scale;

	out_placement_coord = HSM_AddPosScaleToCoord(inherited_placement_coord, inherited_scale, offset_pos, out_placement_scale / inherited_scale);

	vec2 out_coord = vec2(0.5);

	if (use_inherited_curved_coord == 1)
	{
		out_coord = HSM_AddPosScaleToCoord(inherited_coord, inherited_scale, offset_pos, offset_scale);
	}
	else
	{
		vec2 drawing_scale = out_placement_scale;
		float slide_x = 0;

		if ( image_fill_mode == FILL_MODE_SPLIT_FILL_HORIZONTAL )
		{
			// Correct the aspect so it matches the texture and is never stretched
			float placement_aspect = out_placement_scale.x / out_placement_scale.y * output_aspect;
			float placement_aspect_difference = explicit_texture_aspect / placement_aspect;
			drawing_scale.x *= placement_aspect_difference;

			slide_x = (out_placement_scale.x - drawing_scale.x) / 2;// * out_placement_scale.x; 

			if ( out_placement_coord.x < 0.5 )
				slide_x *= -1;

			inherited_coord.x -= slide_x;
		}

		// The inherited_coord is already the coord from the inherited space
		// We only need to apply an offset from this
		out_coord = HSM_AddPosScaleToCoord(inherited_coord, inherited_scale, offset_pos, drawing_scale / inherited_scale);
		// out_coord.x -= slide_x;
	}

	return out_coord;
}