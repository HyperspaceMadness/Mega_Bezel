/*   
hsm-image-layers

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL
Description:
	-Adds a reflection effect around the screen to imitate the reflection on the bezel of a real CRT
	-Must be used with a crt shader which integrates the hsm-mega-bezel-include.inc scaling so 
		the crt screen and reflection scale and position are synchronized

	-Retroarch Video Settings:
		-Aspect Ratio should be set to the monitor aspect ratio
		-Integer Scale should be set to OFF

Thanks to:
	Hunkerk who sparked this idea with his Bezel Reflection Test  
	Everyone on the libretro forum for helpful suggestions
*/

/////////////// IMPORTS ///////////////
#include "hsm-common-functions-bezel.inc"

// Removed because BackgroundImageSize returns vec4(0,0,0,0), this happens with all user defined textures
// layout(push_constant) uniform Push
// {
// 	vec4 BackgroundImageSize;
// } params;

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

#define IMAGE_DUALSCREEN_MODE_BOTH 0
#define IMAGE_DUALSCREEN_MODE_SCREEN_1 1
#define IMAGE_DUALSCREEN_MODE_SCREEN_2 2

float USE_INHERITED_COORD_OFF = 0;
float USE_INHERITED_COORD_ON = 1;

vec2 VIEWPORT_COORD = vec2(0.5);

/////////////// Helper Functions ///////////////

// Return the mask for the specific mode
float GetMask(float mask_mode)
{
	float mask = 	(mask_mode == MASK_MODE_ALL) ? 1 :
					(mask_mode == MASK_MODE_SCREEN) ? SCREEN_MASK :
					(mask_mode == MASK_MODE_TUBE) ? TUBE_MASK :
					(mask_mode == MASK_MODE_INSIDE_BEZEL) ? INSIDE_BEZEL_MASK :
					(mask_mode == MASK_MODE_BEZEL) ? BEZEL_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_TUBE) ? OUTSIDE_TUBE_MASK_FOR_IMAGE :
					(mask_mode == MASK_MODE_FRAME) ? FRAME_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_BEZEL) ? OUTSIDE_BEZEL_MASK :
					(mask_mode == MASK_MODE_OUTSIDE_FRAME) ? OUTSIDE_FRAME_MASK : 0.5;
	return mask;
}

// Assumes Opacity is already encoded in alpha
vec4 BlendModeMaskLayerMix(vec4 color_under, vec4 color_over, float blend_mode, float mask_mode, float cutout_mode, float dualscreen_mode, float layer_opacity)
{
	if ( blend_mode == 0 || (dualscreen_mode != IMAGE_DUALSCREEN_MODE_BOTH && dualscreen_mode != SCREEN_INDEX) )
		return color_under;
	
	float cutout_mask = 1;
	if (cutout_mode == CUTOUT_MODE_INSIDE)
		cutout_mask = CUTOUT_MASK;
	if (cutout_mode == CUTOUT_MODE_OUTSIDE)
		cutout_mask = 1 - CUTOUT_MASK;

	if (blend_mode == BLEND_MODE_OFF)
		return color_under;
	
	color_over.a *= layer_opacity * GetMask(mask_mode) * cutout_mask;

	vec4 out_color = vec4(0);

	if (blend_mode == BLEND_MODE_NORMAL)
	{
		color_over.rgb *= color_over.a;
		out_color = HSM_PreMultAlphaBlend(color_under, color_over);
	}
	else
	{
		vec4 blend_color = color_under; 
		if (blend_mode == BLEND_MODE_ADD)  	 		blend_color.rgb = color_under.rgb + color_over.rgb ;
		if (blend_mode == BLEND_MODE_MULTIPLY)  	blend_color.rgb = color_under.rgb * color_over.rgb ;

		out_color = vec4(clamp(mix(color_under.rgb, blend_color.rgb, color_over.a), 0, 1), color_under.a);
	}
	return out_color;
}

//TODO remove this and replace with simpler calls
float GetFade(float current_position, float corner_position, float fade_distance)
{
	return smoothstep(corner_position + fade_distance / 2, corner_position - fade_distance / 2, current_position);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;
layout(location = 7) out vec2 UNFLIPPED_VIEWPORT_COORD;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	UNFLIPPED_VIEWPORT_COORD = vTexCoord;

	if (HSM_FLIP_VIEWPORT_VERTICAL > 0.5)
		vTexCoord.y =  1 - vTexCoord.y;

	if (HSM_FLIP_VIEWPORT_HORIZONTAL > 0.5)
		vTexCoord.x =  1 - vTexCoord.x;

	VIEWPORT_COORD = vTexCoord * 1.0001;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;
layout(location = 7) in vec2 UNFLIPPED_VIEWPORT_COORD;
layout(location = 8) in vec3 BEZEL_FRAME_ORIGINAL_COLOR_RGB;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D InfoCachePass;

layout(set = 0, binding = 2) uniform sampler2D BackgroundImage;
layout(set = 0, binding = 3) uniform sampler2D BackgroundVertImage;
layout(set = 0, binding = 4) uniform sampler2D NightLightingImage;

layout(set = 0, binding = 5) uniform sampler2D LEDImage;
layout(set = 0, binding = 6) uniform sampler2D FrameTextureImage;
layout(set = 0, binding = 7) uniform sampler2D DeviceImage;
layout(set = 0, binding = 8) uniform sampler2D DeviceVertImage;
layout(set = 0, binding = 9) uniform sampler2D DecalImage;
layout(set = 0, binding = 10) uniform sampler2D CabinetGlassImage;
layout(set = 0, binding = 11) uniform sampler2D TopLayerImage;

#ifdef LAYERS_OVER_CRT
layout(set = 0, binding = 12) uniform sampler2D BR_LayersOverCRTPassFeedback;
#define PassFeedback BR_LayersOverCRTPassFeedback
#else
layout(set = 0, binding = 12) uniform sampler2D BR_LayersUnderCRTPassFeedback;
#define PassFeedback BR_LayersUnderCRTPassFeedback
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	if (HSM_LIVE_UPDATE_ON < 0.5 && feedback_color_test.a > 0.001)
	{
		FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
		return;
	}

	//-----------------------------------------------------------------------------------------
	// Night Lighting Image
	//-----------------------------------------------------------------------------------------
	// Add Multiplied Image for Darkening
	// Sample the image and do some color correction
	vec4 night_lighting_image = vec4(1);
	if (HSM_NIGHT_OPACITY > 0)
		night_lighting_image = HSM_GetNightLightingMultiplyColor( VIEWPORT_COORD, NightLightingImage );

	VIEWPORT_COORD = vTexCoord;

	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, VIEWPORT_COORD);


	SCREEN_BLACK_EDGE_CURVED_COORD = HSM_GetCurvedCoord(SCREEN_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, SCREEN_ASPECT);

	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);
	vec2 tube_curved_coord_ctr = tube_curved_coord - 0.5;
	vec2 edge_mask_coord = tube_curved_coord_ctr * (1 - (HSM_BZL_INNER_EDGE_THICKNESS / vec2(SCREEN_ASPECT, 1))) + 0.5;

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;
	
	float edge_mask =  HSM_GetCornerMask(edge_mask_coord, SCREEN_ASPECT, bezel_corner_radius, HSM_BZL_INNER_EDGE_SHARPNESS);

	float outside_tube_mask = 1 - HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	TUBE_MASK = 1 - outside_tube_mask;

	// Shrink the mask by 0.001 to clip off outer edge
	SCREEN_MASK = HSM_GetCornerMask(((SCREEN_BLACK_EDGE_CURVED_COORD - 0.5) * 1.001) + 0.5, SCREEN_ASPECT, HSM_SCREEN_CORNER_RADIUS, HSM_SCREEN_EDGE_SHARPNESS);

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
	vec2 outermap_screen_size_from_center = vec2(0.5, 0.5);
	vec2 outermap_warped_outside_screen_vector = (tube_curved_coord_ctr - clamp(tube_curved_coord_ctr, -0.490, 0.490)) * vec2(1 / SCREEN_ASPECT, 1);
	float output_aspect = global.OutputSize.x / global.OutputSize.y;
	float outside_ratio_warped = clamp(length(outermap_warped_outside_screen_vector) / outermap_range, 0, 1);
	vec2 outermap_screen_corner_ctr_coord = vec2(0.5, -0.5);

	// Get a coordinate offset so it is centered around the corner
	vec2 outermap_coord_warped_ctr_at_screen_corner = abs(tube_curved_coord_ctr) - vec2(0.5);

	vec2 frame_outside_curved_coord = vec2(0.5, 0.5);

	// Have to get the scale of the coordinates so we can figure out the size of the onscreen rectangle of the area 
	HSM_GetBezelCoords(SCREEN_COORD, 
						SCREEN_SCALE, 
						TUBE_SCALE, 
						SCREEN_ASPECT, 
						BEZEL_OUTSIDE_SCALE,
						BEZEL_OUTSIDE_COORD, 
						BEZEL_OUTSIDE_CURVED_COORD, 
						frame_outside_curved_coord);

	OUTSIDE_BEZEL_MASK = 1 - HSM_GetCornerMask(BEZEL_OUTSIDE_CURVED_COORD, SCREEN_ASPECT, bezel_corner_radius * HSM_BZL_OUTER_CORNER_RADIUS_SCALE, 0.9);

	// Get color for the frame area outside of the bezel
	vec2 frame_outside_coord_ctr = frame_outside_curved_coord - 0.5;
	float SHADOW_OUTSIDE_FRAME_MASK = 1 - HSM_GetCornerMask(frame_outside_coord_ctr * 1.01 + 0.5, SCREEN_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);
	OUTSIDE_FRAME_MASK = 1 - HSM_GetCornerMask(frame_outside_coord_ctr + 0.5, SCREEN_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);
	OUTSIDE_FRAME_MASK_FOR_IMAGE = 1 - HSM_GetCornerMask(frame_outside_coord_ctr * 0.999 + 0.5, SCREEN_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);
	// Get masks for shadows, from frame as well as sides and top and bottom of viewport
	INSIDE_BEZEL_MASK = 1 - OUTSIDE_BEZEL_MASK;
	BEZEL_MASK = INSIDE_BEZEL_MASK * outside_tube_mask;
	FRAME_MASK = OUTSIDE_BEZEL_MASK * (1 - OUTSIDE_FRAME_MASK);

#ifdef LAYERS_UNDER_CRT
	//----------------------------------------------------
	//  Calculate Corner Highlight Mask
	//----------------------------------------------------
	const float pi = 3.1415;

	// Get amount to shift the point at the outer corner to match the overall position offset
	vec2 pos_shift_offset = vec2(0, HSM_BZL_OUTER_POSITION_Y) * SCREEN_SCALE.y / outermap_scale;
	pos_shift_offset *= tube_curved_coord.y > 0.5 ? 1 : -1;

	// Get the direction vector from the inner corner of the bezel pointing at the outer corner 
	vec2 corner_crease_dir = (outermap_screen_corner_ctr_coord + pos_shift_offset) / vec2(HSM_BZL_HEIGHT + 1, HSM_BZL_WIDTH + 1) - (outermap_screen_corner_ctr_coord) ;
	corner_crease_dir *= vec2(SCREEN_ASPECT, 1);

	float aspect_corner_length_scale_offset = SCREEN_ASPECT > 1 ? 0.9 : 1.5;
	float corner_crease_length = length(corner_crease_dir * aspect_corner_length_scale_offset);

	// A hack to adjust the angle offset, because without it the corner angle isn't pointing exactly at the corner
	// This offset is the opposite direction for vertical and horizontal aspect ratio
	float corner_rotation_offset = (SCREEN_COORD.y < 0.5) ? -HSM_REFLECT_CORNER_ROTATION_OFFSET_TOP : -HSM_REFLECT_CORNER_ROTATION_OFFSET_BOTTOM;

	if (HSM_CURVATURE_MODE == 0)
		// If we are using a 3d Curvature no offset is necessary
		corner_rotation_offset += (SCREEN_ASPECT > 1) ? 2 : 3;

	// Convert direction vector to an angle so we can rotate the corner crease direction
	float corner_angle_degrees = atan(corner_crease_dir.y / corner_crease_dir.x) / (2 * pi) * 360;

	corner_angle_degrees += corner_rotation_offset;
	float corner_angle_radians = corner_angle_degrees / 360 * 2 * pi;
	corner_crease_dir = vec2(cos(corner_angle_radians), sin(corner_angle_radians));

	// Get the distance perpendicular to the crease direction so we can use it to fade later
	float distance_from_crease = HHLP_GetDistanceToLine(outermap_coord_warped_ctr_at_screen_corner.x, outermap_coord_warped_ctr_at_screen_corner.y, 1, corner_crease_dir.y / corner_crease_dir.x, 0 );

	float fade_out_to_corner = HHLP_QuadraticBezier(clamp(length(outermap_warped_outside_screen_vector) / (corner_crease_length * 2), 0, 1), vec2(0.5, HSM_REFLECT_CORNER_SPREAD_FALLOFF / 100));

	float corner_fade_width_inner = HSM_REFLECT_CORNER_INNER_SPREAD * (SCREEN_SCALE.x + SCREEN_SCALE.y) * bezel_corner_radius / 10 / 250 * 1.2;
	float corner_fade_width_outer = HSM_REFLECT_CORNER_OUTER_SPREAD * (SCREEN_SCALE.x + SCREEN_SCALE.y) * HSM_BZL_OUTER_CORNER_RADIUS_SCALE * bezel_corner_radius / 10 / 250 * 1.6;
	float corner_fade_width = (corner_fade_width_inner + fade_out_to_corner * (corner_fade_width_outer - corner_fade_width_inner));

	// Get a vector perpendicular to the crease that we can shift the crease to blend between bottom/top and sides
	vec2 corner_crease_perp_dir = normalize(vec2(corner_crease_dir.y, corner_crease_dir.x));
	vec2 corner_coord_shifted = outermap_coord_warped_ctr_at_screen_corner - corner_crease_perp_dir * corner_fade_width / 2;
	vec2 corner_crease_dir_shifted = corner_crease_dir - corner_crease_perp_dir * corner_fade_width / 2;

	// Get the distance to this shifted crease
	float distance_from_crease_shifted = HHLP_GetDistanceToLine(corner_coord_shifted.x, corner_coord_shifted.y, 1, corner_crease_dir_shifted.y / corner_crease_dir_shifted.x, 0 );

	float top_half_mask = smoothstep(0.55, 0.5, tube_curved_coord.y);

	// Get a mask which transitions between sides and top/bottom at the corner crease  
	float top_bottom_vs_sides_mask = dot(normalize(corner_coord_shifted), normalize(corner_crease_dir_shifted)) > 0 ? 1 - smoothstep(0, corner_fade_width / 2, distance_from_crease_shifted) : 1;

	// Masks isolating specific parts
	float sides_mask = 1 - top_bottom_vs_sides_mask;
	float top_mask = top_half_mask * top_bottom_vs_sides_mask;
	float bottom_mask = (1 -top_half_mask) * top_bottom_vs_sides_mask;

	float corner_mask = smoothstep(corner_fade_width / 2, 0, distance_from_crease);

	float top_corner_mask = corner_mask * top_half_mask;
	float bottom_corner_mask = corner_mask * (1 - top_half_mask);

	float frame_inner_edge_mask = (HSM_FRM_INNER_EDGE_THICKNESS == 0) ? 0 : 1 - HSM_GetCornerMask(	(BEZEL_OUTSIDE_CURVED_COORD - 0.5) * (1 + (HSM_FRM_INNER_EDGE_THICKNESS / vec2(SCREEN_ASPECT, 1))) + 0.5, 
																										SCREEN_ASPECT, 
																										HSM_BZL_OUTER_CORNER_RADIUS_SCALE * bezel_corner_radius, 
																										0.9);
	float outside_tube_mask_wider = 1 - HSM_GetCornerMask(tube_curved_coord_ctr * 0.996 + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0.9);
	float tube_shadow_mask = HSM_GetCornerMask(tube_curved_coord_ctr + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0);
	float tube_edge_shadow_mult = HSM_BZL_INNER_EDGE_SHADOW * (tube_shadow_mask) + (1 - HSM_BZL_INNER_EDGE_SHADOW);

	// crt_linear.rgb *= tube_edge_shadow_mult * (1 - outside_tube_mask_wider);
	// crt_linear.a = (1 - outside_tube_mask_wider);

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
	float hmbz_bezel_brightness_top = 0.5;
	float hmbz_bezel_brightness_sides = 1;
	float hmbz_bezel_brightness_bottom = 2;

	float hmbz_bezel_highlight_edge = 0.9;
	float hmbz_bezel_highlight_top = 0.2;
	float hmbz_bezel_highlight_bottom = 0.3;
	float hmbz_bezel_highlight_sides = 0.2;
	
	float hmbz_bezel_highlight_falloff_speed = 0.5;
	float hmbz_bezel_highlight_width = 0.25;

	float hmbz_bezel_edge_highlight_width = 0.8;
	
	float hmbz_bezel_brightness_frame_inner_edge = 0.014;
	float hmbz_bezel_brightness_frame_outer_edge = 0.0;
	float hmbz_brightness_shadow = 0;
	float hmbz_frame_brightness = 100;

	// Not sure why we need linearize this but it seems to have a smoother range this way
	vec3 base_color = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), HSM_DEFAULT_SRGB_GAMMA).rgb;
	float noise_mask = clamp(fract(sin(dot(tube_curved_coord_ctr + vec2(0.5, 0.5) + 1, vec2(12.9898, 78.233))) * 43758.5453), 0, 1);
	vec3 base_color_with_noise = mix(base_color, 1.5 * base_color * noise_mask, HSM_BZL_NOISE);
	vec3 top_color = hmbz_bezel_brightness_top * HSM_BZL_BRIGHTNESS * base_color_with_noise;
	vec3 bottom_color = hmbz_bezel_brightness_bottom * HSM_BZL_BRIGHTNESS * base_color_with_noise;
	vec3 sides_color = hmbz_bezel_brightness_sides * HSM_BZL_BRIGHTNESS * base_color_with_noise;

	vec3 frame_color = hmbz_frame_brightness / 100 * mix(base_color, 1.5 * base_color * noise_mask, 0.6 * HSM_BZL_NOISE);;
	
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

	// Combine all the individual highlights into one mask
	float combined_highlight_mask = (1 + 2.5 * HSM_BZL_NOISE) * (1 - noise_mask * 2.5 * HSM_BZL_NOISE) * (top_center_highlight_mask + bottom_center_highlight_mask + sides_highlight_mask);
	vec3 bezel_color = bezel_diffuse_color + HSM_BZL_HIGHLIGHT * combined_highlight_mask;
	bezel_color += HSM_BZL_HIGHLIGHT * edge_highlight_mask;

	// Add the inner edge highlight on top of the bezel color which has it's own highlight
	vec3 frame_inner_edge_color = bezel_color + hmbz_bezel_brightness_frame_inner_edge + HSM_BZL_HIGHLIGHT * 10 * hmbz_bezel_brightness_frame_inner_edge;
	bezel_color = mix(bezel_color, frame_inner_edge_color, frame_inner_edge_mask);

	float dist_inside_outer_edge = min(0.5 - abs(frame_outside_coord_ctr.x), 0.5 - abs(frame_outside_coord_ctr.y));
	float frame_outer_edge_width = HSM_FRM_OUTER_EDGE_THICKNESS;
	vec3 frame_diffuse_color = mix(frame_color, 0.2 * frame_color, HSM_FRM_OUTER_EDGE_SHADING * smoothstep(frame_outer_edge_width, 0, dist_inside_outer_edge));

	if (HSM_FRM_TEXTURE_OPACITY > 0)
	{
		vec4 frame_texture_color = HSM_Linearize(texture(FrameTextureImage, frame_outside_curved_coord), HSM_DEFAULT_SRGB_GAMMA);
		frame_diffuse_color = HSM_BlendModeLayerMix(vec4(frame_diffuse_color, 1), frame_texture_color, HSM_FRM_TEXTURE_BLEND_MODE, HSM_FRM_TEXTURE_OPACITY).rgb;
	}

	frame_diffuse_color = mix(frame_diffuse_color, outside_frame_color, OUTSIDE_FRAME_MASK);

	// Composite in color from outside the bezel
	vec3 bezel_and_frame_rgb = mix(bezel_color, frame_diffuse_color, OUTSIDE_BEZEL_MASK);

	float frame_edge = 0.495;

	// Get masks on side of frame to multiply together to get a shadow around the frame
	// Get vector from the screen edge outward
	float dist_outside_frame = length(clamp(abs(frame_outside_coord_ctr * 1.01) - frame_edge, 0, 1) * vec2(SCREEN_ASPECT, 1));

	vec4 frame_shadow_layer = vec4(0);
	if (HSM_FRM_OPACITY > 0.001)
		frame_shadow_layer.a = SHADOW_OUTSIDE_FRAME_MASK * HHLP_QuadraticBezier(smoothstep(HSM_FRM_SHADOW_WIDTH, 0, dist_outside_frame), vec2(1, 0));

	//----------------------------------------------------
	// Generated Bezel
	//----------------------------------------------------

	vec4 bezel_layer = vec4(0);
	vec4 frame_layer = vec4(0);

	if (HSM_BZL_OPACITY > 0 || HSM_FRM_OPACITY > 0)
	{
		// Create image of bezel & frame with outside of frame transparent
		vec4 bezel_and_frame_rgba = vec4(bezel_and_frame_rgb * (1 - OUTSIDE_FRAME_MASK), (1 - OUTSIDE_FRAME_MASK));

		// Cut out Tube Area
		// bezel_and_frame_rgba *= 1 - TUBE_MASK;

		// // Put the frame and bezel over top a black tube
		// bezel_and_frame_rgba = HSM_PreMultAlphaBlend(vec4(0, 0, 0, 1 - outside_tube_mask_wider), bezel_and_frame_rgba);

		if (HSM_STATIC_LAYERS_GAMMA != 1)
			bezel_and_frame_rgba = HSM_ApplyGamma(bezel_and_frame_rgba, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
			if (HSM_BZL_BLEND_MODE != BLEND_MODE_ADD)
				bezel_and_frame_rgba.rgb *= night_lighting_image.rgb;

		float FRAME_AND_BEZEL_MASK = outside_tube_mask * (1 - OUTSIDE_FRAME_MASK);

		if (HSM_BZL_OPACITY > 0 )
			if (HSM_FRM_OPACITY > 0)
				bezel_layer = clamp(bezel_and_frame_rgba * FRAME_AND_BEZEL_MASK, 0, 1);
			else
				bezel_layer = clamp(bezel_and_frame_rgba * BEZEL_MASK, 0, 1);

		// bezel_layer = clamp(bezel_and_frame_rgba, 0, 1);

		if (HSM_FRM_OPACITY > 0 )
			frame_layer = clamp(bezel_and_frame_rgba * FRAME_MASK, 0, 1);
	}

// end of ifndef LAYERS_UNDER_CRT
#endif

	//-----------------------------------------------------------------------------------------
	// Background
	//-----------------------------------------------------------------------------------------
	BACKGROUND_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
										HSM_BG_TEXTURE_ASPECT_MODE,
										HSM_BG_EXPLICIT_TEXTURE_ASPECT,
										vec2(HSM_BG_POS_X, HSM_BG_POS_Y),
										vec2(HSM_BG_SCALE * HSM_BG_SCALE_X, HSM_BG_SCALE),
										HSM_BG_SCALE_MODE,
										HSM_BG_SCALE_KEEP_ASPECT,
										HSM_BG_FILL_MODE,
										true,
										USE_INHERITED_COORD_OFF,
										BACKGROUND_COORD,
										BACKGROUND_SCALE);

	if (HSM_BG_MIRROR_WRAP == 1)
		BACKGROUND_CURVED_COORD = HSM_GetMirrorWrapCoord(BACKGROUND_CURVED_COORD);

	vec4 bg_image = vec4(0);
	if (HSM_BG_OPACITY > 0)
	{
		if (SCREEN_ASPECT > 1) 
			bg_image = texture(BackgroundImage, BACKGROUND_CURVED_COORD);
		else
			bg_image = texture(BackgroundVertImage, BACKGROUND_CURVED_COORD);

		// Premultiply Alpha
		bg_image = HSM_GetPreMultipliedColorLinear(bg_image, HSM_BG_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// HSV Adjustments
		if (HSM_BG_COLORIZE_ON > 0.5 || HSM_BG_HUE != 0 || HSM_BG_SATURATION != 1 || HSM_BG_BRIGHTNESS != 1)
			bg_image.rgb = HSM_ApplyHSVAdjustment(bg_image.rgb, HSM_BG_HUE, HSM_BG_SATURATION, HSM_BG_BRIGHTNESS, HSM_BG_COLORIZE_ON);
		
		if (HSM_BG_APPLY_ADJUSTMENT == 1)
			if (HSM_STATIC_LAYERS_GAMMA != 1)
				bg_image = HSM_ApplyGamma(bg_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_BG_APPLY_ADJUSTMENT == 1)
			if (HSM_NIGHT_OPACITY > 0)
				bg_image.rgb *= night_lighting_image.rgb;
	}

	//----------------------------------------------------
	// Device Image
	//----------------------------------------------------

	DEVICE_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
											HSM_DEVICE_TEXTURE_ASPECT_MODE,
											HSM_DEVICE_EXPLICIT_TEXTURE_ASPECT,
											vec2(HSM_DEVICE_POS_X, HSM_DEVICE_POS_Y),
											vec2(HSM_DEVICE_SCALE * HSM_DEVICE_SCALE_X, HSM_DEVICE_SCALE),
											HSM_DEVICE_SCALE_MODE,
											HSM_DEVICE_SCALE_KEEP_ASPECT,
											HSM_DEVICE_FILL_MODE,
											true,
											HSM_DEVICE_COPY_INHERITED_COORD,
											DEVICE_COORD,
											DEVICE_SCALE);

	vec4 device_image = vec4(0);
	if (HSM_DEVICE_OPACITY > 0)
	{
		device_image = SCREEN_ASPECT < 1 ? texture(DeviceVertImage, DEVICE_CURVED_COORD):
										  texture(DeviceImage,     DEVICE_CURVED_COORD);

		// Premultiply Alpha
		device_image = HSM_GetPreMultipliedColorLinear(device_image, HSM_DEVICE_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// HSV Adjustments
		// if (HSM_DEVICE_COLORIZE_ON == 1 || HSM_DEVICE_HUE != 0 || HSM_DEVICE_SATURATION != 1 || HSM_DEVICE_BRIGHTNESS != 1)
			device_image.rgb = HSM_ApplyHSVAdjustment(device_image.rgb, HSM_DEVICE_HUE, HSM_DEVICE_SATURATION, HSM_DEVICE_BRIGHTNESS, HSM_DEVICE_COLORIZE_ON);

		if (HSM_STATIC_LAYERS_GAMMA != 1)
			device_image = HSM_ApplyGamma(device_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
			if (HSM_DEVICE_BLEND_MODE != BLEND_MODE_ADD)
				device_image.rgb *= night_lighting_image.rgb;
	}

	//----------------------------------------------------
	// LED Image
	//----------------------------------------------------
	vec4 led_image = vec4(0);
	if (HSM_LED_OPACITY > 0)
	{
		LED_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
											HSM_LED_TEXTURE_ASPECT_MODE, 
											HSM_LED_EXPLICIT_TEXTURE_ASPECT, 
											vec2(HSM_LED_POS_X, HSM_LED_POS_Y), 
											vec2(HSM_LED_SCALE * HSM_LED_SCALE_X, HSM_LED_SCALE),
											HSM_LED_SCALE_MODE, 
											HSM_LED_SCALE_KEEP_ASPECT, 
											HSM_LED_FILL_MODE,
											true,
											USE_INHERITED_COORD_OFF,
											LED_COORD,
											LED_SCALE);

		led_image = texture(LEDImage, LED_CURVED_COORD);

		// Premultiply Alpha
		led_image = HSM_GetPreMultipliedColorLinear(led_image, HSM_LED_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// STATIC GAMMA
		if (HSM_STATIC_LAYERS_GAMMA != 1)
			led_image = HSM_ApplyGamma(led_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
			if (HSM_LED_BLEND_MODE != BLEND_MODE_ADD)
				led_image.rgb *= night_lighting_image.rgb;
	}

	//----------------------------------------------------
	// Decal Image
	//----------------------------------------------------
	vec4 decal_image = vec4(0);
	DECAL_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
									HSM_DECAL_TEXTURE_ASPECT_MODE, 
									HSM_DECAL_EXPLICIT_TEXTURE_ASPECT, 
									vec2(HSM_DECAL_POS_X, HSM_DECAL_POS_Y), 
									vec2(HSM_DECAL_SCALE * HSM_DECAL_SCALE_X, HSM_DECAL_SCALE),
									HSM_DECAL_SCALE_MODE,
									HSM_DECAL_SCALE_KEEP_ASPECT, 
									HSM_DECAL_FILL_MODE,
									true,
									USE_INHERITED_COORD_OFF,
									DECAL_COORD,
									DECAL_SCALE);
	if (HSM_DECAL_OPACITY > 0)
	{
		decal_image = texture(DecalImage, DECAL_CURVED_COORD);
		
		// Premultiply Alpha
		decal_image = HSM_GetPreMultipliedColorLinear(decal_image, HSM_DECAL_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// Do HSV alterations
		if (HSM_DECAL_COLORIZE_ON == 1 || HSM_DECAL_HUE != 0 || HSM_DECAL_SATURATION != 1 || HSM_DECAL_BRIGHTNESS != 1)
			decal_image.rgb = HSM_ApplyHSVAdjustment(decal_image.rgb, HSM_DECAL_HUE, HSM_DECAL_SATURATION, HSM_DECAL_BRIGHTNESS, HSM_DECAL_COLORIZE_ON);

		// STATIC GAMMA
		if (HSM_STATIC_LAYERS_GAMMA != 1)
			decal_image = HSM_ApplyGamma(decal_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
			if (HSM_DECAL_BLEND_MODE != BLEND_MODE_ADD)
				decal_image.rgb *= night_lighting_image.rgb;
	}

//----------------------------------------------------
//  ADV Get Additional Layers and Composite 
//----------------------------------------------------
// #ifdef IS_ADV_PRESET
	vec4 top_image = vec4(0);
	TOP_IMAGE_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
										HSM_TOP_TEXTURE_ASPECT_MODE, 
										HSM_TOP_EXPLICIT_TEXTURE_ASPECT, 
										vec2(HSM_TOP_POS_X, HSM_TOP_POS_Y), 
										vec2(HSM_TOP_SCALE * HSM_TOP_SCALE_X, HSM_TOP_SCALE),
										HSM_TOP_SCALE_MODE, 
										HSM_TOP_SCALE_KEEP_ASPECT, 
										HSM_TOP_FILL_MODE,
										true,
										USE_INHERITED_COORD_OFF,
										TOP_IMAGE_COORD,
										TOP_IMAGE_SCALE);

	if (HSM_TOP_MIRROR_WRAP == 1)
		TOP_IMAGE_CURVED_COORD = HSM_GetMirrorWrapCoord(TOP_IMAGE_CURVED_COORD);

	if (HSM_TOP_OPACITY > 0)
	{
		// Get the top image color and masking values if needed
		top_image = texture(TopLayerImage,     TOP_IMAGE_CURVED_COORD);

		// Premultiply Alpha
		top_image = HSM_GetPreMultipliedColorLinear(top_image, HSM_TOP_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// STATIC GAMMA
		if (HSM_STATIC_LAYERS_GAMMA != 1)
			top_image = HSM_ApplyGamma(top_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
				if (HSM_TOP_BLEND_MODE != BLEND_MODE_ADD)
					top_image.rgb *= night_lighting_image.rgb;
	}

	//----------------------------------------------------
	// Cabinet Glass Image
	//----------------------------------------------------
	vec4 cab_glass_image = vec4(0);
	if (HSM_CAB_GLASS_OPACITY > 0)
	{
		CAB_GLASS_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
											HSM_CAB_GLASS_TEXTURE_ASPECT_MODE, 
											HSM_CAB_GLASS_EXPLICIT_TEXTURE_ASPECT, 
											vec2(HSM_CAB_GLASS_POS_X, HSM_CAB_GLASS_POS_Y), 
											vec2(HSM_CAB_GLASS_SCALE * HSM_CAB_GLASS_SCALE_X, HSM_CAB_GLASS_SCALE),
											HSM_CAB_GLASS_SCALE_MODE, 
											HSM_CAB_GLASS_SCALE_KEEP_ASPECT, 
											HSM_CAB_GLASS_FILL_MODE,
											true,
											USE_INHERITED_COORD_OFF,
											CAB_GLASS_COORD,
											CAB_GLASS_SCALE);

		// Add Scaling
		CAB_GLASS_CURVED_COORD = (CAB_GLASS_CURVED_COORD - 0.5) * 1 / HSM_CAB_GLASS_SCALE + 0.5;

		// Sample Texture
		cab_glass_image = texture(CabinetGlassImage, CAB_GLASS_CURVED_COORD.xy);

		// Premultiply Alpha
		cab_glass_image = HSM_GetPreMultipliedColorLinear(cab_glass_image, HSM_CAB_GLASS_SOURCE_MATTE_TYPE, HSM_DEFAULT_SRGB_GAMMA);

		// Adjust Brightness
		cab_glass_image.rgb *= HSM_CAB_GLASS_BRIGHTNESS;

		// STATIC GAMMA
		if (HSM_STATIC_LAYERS_GAMMA != 1)
			cab_glass_image = HSM_ApplyGamma(cab_glass_image, HSM_STATIC_LAYERS_GAMMA);

		// NIGHT LIGHTING
		if (HSM_NIGHT_OPACITY > 0)
			if (HSM_CAB_GLASS_APPLY_NIGHTLIGHTING > 0.5)
				if (HSM_CAB_GLASS_BLEND_MODE != BLEND_MODE_ADD)
					cab_glass_image.rgb *= night_lighting_image.rgb;
	}


	//-----------------------------------------------------------------------------------------
	// CUTOUT MASK
	//-----------------------------------------------------------------------------------------
	CUTOUT_MASK = 1;

	vec2 temp_scale = vec2(0.5);
	vec2 temp_coord = vec2(0.5);

	vec2 cutout_base_scale_offset = vec2(0.5);

	if ( HSM_CUTOUT_SCALE_MODE == INHERITED_SCALE_MODE_VIEWPORT )
		cutout_base_scale_offset *= vec2(DEFAULT_UNCORRECTED_BEZEL_SCALE.y * SCREEN_ASPECT / output_aspect, DEFAULT_BEZEL_SCALE.y);

	vec2 cutout_coord = HSM_GetScaledCoord(VIEWPORT_COORD,
										  	HSM_CUTOUT_ASPECT_MODE,
											HSM_CUTOUT_EXPLICIT_ASPECT,
											vec2(HSM_CUTOUT_POS_X, HSM_CUTOUT_POS_Y),
											vec2(vec2(HSM_CUTOUT_SCALE * HSM_CUTOUT_SCALE_X, HSM_CUTOUT_SCALE) * cutout_base_scale_offset),
											HSM_CUTOUT_SCALE_MODE,
											HSM_CUTOUT_KEEP_ASPECT,
											FILL_MODE_STRETCH,
											false,
											USE_INHERITED_COORD_OFF,
											temp_coord,
											temp_scale);
	
	CUTOUT_MASK = 1 - HSM_GetCornerMask(cutout_coord, SCREEN_ASPECT, HSM_CUTOUT_CORNER_RADIUS, 0.8);

	//-----------------------------------------------------------------------------------------
	// Full Viewport Vignette
	//-----------------------------------------------------------------------------------------
	vec4 vignette_layer = vec4(0);
	if (HSM_VIEWPORT_VIGNETTE_OPACITY > 0)
	{
		vec2 vignette_coord = HSM_GetScaledCoord(VIEWPORT_COORD,
												TEXTURE_ASPECT_MODE_VIEWPORT,
												1,
												vec2(HSM_VIEWPORT_VIGNETTE_POS_X, HSM_VIEWPORT_VIGNETTE_POS_Y), 
												vec2(vec2(HSM_VIEWPORT_VIGNETTE_SCALE * HSM_VIEWPORT_VIGNETTE_SCALE_X, HSM_VIEWPORT_VIGNETTE_SCALE)), 
												HSM_VIEWPORT_VIGNETTE_SCALE_MODE,
												INHERITED_SCALE_MODE_VIEWPORT,
												FILL_MODE_STRETCH,
												false,
												USE_INHERITED_COORD_OFF,
												temp_coord,
												temp_scale);

		vignette_layer.a += 0.75 * HHLP_QuadraticBezier(1 - HSM_GetVignetteFactor(vignette_coord, HSM_VIEWPORT_VIGNETTE_OPACITY, 1), vec2(1, 0.5));
	}


	//-----------------------------------------------------------------------------------------
	// COMPOSITE ALL LAYERS
	//-----------------------------------------------------------------------------------------

	vec4 frag_color_linear = vec4(0);

	int start_layer = 0;
	int end_layer = int(MAX_LAYER_ORDER);

#ifdef LAYERS_OVER_CRT
	start_layer = int(HSM_CRT_LAYER_ORDER);
	end_layer = int(MAX_LAYER_ORDER);
#else
	start_layer = 0;
	end_layer = int(HSM_CRT_LAYER_ORDER - 1);
#endif

	OUTSIDE_TUBE_MASK_FOR_IMAGE = 1 - HSM_GetCornerMask(tube_curved_coord_ctr * 1.003 + 0.5, SCREEN_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);

	for(int i=start_layer; i <= end_layer; i++)
	{
		// BACKGROUND
		if (HSM_BG_LAYER_ORDER == i)
			frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
													bg_image, 
													HSM_BG_BLEND_MODE, 
													HSM_BG_MASK_MODE, 
													HSM_BG_CUTOUT_MODE,
													HSM_BG_DUALSCREEN_VIS_MODE,
													HSM_BG_OPACITY);

		// VIGNETTE
		if (HSM_VIEWPORT_VIGNETTE_LAYER_ORDER == i)
			frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
													vignette_layer, 
													BLEND_MODE_NORMAL, 
													HSM_VIEWPORT_VIGNETTE_MASK_MODE, 
													HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE,
													IMAGE_DUALSCREEN_MODE_BOTH,
													HSM_VIEWPORT_VIGNETTE_OPACITY);
		// LED IMAGE
		if (HSM_LED_LAYER_ORDER == i)
			if (HSM_LED_OPACITY > 0)
				frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
														led_image, 
														HSM_LED_BLEND_MODE, 
														HSM_LED_MASK_MODE, 
														HSM_LED_CUTOUT_MODE, 
														HSM_LED_DUALSCREEN_VIS_MODE,
														HSM_LED_OPACITY);

		// DEVICE IMAGE
		if (HSM_DEVICE_LAYER_ORDER == i)
			if (HSM_DEVICE_OPACITY > 0)
				frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
														device_image, 
														HSM_DEVICE_BLEND_MODE, 
														HSM_DEVICE_MASK_MODE, 
														HSM_DEVICE_CUTOUT_MODE, 
														HSM_DEVICE_DUALSCREEN_VIS_MODE,
														HSM_DEVICE_OPACITY);

		// DECAL IMAGE
		if (HSM_DECAL_LAYER_ORDER == i)
			if (HSM_DECAL_OPACITY > 0)
				frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
														decal_image, 
														HSM_DECAL_BLEND_MODE, 
														HSM_DECAL_MASK_MODE, 
														HSM_DECAL_CUTOUT_MODE, 
														HSM_DECAL_DUALSCREEN_VIS_MODE,
														HSM_DECAL_OPACITY);

		// CABINET GLASS
		if (HSM_CAB_GLASS_LAYER_ORDER == i)
			if (HSM_CAB_GLASS_OPACITY > 0)
				frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
														cab_glass_image, 
														HSM_CAB_GLASS_BLEND_MODE, 
														HSM_CAB_GLASS_MASK_MODE, 
														HSM_CAB_GLASS_CUTOUT_MODE, 
														HSM_CAB_GLASS_DUALSCREEN_VIS_MODE,
														HSM_CAB_GLASS_OPACITY);


		// Top Layer
		if (HSM_TOP_LAYER_ORDER == i)
			if (HSM_TOP_OPACITY > 0)
				frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
														top_image, 
														HSM_TOP_BLEND_MODE, 
														HSM_TOP_MASK_MODE, 
														HSM_TOP_CUTOUT_MODE, 
														HSM_TOP_DUALSCREEN_VIS_MODE,
														HSM_TOP_OPACITY);
	}

#ifdef LAYERS_UNDER_CRT
	// BEZEL
	// GENERATED BEZEL LAYER
	if (HSM_BZL_OPACITY > 0)
		frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
												bezel_layer, 
												HSM_BZL_BLEND_MODE, 
												MASK_MODE_ALL, 
												0, 
												IMAGE_DUALSCREEN_MODE_BOTH,
												HSM_BZL_OPACITY);
	if (HSM_FRM_OPACITY > 0)
	{
		if (HSM_FRM_SHADOW_OPACITY > 0)
			frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
													frame_shadow_layer, 
													BLEND_MODE_NORMAL, 
													MASK_MODE_OUTSIDE_BEZEL,
													0, 
													IMAGE_DUALSCREEN_MODE_BOTH,
													HSM_FRM_OPACITY * HSM_FRM_SHADOW_OPACITY);

		// GENERATED FRAME LAYER
		frag_color_linear = BlendModeMaskLayerMix(frag_color_linear, 
												frame_layer, 
												HSM_FRM_BLEND_MODE, 
												MASK_MODE_ALL, 
												0, 
												IMAGE_DUALSCREEN_MODE_BOTH,
												HSM_FRM_OPACITY);
	}

// end #ifdef LAYERS_UNDER_CRT
#endif

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

		frag_color_linear = HSM_PreMultAlphaBlend(frag_color_linear, vec4(1, 0, 0, 1) * 0.15 * debug_mask);
		frag_color_linear = HSM_PreMultAlphaBlend(frag_color_linear, vec4(0.05, 0.05, 0.05, 1) * 0.15 * (1 - debug_mask));
		frag_color_linear = clamp(frag_color_linear, 0, 1);
	}

	if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_OUTSIDE_TUBE_MASK )
		frag_color_linear = vec4(1 - TUBE_MASK);

	if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_OUTSIDE_SCREEN_MASK )
		frag_color_linear = vec4(1 - SCREEN_MASK);

	FragColor = frag_color_linear;

	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
		FragColor = vec4(0.01, 0.01, 0.01, 10);

	//-----------------------------------------------------------------------------------------
	// END OF MAIN IMAGE LAYERING
	//-----------------------------------------------------------------------------------------
	return;
}
































