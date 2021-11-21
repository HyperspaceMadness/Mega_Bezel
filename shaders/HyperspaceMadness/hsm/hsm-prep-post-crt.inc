#include "../hsm/hsm-common-functions-bezel.inc"

#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 vTexCoord;
layout(location = 7) out vec2 UNFLIPPED_VIEWPORT_COORD;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.00001;

	UNFLIPPED_VIEWPORT_COORD = vTexCoord * 1.0001;

	if (HSM_FLIP_VIEWPORT_VERTICAL > 0.5)
		vTexCoord.y =  1 - vTexCoord.y;

	if (HSM_FLIP_VIEWPORT_HORIZONTAL > 0.5)
		vTexCoord.x =  1 - vTexCoord.x;
}

#pragma stage fragment

layout(location = 0) in vec2 vTexCoord;
layout(location = 7) in vec2 UNFLIPPED_VIEWPORT_COORD;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D Source;

layout(set = 0, binding = 2) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 3) uniform sampler2D InfoCachePassFeedback;

#ifndef IS_POTATO_PRESET
layout(set = 0, binding = 4) uniform sampler2D TubeDiffuseImage;
layout(set = 0, binding = 5) uniform sampler2D TubeColoredGelImage;
layout(set = 0, binding = 6) uniform sampler2D TubeStaticReflectionImage;
#endif

layout(set = 0, binding = 7) uniform sampler2D BackgroundImage;
layout(set = 0, binding = 8) uniform sampler2D BackgroundVertImage;

#ifndef IS_POTATO_PRESET
layout(set = 0, binding = 9) uniform sampler2D NightLightingImage;
layout(set = 0, binding = 10) uniform sampler2D IntroPass;
#endif

#ifdef IS_POTATO_PRESET
vec4 HSM_GetPostCrtPreppedColorPotato(vec4 in_color_with_gamma, vec2 VIEWPORT_COORD, vec2 screen_curved_coord, in sampler2D BackgroundImage, in sampler2D BackgroundVertImage)
{ 
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);
	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	TUBE_MASK = HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);

	if (HSM_MONOCHROME_MODE > 0.5)
		in_color_with_gamma = HSM_ApplyMonochrome(in_color_with_gamma);

	vec4 out_color = HSM_Linearize(in_color_with_gamma, HSM_DEFAULT_SRGB_GAMMA);
	
	out_color *= HSM_POST_CRT_BRIGHTNESS;

	vec2 mirrored_screen_coord = HSM_GetMirrorWrappedCoord(screen_curved_coord);
	float screen_mask = HSM_GetCornerMask((screen_curved_coord - 0.5) * 0.999 + 0.5, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS, 0.9);

	if (HSM_SCREEN_VIGNETTE_ON > 0.5)
	{
		// out_color *= HSM_GetScreenVignetteFactor(screen_curved_coord);

		float vignette_factor = HSM_GetScreenVignetteFactor(mirrored_screen_coord);

		// float vignette_factor_outside_screen = HSM_SCREEN_VIGNETTE_IN_REFLECTION * vignette_factor + (1 - HSM_SCREEN_VIGNETTE_IN_REFLECTION) * 1;
		if (HSM_SCREEN_VIGNETTE_IN_REFLECTION < 0.5)
			vignette_factor = screen_mask * vignette_factor + (1 - screen_mask);

		out_color *= vignette_factor;
	}

	out_color.rgb *= screen_mask;
	out_color *= TUBE_MASK;

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

vec4 HSM_GetPostCrtPreppedColor(vec4 in_color_with_gamma, vec2 VIEWPORT_COORD, vec2 screen_curved_coord, in sampler2D source_pass, bool source_is_linear, in sampler2D TubeDiffuseImage, in sampler2D TubeColoredGelImage, in sampler2D TubeStaticReflectionImage, in sampler2D BackgroundImage, in sampler2D BackgroundVertImage, in sampler2D NightLightingImage)
{
	if (HSM_MONOCHROME_MODE > 0.5)
		in_color_with_gamma = HSM_ApplyMonochrome(in_color_with_gamma);

	vec4 out_color = HSM_Linearize(in_color_with_gamma, HSM_DEFAULT_SRGB_GAMMA);
	out_color *= HSM_POST_CRT_BRIGHTNESS;

	vec2 mirrored_screen_coord = HSM_GetMirrorWrappedCoord(screen_curved_coord);
	float screen_mask = HSM_GetCornerMask((screen_curved_coord - 0.5) * 0.999 + 0.5, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS, 0.9);

	if (HSM_SCREEN_VIGNETTE_ON > 0.5)
	{
		// out_color *= HSM_GetScreenVignetteFactor(screen_curved_coord);

		float vignette_factor = HSM_GetScreenVignetteFactor(mirrored_screen_coord);

		float vignette_factor_outside_screen = HSM_SCREEN_VIGNETTE_IN_REFLECTION * vignette_factor + (1 - HSM_SCREEN_VIGNETTE_IN_REFLECTION) * 1;
		vignette_factor = screen_mask * vignette_factor + (1 - screen_mask) * vignette_factor_outside_screen;

		out_color *= vignette_factor;
	}

	vec4 night_lighting_image = HSM_GetNightLightingMultiplyColor( VIEWPORT_COORD, NightLightingImage );

	// float screen_edge_mask = HSM_GetCornerMask(screen_curved_coord, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS, HSM_TUBE_BLACK_EDGE_SHARPNESS);
	// screen_edge_mask = 1 - ((1 - screen_edge_mask) * screen_mask);

	// out_color *= screen_edge_mask;


	if (HSM_CRT_ORIGINAL_AB_COMPARE_ON == 1)
	{
		vec4 source_color = HSM_GetCroppedTexSample(source_pass, mirrored_screen_coord);
		if (!source_is_linear)
			source_color = HSM_Linearize(source_color, HSM_GAMMA_IN_LINEARIZE);
		source_color = HSM_Delinearize(source_color, HSM_GAMMA_OUT_CRT);
		source_color = HSM_Linearize(source_color, HSM_DEFAULT_SRGB_GAMMA);

		out_color = mix(out_color, source_color, mirrored_screen_coord.x < HSM_CRT_ORIGINAL_AB_COMPARE_SPLIT_POSITION ? 1 : 0);
	}

	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);
	vec2 mirrored_tube_coord = HSM_GetMirrorWrappedCoord(tube_curved_coord);

	if (HSM_FAKE_SCANLINE_OPACITY > 0.001 && HSM_FAKE_SCANLINE_MODE > 0.5)
	{
		vec4 scanline_masked_color = HSM_ApplyScanlineMask(out_color, SCREEN_SCALE, SCREEN_COORD, screen_curved_coord);
		
		// Darken the outside image a bit
		out_color = mix(out_color, out_color * 0.9, HSM_FAKE_SCANLINE_OPACITY);

		// Show scanlines only in the tube area
		float tube_highlight_mask = HSM_GetCornerMask((tube_curved_coord - 0.5) * 0.995 + 0.5 , SCREEN_ASPECT, HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS, 0.05);
		out_color = mix(out_color, scanline_masked_color, tube_highlight_mask);
	}

	// Add a Colored Gel image on top of the screen, this is what colors games like space invaders or battlezone
	if (HSM_TUBE_COLORED_GEL_IMAGE_MULTIPLY_AMOUNT > 0 || HSM_TUBE_COLORED_GEL_IMAGE_ADDITIVE_AMOUNT > 0 || HSM_TUBE_COLORED_GEL_IMAGE_NORMAL_AMOUNT > 0)
	{
		// TODO need to update this for flip horizontal
		vec2 gel_coord = mix( mirrored_tube_coord, -1 * (mirrored_tube_coord - 0.5) + 0.5 , HSM_FLIP_VIEWPORT_VERTICAL);
		vec4 gel_image = texture(TubeColoredGelImage, mirrored_tube_coord);
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

	
	// TubeDiffuseImage
	vec4 tube_diffuse = vec4(0, 0, 0, 1);
	if (HSM_TUBE_DIFFUSE_BRIGHTNESS > 0)
	{
		tube_diffuse = texture(TubeDiffuseImage, mirrored_tube_coord);
		tube_diffuse = HSM_GetPreMultipliedColorLinear(tube_diffuse, SOURCE_MATTE_PREMULTIPLIED, HSM_DEFAULT_SRGB_GAMMA);
		tube_diffuse.rgb = HSM_ApplyHSVAdjustment(tube_diffuse.rgb, HSM_TUBE_DIFFUSE_HUE, HSM_TUBE_DIFFUSE_SATURATION, HSM_TUBE_DIFFUSE_BRIGHTNESS, HSM_TUBE_DIFFUSE_COLORIZE_ON, HSM_TUBE_DIFFUSE_GAMMA);
	}
	tube_diffuse *= HSM_TUBE_DIFFUSE_OPACITY;
	tube_diffuse.rgb *= night_lighting_image.rgb;


	// Apply the CRT image additively to the tube diffuse color
	out_color.a = in_color_with_gamma.a;
	out_color = HSM_BlendModeLayerMix(tube_diffuse, out_color, BLEND_MODE_ADD, 1);

	#ifdef IS_NOREFLECT_PRESET
	out_color.rgb *= HSM_Get_Screen_Black_Edge_Mask(SCREEN_COORD, false);

	// TUBE HIGHLIGHT
	if (HSM_TUBE_STATIC_REFLECTION_IMAGE_OPACITY > 0)
		out_color += HSM_Get_Tube_Highlight(tube_curved_coord, false, TubeStaticReflectionImage);
	#endif

	#ifndef IS_NOREFLECT_PRESET
	// Apply the final Gamma
	out_color = HSM_Delinearize(out_color, HSM_DEFAULT_SRGB_GAMMA);
	#endif

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	TUBE_MASK = HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);

	return out_color;
}

void main()
{
	vec2 viewportCoordTransformed = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, InfoCachePassFeedback, vTexCoord);

	if (HSM_CACHE_OUTSIDE_FRAME > 0.5)
	{
		// Have to get the scale of the coordinates so we can figure out the size of the onscreen rectangle of the area 
		HSM_GetBezelCoords(SCREEN_COORD, 
							SCREEN_SCALE, 
							TUBE_SCALE, 
							SCREEN_ASPECT, 
							false,
							BEZEL_OUTSIDE_SCALE,
							BEZEL_OUTSIDE_COORD, 
							BEZEL_OUTSIDE_CURVED_COORD, 
							FRAME_OUTSIDE_CURVED_COORD);

		FRAME_OUTSIDE_CURVED_COORD = (FRAME_OUTSIDE_CURVED_COORD - 0.5) * 0.9 + 0.5;
		if (FRAME_OUTSIDE_CURVED_COORD.x < -0.01 || FRAME_OUTSIDE_CURVED_COORD.x > 1.01 || FRAME_OUTSIDE_CURVED_COORD.y < -0.01 || FRAME_OUTSIDE_CURVED_COORD.y > 1.01)
		{
				FragColor = vec4(0);
				return;
		}
	}

	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);

	FragColor = texture(Source, UNFLIPPED_VIEWPORT_COORD);

#ifdef IS_POTATO_PRESET
	FragColor = HSM_GetPostCrtPreppedColorPotato(FragColor, vTexCoord, screen_curved_coord, BackgroundImage, BackgroundVertImage);
#else
	FragColor = HSM_GetPostCrtPreppedColor(FragColor, vTexCoord, screen_curved_coord, IntroPass, false, TubeDiffuseImage, TubeColoredGelImage, TubeStaticReflectionImage, BackgroundImage, BackgroundVertImage, NightLightingImage);
#endif

}
