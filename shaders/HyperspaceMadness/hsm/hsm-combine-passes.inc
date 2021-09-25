/*   
hsm-image-layers

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL

*/

/////////////// IMPORTS ///////////////
#include "hsm-globals-and-no-reflect-params.inc"
#include "hsm-common-functions-bezel.inc"


vec2 VIEWPORT_COORD = vec2(0.5);

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;
layout(location = 7) out vec2 UNFLIPPED_VIEWPORT_COORD;
layout(location = 8) out vec3 BEZEL_FRAME_ORIGINAL_COLOR_RGB;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	UNFLIPPED_VIEWPORT_COORD = vTexCoord * 1.0001;

	if (HSM_FLIP_VIEWPORT_VERTICAL > 0.5)
		vTexCoord.y =  1 - vTexCoord.y;

	if (HSM_FLIP_VIEWPORT_HORIZONTAL > 0.5)
		vTexCoord.x =  1 - vTexCoord.x;

	VIEWPORT_COORD = vTexCoord * 1.0001;

	// Not sure why we need linearize this but it seems to have a smoother range this way
	BEZEL_FRAME_ORIGINAL_COLOR_RGB = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), HSM_DEFAULT_SRGB_GAMMA).rgb;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;
layout(location = 7) in vec2 UNFLIPPED_VIEWPORT_COORD;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 2) uniform sampler2D BR_LayersUnderCRTPass;

#ifdef IS_NO_REFLECT_PRESET
layout(set = 0, binding = 3) uniform sampler2D MBZ_PostCRTPass;
#define MiddlePass MBZ_PostCRTPass
#else
layout(set = 0, binding = 3) uniform sampler2D BR_CRTAndReflectionPass;
#define MiddlePass BR_CRTAndReflectionPass
#endif

layout(set = 0, binding = 4) uniform sampler2D BR_LayersOverCRTPass;

layout(set = 0, binding = 5) uniform sampler2D CombinePassFeedback;
#define PassFeedback CombinePassFeedback


//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	VIEWPORT_COORD = vTexCoord;
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, VIEWPORT_COORD);

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

	if (BEZEL_OUTSIDE_COORD.x < -0.01 || BEZEL_OUTSIDE_COORD.x > 1.01 || BEZEL_OUTSIDE_COORD.y < -0.01 || BEZEL_OUTSIDE_COORD.y > 1.01)
	{
		vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
		if (HSM_LIVE_UPDATE_ON < 0.5 && feedback_color_test.a > 0.001)
		{
			FragColor = texture(PassFeedback, UNFLIPPED_VIEWPORT_COORD);
			//TODO add Caching Debug
			// - vec4(0.0,0.05,0.05,0);
			return;
		}
	}

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_SCREEN_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(SCREEN_COORD, 1, SCREEN_SCALE, TUBE_SCALE, SCREEN_ASPECT, 1);

	vec4 out_color = texture(BR_LayersUnderCRTPass, vTexCoord);

	vec4 crt_layer = texture(MiddlePass, vTexCoord);
	vec4 reflection_layer = crt_layer;
	if (HSM_CRT_SCREEN_BLEND_MODE > 1.5)
	{
		crt_layer.rgb *= crt_layer.a;
		out_color.rgb += crt_layer.rgb;
	}
	else if (HSM_CRT_SCREEN_BLEND_MODE > 0.5)
		out_color = HSM_PreMultAlphaBlend(out_color, crt_layer);


#ifndef IS_NO_REFLECT_PRESET
	float outside_tube_mask_smaller = 1 - HSM_GetCornerMask((tube_curved_coord - 0.5) * 1.01 + 0.5, SCREEN_ASPECT, bezel_corner_radius, 0.99);
	reflection_layer *= outside_tube_mask_smaller;
	out_color.rgb += reflection_layer.rgb;
#endif

	vec4 over_layer = texture(BR_LayersOverCRTPass, vTexCoord);
	out_color = HSM_PreMultAlphaBlend(out_color, over_layer);

	FragColor = HSM_ApplyGamma(clamp(out_color, 0, 1), HSM_DEFAULT_SRGB_GAMMA);

	if (UNFLIPPED_VIEWPORT_COORD.x < (2 / global.OutputSize.x) && UNFLIPPED_VIEWPORT_COORD.y < (2 / global.OutputSize.y))
		FragColor = vec4(0.01, 0.01, 0.01, 10);

	return;
}
































