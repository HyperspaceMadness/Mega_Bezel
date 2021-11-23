/*   
hsm-image-layers

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL

*/

/////////////// IMPORTS ///////////////
#include "hsm-globals-and-no-reflect-params.inc"
#include "hsm-common-functions-bezel.inc"


vec2 UNFLIPPED_VIEWPORT_COORD = vec2(0.5);
vec2 FLIPPED_VIEWPORT_COORD = vec2(0.5);

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;
layout(location = 8) out vec3 BEZEL_FRAME_ORIGINAL_COLOR_RGB;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.0001;

	// Not sure why we need linearize this but it seems to have a smoother range this way
	BEZEL_FRAME_ORIGINAL_COLOR_RGB = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), HSM_DEFAULT_SRGB_GAMMA).rgb;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 2) uniform sampler2D InfoCachePassFeedback;

layout(set = 0, binding = 3) uniform sampler2D BR_LayersUnderCRTPass;

#ifdef IS_NO_REFLECT_PRESET
layout(set = 0, binding = 4) uniform sampler2D MBZ_PostCRTPass;
#define MiddlePass MBZ_PostCRTPass
#else
layout(set = 0, binding = 4) uniform sampler2D BR_CRTAndReflectionPass;
#define MiddlePass BR_CRTAndReflectionPass
#endif

layout(set = 0, binding = 5) uniform sampler2D BR_LayersOverCRTPass;

layout(set = 0, binding = 6) uniform sampler2D CombinePassFeedback;
#define PassFeedback CombinePassFeedback

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	UNFLIPPED_VIEWPORT_COORD = vTexCoord;
	vec2 VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, InfoCachePassFeedback, vTexCoord);


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

	if (BEZEL_OUTSIDE_COORD.x < -0.01 || BEZEL_OUTSIDE_COORD.x > 1.01 || BEZEL_OUTSIDE_COORD.y < -0.01 || BEZEL_OUTSIDE_COORD.y > 1.01)
	{
		vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
		if (HSM_LIVE_UPDATE_ON < 0.5 && feedback_color_test.a < 0 && !HSM_CheckCacheInfoChanged() && HSM_RENDER_SIMPLE_MODE < 0.5)
		{
			FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
			return;
		}
	}

	vec4 out_color = texture(BR_LayersUnderCRTPass, vTexCoord);
	vec4 crt_layer = texture(MiddlePass, vTexCoord);
	if (HSM_RENDER_SIMPLE_MODE > 0.5)
		crt_layer.rgb = vec3(0);

	// vec4 reflection_layer = crt_layer;

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);

	TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	// crt_layer *= TUBE_MASK;

	// float TUBE_MASK_EXPAND = HSM_GetCornerMask((tube_curved_coord - 0.5) * 0.997 + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	// crt_layer *= TUBE_MASK_EXPAND;

	float TUBE_MASK_TRIM = HSM_GetCornerMask((tube_curved_coord - 0.5) * 1.003 + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	// crt_layer.rgb *= TUBE_MASK_TRIM;

	// // If the tube is opaque apply it on top of the under CRT layers
	// vec4 tube_background = vec4(0, 0, 0, TUBE_MASK_EXPAND) * HSM_TUBE_DIFFUSE_IMAGE_OPACITY;
	// out_color = HSM_PreMultAlphaBlend(out_color, tube_background);

	// out_color.rgb += crt_layer.rgb;
	if (HSM_RENDER_SIMPLE_MODE < 0.5)
		out_color.rgb += crt_layer.rgb;

	// if (HSM_CRT_SCREEN_BLEND_MODE > 1.5)
	// {
	// 	crt_layer.rgb *= crt_layer.a;
	// 	out_color.rgb += crt_layer.rgb;
	// }
	// else if (HSM_CRT_SCREEN_BLEND_MODE > 0.5)
	// 	out_color = HSM_PreMultAlphaBlend(out_color, crt_layer);

// #ifndef IS_NO_REFLECT_PRESET
// 	float outside_tube_mask_smaller = 1 - HSM_GetCornerMask((tube_curved_coord - 0.5) * 1.01 + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0.99);
// 	if (HSM_RENDER_SIMPLE_MODE < 0.5)
// 	{
// 		reflection_layer *= 1 - TUBE_MASK_TRIM;
// 		out_color.rgb += reflection_layer.rgb;
// 	}
// #endif

	vec4 over_layer = texture(BR_LayersOverCRTPass, vTexCoord);
	out_color = HSM_PreMultAlphaBlend(out_color, over_layer);

	FragColor = HSM_ApplyGamma(clamp(out_color, 0, 1), HSM_DEFAULT_SRGB_GAMMA);

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		SCREEN_BLACK_EDGE_CURVED_COORD = HSM_GetCurvedCoord(SCREEN_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, SCREEN_ASPECT);
		SCREEN_MASK = HSM_GetCornerMask(((SCREEN_BLACK_EDGE_CURVED_COORD - 0.5) * 1.001) + 0.5, SCREEN_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE, HSM_TUBE_BLACK_EDGE_SHARPNESS);

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_TUBE )
			FragColor = HSM_PreMultAlphaBlend(FragColor, vec4(0, 0, 0, 1) * (TUBE_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_SCREEN )
			FragColor = HSM_PreMultAlphaBlend(FragColor, vec4(1, 0, 1, 1) * (SCREEN_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_TUBE )
			FragColor = HSM_PreMultAlphaBlend(FragColor, vec4(1, 0, 1, 1) * (TUBE_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_OUTSIDE_TUBE_MASK )
			FragColor = vec4(1 - TUBE_MASK);

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_OUTSIDE_SCREEN_MASK )
			FragColor = vec4(1 - SCREEN_MASK);
	}
	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
		FragColor = vec4(0.01, 0.01, 0.01, -1);
		
	return;
}
































