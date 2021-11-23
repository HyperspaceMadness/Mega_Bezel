
float HSM_GetParameterSum_ScreenScale()
{
	// Each value should be multipled so that it becomes an integer if possible
	float out_sum = (0
					// Night Lighting
					+ HSM_AMBIENT_LIGHTING_OPACITY * 200
					+ abs(HSM_AMBIENT_LIGHTING_HUE) * 720
					+ HSM_AMBIENT_LIGHTING_SATURATION * 200
					+ HSM_AMBIENT_LIGHTING_VALUE * 200
					+ HSM_AMBIENT_LIGHTING_SCALE * 200
					+ HSM_AMBIENT_LIGHTING_MIRROR_HORZ * 2
					+ HSM_NIGHT_DITHERING_SAMPLES * 2

					//    Zoom & Pan
					// + HSM_VIEWPORT_ZOOM * 2000
					// + HSM_VIEWPORT_POSITION_X * 2000
					// + HSM_VIEWPORT_POSITION_Y * 2000

					// //    FLIP & ROTATE
					+ HSM_FLIP_VIEWPORT_VERTICAL * 2
					+ HSM_FLIP_VIEWPORT_HORIZONTAL * 2
					// + HSM_FLIP_CORE_VERTICAL * 2
					// + HSM_FLIP_CORE_HORIZONTAL * 2
					// + HSM_ROTATE_CORE_IMAGE * 2

					// //    ASPECT RATIO
					// + HSM_ASPECT_RATIO_ORIENTATION
					// + HSM_ASPECT_RATIO_MODE
					// + HSM_ASPECT_RATIO_EXPLICIT

					// //    SCALING
					// + HSM_INT_SCALE_MODE
					// + HSM_INT_SCALE_MULTIPLE_OFFSET
					// + HSM_INT_SCALE_MULTIPLE_OFFSET_LONG
					// + HSM_INT_SCALE_BORDER_MIN_HEIGHT
					// + HSM_VERTICAL_PRESET
					// + HSM_NON_INTEGER_SCALE

					// //    Extended Scale
					// + HSM_USE_IMAGE_FOR_PLACEMENT
					// + HSM_PLACEMENT_IMAGE_MODE
					// + HSM_NON_INTEGER_SCALE_OFFSET * 100
					// + HSM_SNAP_TO_INT_SCALE_TOLERANCE

					// //    Position
					// + HSM_SCREEN_POSITION_X * 1000
					// + HSM_SCREEN_POSITION_Y * 1000

					// //    CROPPING
					// + HSM_CROP_MODE
					// + HSM_CROP_PERCENT_ZOOM / 100
					// + HSM_CROP_PERCENT_TOP * 100
					// + HSM_CROP_PERCENT_BOTTOM * 100
					// + HSM_CROP_PERCENT_LEFT * 100
					// + HSM_CROP_PERCENT_RIGHT * 100
					// + HSM_CROP_BLACK_THRESHOLD * 100

					// //    DUAL SCREEN
					+ HSM_DUALSCREEN_MODE * 2
					+ HSM_DUALSCREEN_CORE_IMAGE_SPLIT_MODE * 2
					+ HSM_DUALSCREEN_CORE_IMAGE_SWAP_SCREENS * 2
					+ HSM_DUALSCREEN_CORE_IMAGE_SPLIT_OFFSET * 200
					+ HSM_DUALSCREEN_VIEWPORT_SPLIT_LOCATION * 2000
					+ HSM_DUALSCREEN_SHIFT_POSITION_WITH_SCALE * 2
					+ HSM_DUALSCREEN_POSITION_OFFSET_BETWEEN_SCREENS * 4000
					// + HSM_2ND_SCREEN_ASPECT_RATIO_MODE * 2
					// + HSM_2ND_SCREEN_INDEPENDENT_SCALE * 2
					// + HSM_2ND_SCREEN_SCALE_OFFSET * 200
					// + HSM_2ND_SCREEN_POS_X * 2000
					// + HSM_2ND_SCREEN_POS_Y * 2000
					// + HSM_2ND_SCREEN_CROP_PERCENT_ZOOM * 200
					// + HSM_2ND_SCREEN_CROP_PERCENT_TOP * 200
					// + HSM_2ND_SCREEN_CROP_PERCENT_BOTTOM * 200
					// + HSM_2ND_SCREEN_CROP_PERCENT_LEFT * 200
					// + HSM_2ND_SCREEN_CROP_PERCENT_RIGHT * 200

					// //    CURVATURE
					+ HSM_CURVATURE_MODE * 2
					+ HSM_CURVATURE_2D_SCALE_LONG_AXIS * 100
					+ HSM_CURVATURE_2D_SCALE_SHORT_AXIS * 100
					+ HSM_CURVATURE_3D_RADIUS * 100
					+ HSM_CURVATURE_3D_VIEW_DIST * 100
					+ HSM_CURVATURE_3D_TILT_ANGLE_X * 100
					+ HSM_CURVATURE_3D_TILT_ANGLE_Y * 100

					// //    SCREEN BLACK EDGE
					+ HSM_GLOBAL_CORNER_RADIUS
					+ HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE * 100
					+ HSM_TUBE_BLACK_EDGE_SHARPNESS * 100
					+ HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE * 100
					+ HSM_TUBE_BLACK_EDGE_THICKNESS * 100

					// //    BEZEL INDEPENDENT SCALE
					// + HSM_BZL_USE_INDEPENDENT_SCALE
					// + HSM_BZL_INDEPENDENT_SCALE * 100
					// + HSM_BZL_USE_INDEPENDENT_CURVATURE
					// + HSM_BZL_INDEPENDENT_CURVATURE_SCALE_LONG_AXIS * 100
					// + HSM_BZL_INDEPENDENT_CURVATURE_SCALE_SHORT_AXIS * 100

					// //    BEZEL GENERAL
					+ HSM_BZL_OPACITY * 100
					+ HSM_BZL_BLEND_MODE
					+ HSM_BZL_WIDTH /  0.0008624
					+ HSM_BZL_HEIGHT / 0.0008732
					+ HSM_BZL_SCALE_OFFSET * 100
					+ HSM_BZL_INNER_CURVATURE_SCALE * 100
					+ HSM_BZL_INNER_CORNER_RADIUS_SCALE * 100
					);
	return out_sum;
}

float HSM_GetParameterSum_Bezel()
{
	float out_sum = (0

	#ifdef HAS_BEZEL_PARAMS
					// //	Bezel Params not in Screen Scale
					+ HSM_BZL_INNER_EDGE_THICKNESS / 0.00007
					+ HSM_BZL_INNER_EDGE_SHARPNESS * 100
					+ HSM_BZL_OUTER_CORNER_RADIUS_SCALE * 100
					+ HSM_BZL_OUTER_CURVATURE_SCALE * 100
					+ HSM_BZL_OUTER_POSITION_Y * 100
					+ HSM_BZL_BRIGHTNESS * 100
					+ HSM_BZL_HIGHLIGHT * 100
					+ HSM_BZL_INNER_EDGE_SHADOW * 100

					// // Generated Bezel
					+ HSM_BZL_COLOR_HUE * 360
					+ HSM_BZL_COLOR_SATURATION * 100
					+ HSM_BZL_COLOR_VALUE * 400
					+ HSM_BZL_NOISE * 100

					// // Generated Frame
					+ HSM_FRM_OPACITY * 100
					+ HSM_FRM_BLEND_MODE * 2
					+ HSM_FRM_TEXTURE_OPACITY * 1000
					+ HSM_FRM_TEXTURE_BLEND_MODE * 2
					+ HSM_FRM_INNER_EDGE_THICKNESS / 0.00003
					+ HSM_FRM_THICKNESS / 0.0007
					+ HSM_FRM_THICKNESS_SCALE_X * 100
					+ HSM_FRM_OUTER_POS_Y * 2000
					+ HSM_FRM_OUTER_CURVATURE_SCALE * 100
					+ HSM_FRM_OUTER_CORNER_RADIUS * 100
					+ HSM_FRM_OUTER_EDGE_THICKNESS / 0.00006
					+ HSM_FRM_OUTER_EDGE_SHADING * 100
					+ HSM_FRM_SHADOW_OPACITY * 100
					+ HSM_FRM_SHADOW_WIDTH * 1000

					// Corner
					+ HSM_REFLECT_CORNER_FADE * 200
					+ HSM_REFLECT_CORNER_FADE_DISTANCE * 200
					+ HSM_REFLECT_CORNER_INNER_SPREAD * 100
					+ HSM_REFLECT_CORNER_OUTER_SPREAD * 100
					+ HSM_REFLECT_CORNER_ROTATION_OFFSET_TOP * 4
					+ HSM_REFLECT_CORNER_ROTATION_OFFSET_BOTTOM * 4
					+ HSM_REFLECT_CORNER_SPREAD_FALLOFF * 2

#endif

					);
	return out_sum;
}

float HSM_GetParameterSum_ImageLayers1()
{
	float out_sum = (0

#ifdef HAS_IMAGE_LAYER_PARAMS
					+ HSM_STATIC_LAYERS_GAMMA * 100

					// Layer Order
					+ HSM_BG_LAYER_ORDER
					+ HSM_VIEWPORT_VIGNETTE_LAYER_ORDER
					+ HSM_CRT_LAYER_ORDER
					+ HSM_DEVICE_LAYER_ORDER
					+ HSM_CAB_GLASS_LAYER_ORDER
					+ HSM_DECAL_LAYER_ORDER
					+ HSM_LED_LAYER_ORDER
					+ HSM_TOP_LAYER_ORDER

					// Cutout
					+ HSM_CUTOUT_ASPECT_MODE
					+ HSM_CUTOUT_EXPLICIT_ASPECT
					+ HSM_CUTOUT_SCALE_MODE
					+ HSM_CUTOUT_KEEP_ASPECT
					+ HSM_CUTOUT_SCALE * 100
					+ HSM_CUTOUT_SCALE_X * 100
					+ HSM_CUTOUT_POS_X * 400
					+ HSM_CUTOUT_POS_Y * 400
					+ HSM_CUTOUT_CORNER_RADIUS

					// Background
					+ HSM_BG_OPACITY
					+ HSM_BG_HUE
					+ HSM_BG_COLORIZE_ON
					+ HSM_BG_SATURATION
					+ HSM_BG_BRIGHTNESS
					+ HSM_BG_GAMMA
					+ HSM_BG_APPLY_ADJUSTMENT
					+ HSM_BG_BLEND_MODE
					+ HSM_BG_SOURCE_MATTE_TYPE
					+ HSM_BG_MASK_MODE
					+ HSM_BG_CUTOUT_MODE
					+ HSM_BG_DUALSCREEN_VIS_MODE
					+ HSM_BG_TEXTURE_ASPECT_MODE
					+ HSM_BG_EXPLICIT_TEXTURE_ASPECT * 100
					+ HSM_BG_SCALE_MODE
					+ HSM_BG_FILL_MODE
					+ HSM_BG_SCALE_KEEP_ASPECT
					+ HSM_BG_SCALE * 100
					+ HSM_BG_SCALE_X * 100
					+ HSM_BG_POS_X * 400
					+ HSM_BG_POS_Y * 400
					+ HSM_BG_MIRROR_WRAP

					// Background Vignette
					+ HSM_VIEWPORT_VIGNETTE_OPACITY * 100
					+ HSM_VIEWPORT_VIGNETTE_MASK_MODE
					+ HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE
					+ HSM_VIEWPORT_VIGNETTE_SCALE_MODE
					+ HSM_VIEWPORT_VIGNETTE_SCALE * 100
					+ HSM_VIEWPORT_VIGNETTE_SCALE_X * 100
					+ HSM_VIEWPORT_VIGNETTE_POS_X * 400
					+ HSM_VIEWPORT_VIGNETTE_POS_Y * 400

					// LED
					+ HSM_LED_OPACITY * 100
					+ HSM_LED_COLORIZE_ON
					+ HSM_LED_HUE * 360
					+ HSM_LED_SATURATION * 100
					+ HSM_LED_BRIGHTNESS * 100
					+ HSM_LED_GAMMA * 100
					+ HSM_LED_BLEND_MODE
					+ HSM_LED_SOURCE_MATTE_TYPE
					+ HSM_LED_MASK_MODE
					+ HSM_LED_CUTOUT_MODE
					+ HSM_LED_DUALSCREEN_VIS_MODE
					+ HSM_LED_TEXTURE_ASPECT_MODE
					+ HSM_LED_EXPLICIT_TEXTURE_ASPECT
					+ HSM_LED_SCALE_MODE
					+ HSM_LED_SCALE_KEEP_ASPECT
					+ HSM_LED_FILL_MODE
					+ HSM_LED_SCALE * 100
					+ HSM_LED_SCALE_X * 100
					+ HSM_LED_POS_X * 400
					+ HSM_LED_POS_Y * 400
#endif

					);
	return out_sum;
}

float HSM_GetParameterSum_ImageLayers2()
{
	float out_sum = (0

#ifdef HAS_IMAGE_LAYER_PARAMS
					// // Device
					+ HSM_DEVICE_OPACITY * 100
					+ HSM_DEVICE_HUE * 360
					+ HSM_DEVICE_COLORIZE_ON
					+ HSM_DEVICE_SATURATION * 100
					+ HSM_DEVICE_BRIGHTNESS * 100
					+ HSM_DEVICE_GAMMA * 100
					+ HSM_DEVICE_BLEND_MODE
					+ HSM_DEVICE_SOURCE_MATTE_TYPE
					+ HSM_DEVICE_MASK_MODE
					+ HSM_DEVICE_CUTOUT_MODE
					+ HSM_DEVICE_DUALSCREEN_VIS_MODE
					+ HSM_DEVICE_TEXTURE_ASPECT_MODE
					+ HSM_DEVICE_EXPLICIT_TEXTURE_ASPECT * 100
					+ HSM_DEVICE_SCALE_MODE
					+ HSM_DEVICE_SCALE_KEEP_ASPECT
					+ HSM_DEVICE_FILL_MODE
					+ HSM_DEVICE_SCALE * 100
					+ HSM_DEVICE_SCALE_X * 100
					+ HSM_DEVICE_POS_X * -100
					+ HSM_DEVICE_POS_Y * 100
					+ HSM_DEVICE_COPY_INHERITED_COORD

					// // Decal
					+ HSM_DECAL_OPACITY * 100
					+ HSM_DECAL_HUE * 360
					+ HSM_DECAL_COLORIZE_ON
					+ HSM_DECAL_SATURATION * 100
					+ HSM_DECAL_BRIGHTNESS * 100
					+ HSM_DECAL_GAMMA * 100
					+ HSM_DECAL_BLEND_MODE
					+ HSM_DECAL_SOURCE_MATTE_TYPE
					+ HSM_DECAL_MASK_MODE
					+ HSM_DECAL_TEXTURE_ASPECT_MODE
					+ HSM_DECAL_EXPLICIT_TEXTURE_ASPECT * 100
					+ HSM_DECAL_SCALE_MODE
					+ HSM_DECAL_SCALE_KEEP_ASPECT
					+ HSM_DECAL_FILL_MODE
					+ HSM_DECAL_SCALE * 100
					+ HSM_DECAL_SCALE_X * 100
					+ HSM_DECAL_POS_X * -100
					+ HSM_DECAL_POS_Y * 100
					+ HSM_DECAL_CUTOUT_MODE
					+ HSM_DECAL_DUALSCREEN_VIS_MODE

					// // Cab Glass
					+ HSM_CAB_GLASS_OPACITY * 100
					+ HSM_CAB_GLASS_HUE * 360
					+ HSM_CAB_GLASS_COLORIZE_ON
					+ HSM_CAB_GLASS_SATURATION * 100
					+ HSM_CAB_GLASS_BRIGHTNESS * 100
					+ HSM_CAB_GLASS_GAMMA * 100
					+ HSM_CAB_GLASS_BLEND_MODE
					+ HSM_CAB_GLASS_SOURCE_MATTE_TYPE
					+ HSM_CAB_GLASS_MASK_MODE
					+ HSM_CAB_GLASS_CUTOUT_MODE
					+ HSM_CAB_GLASS_DUALSCREEN_VIS_MODE
					+ HSM_CAB_GLASS_APPLY_NIGHTLIGHTING
					+ HSM_CAB_GLASS_TEXTURE_ASPECT_MODE
					+ HSM_CAB_GLASS_EXPLICIT_TEXTURE_ASPECT * 100
					+ HSM_CAB_GLASS_SCALE_MODE
					+ HSM_CAB_GLASS_SCALE_KEEP_ASPECT
					+ HSM_CAB_GLASS_FILL_MODE
					+ HSM_CAB_GLASS_SCALE * 100
					+ HSM_CAB_GLASS_SCALE_X * 100
					+ HSM_CAB_GLASS_POS_X * -100
					+ HSM_CAB_GLASS_POS_Y * 100

					// // Top Image
					+ HSM_TOP_OPACITY * 100
					+ HSM_TOP_HUE * 360
					+ HSM_TOP_COLORIZE_ON
					+ HSM_TOP_SATURATION * 100
					+ HSM_TOP_BRIGHTNESS * 100
					+ HSM_TOP_GAMMA * 100
					+ HSM_TOP_BLEND_MODE
					+ HSM_TOP_SOURCE_MATTE_TYPE
					+ HSM_TOP_MASK_MODE
					+ HSM_TOP_CUTOUT_MODE
					+ HSM_TOP_DUALSCREEN_VIS_MODE
					+ HSM_TOP_TEXTURE_ASPECT_MODE
					+ HSM_TOP_EXPLICIT_TEXTURE_ASPECT * 100
					+ HSM_TOP_SCALE_MODE
					+ HSM_TOP_SCALE_KEEP_ASPECT * 2
					+ HSM_TOP_FILL_MODE
					+ HSM_TOP_SCALE * 100
					+ HSM_TOP_SCALE_X * 100
					+ HSM_TOP_POS_X * -100
					+ HSM_TOP_POS_Y * 100
					+ HSM_TOP_MIRROR_WRAP
					+ HSM_RENDER_SIMPLE_MODE
					+ HSM_LAYERING_DEBUG_MASK_MODE
#endif

					);
	return out_sum;
}

int HSM_IsCoordIn2DRange(vec2 in_coord, vec4 in_2d_range)
{
	return (in_coord.x > in_2d_range.x && 
			in_coord.y > in_2d_range.y && 
			in_coord.x < in_2d_range.z && 
			in_coord.y < in_2d_range.w ) ? 1 : 0;
}

vec4 HSM_GetColorForScreenInfoCache(vec2 viewport_coord, sampler2D original_pass, sampler2D screen_placement_image)
{
	vec4 out_color = vec4(0);

	vec2 cropped_rotated_size_with_res_mult = vec2(100);
	vec2 cropped_rotated_size = vec2(100);
	vec2 cropped_sample_area_start_pixel_coord = vec2(100);
	HSM_GetCroppedRotatedSizeAndPixelSampleAreaStart(1, original_pass, cropped_rotated_size, cropped_rotated_size_with_res_mult, cropped_sample_area_start_pixel_coord);

	// First Screen
	vec2 screen_pos_and_height = HSM_GetScreenPlacementYPosAndHeight(screen_placement_image, 60);
	float screen_aspect = HSM_GetScreenAspect(1, cropped_rotated_size);
	vec2 screen_scale = HSM_GetScreenScale(screen_aspect, screen_pos_and_height.y, cropped_rotated_size);
	vec2 tube_scale = HSM_GetTubeScale(screen_scale, screen_pos_and_height.y);
	vec2 pos_offset = HSM_GetPositionOffset(vec2(0.5, screen_pos_and_height.x), screen_scale);

	vec2 rotated_core_preppezd_size = HSM_GetRotatedScreenCorePreppedSize(1);

	vec2 cropped_size_with_res_mult_2nd_screen = vec2(100);
	vec2 cropped_size_2nd_screen = vec2(100);
	vec2 sample_area_start_pixel_coord_2nd_screen = vec2(100);
	HSM_GetCroppedRotatedSizeAndPixelSampleAreaStart(2, original_pass, cropped_size_2nd_screen, cropped_size_with_res_mult_2nd_screen, sample_area_start_pixel_coord_2nd_screen);
	float screen_aspect_2nd_screen = HSM_2ND_SCREEN_ASPECT_RATIO_MODE == 1 ? cropped_size_2nd_screen.x/cropped_size_2nd_screen.y : screen_aspect;
	vec2 screen_scale_2nd_screen = HSM_GetScreenScaleFor2ndScreen(screen_scale, screen_aspect_2nd_screen);
	vec2 tube_scale_2nd_screen = HSM_GetTubeScale(screen_scale_2nd_screen, DEFAULT_UNCORRECTED_SCREEN_SCALE.y);
	vec2 pos_offset_2nd_screen = HSM_GetPositionOffsetFor2ndScreen(screen_scale_2nd_screen);

	vec4 sample_2d_range = vec4(0);
	sample_2d_range = vec4(0, 0, 0.25, 0.25);

	// 1st Sample
	// r AVERAGE_LUMA
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.r = HSM_GetAverageLuma(original_pass, global.CorePreppedPassSize.xy);
	}

	// 2nd Sample
	// r SCREEN_ASPECT
	// ba SCREEN_SCALE
	sample_2d_range = vec4(0.25, 0, 0.5, 0.25);
	// sample_2d_range = HSM_GetInfoSampleRange(2);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{ 
		out_color.r = screen_aspect;
		out_color.ba = screen_scale;
	}

	// 3rd Sample
	// rg TUBE_SCALE 
	// ba SCREEN_POS_OFFSET
	sample_2d_range = vec4(0.5, 0, 0.75, 0.25);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = tube_scale;
		out_color.ba = pos_offset;
	}

	// 4th Sample CROPPED_ROTATED_SIZE_WITH_RES_MULT
	// rg CROPPED_ROTATED_SIZE_WITH_RES_MULT
	sample_2d_range = vec4(0.75, 0, 1, 0.25);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_rotated_size_with_res_mult;
	}

	// 5th Sample - 2nd Screen
	// rg CROPPED_ROTATED_SIZE
	// ba SAMPLE_AREA_START_PIXEL_COORD
	sample_2d_range = vec4(0, 0.25, 0.25, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_rotated_size;
		out_color.ba = cropped_sample_area_start_pixel_coord;
	}

	// 6th Sample - 2nd Screen
	// r SCREEN_ASPECT_2ND_SCREEN
	// g PARAMETER_SUM_SCREEN_SCALE
	// ba SCREEN_SCALE_2ND_SCREEN
	sample_2d_range = vec4(0.25, 0.25, 0.5, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.r = screen_aspect_2nd_screen;
		out_color.g = HSM_GetParameterSum_ScreenScale();
		out_color.ba = screen_scale_2nd_screen;
	}

	// 7th Sample - 2nd Screen
	// rg TUBE_SCALE
	// ba SCREEN_POS_OFFSET
	sample_2d_range = vec4(0.5, 0.25, 0.75, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = tube_scale_2nd_screen;
		out_color.ba = pos_offset_2nd_screen;
	}

	// 8th Sample - 2nd Screen
	// rg CROPPED_ROTATED_SIZE_WITH_RES_MULT 2nd screen
	sample_2d_range = vec4(0.75, 0.25, 1, 0.5);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_size_with_res_mult_2nd_screen;
	}

	// 9th Sample
	// rg CROPPED_ROTATED_SIZE 2nd screen
	// ba SAMPLE_AREA_START_PIXEL_COORD 2nd screen
	sample_2d_range = vec4(0, 0.5, 0.25, 0.75);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = cropped_size_2nd_screen;
		out_color.ba = sample_area_start_pixel_coord_2nd_screen;
	}

	// 10th Sample
	// rg CORE_SIZE
	sample_2d_range = vec4(0.25, 0.5, 0.5, 0.75);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = global.CorePassSize.xy;
	}

	// 11th Sample
	// rg VIEWPORT_SCALE
	// ba VIEWPORT_POS
	sample_2d_range = vec4(0.5, 0.5, 0.75, 0.75);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = vec2(HSM_VIEWPORT_ZOOM, HSM_VIEWPORT_ZOOM);
		out_color.ba = vec2(HSM_VIEWPORT_POSITION_X, HSM_VIEWPORT_POSITION_Y);
	}

	// 12th Sample
	// rg SCREEN_SCALE_2ND_SCREEN
	// ba SCREEN_POS_OFFSET_2ND_SCREEN
	sample_2d_range = vec4(0.75, 0.5, 1, 0.75);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.rg = screen_scale_2nd_screen;
		out_color.ba = pos_offset_2nd_screen;
	}

	// 13th Sample
	// r PARAMETER_SUM_SCREEN_SCALE
	sample_2d_range = vec4(0.0, 0.75, 0.25, 1);
	if (HSM_IsCoordIn2DRange(viewport_coord, sample_2d_range) == 1)
	{
		out_color.r = HSM_GetParameterSum_ScreenScale();
		out_color.g = HSM_GetParameterSum_Bezel();
		// out_color.b = HSM_GetParameterSum_ImageLayers1();
		// out_color.a = HSM_GetParameterSum_ImageLayers2();
	}

	return out_color;
}

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
layout(set = 0, binding = 1) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D ScreenPlacementImage;

void main()
{
   FragColor = HSM_GetColorForScreenInfoCache(vTexCoord, Source, ScreenPlacementImage);
}