// #include "../hsm/hsm-common-functions-bezel.inc"

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



void main()
{
	vec2 viewportCoordTransformed = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, InfoCachePassFeedback, vTexCoord);

	// if (HSM_CACHE_OUTSIDE_FRAME > 0.5)
	// {
	// 	// Have to get the scale of the coordinates so we can figure out the size of the onscreen rectangle of the area 
	// 	HSM_GetBezelCoords(SCREEN_COORD, 
	// 						SCREEN_SCALE, 
	// 						TUBE_SCALE, 
	// 						SCREEN_ASPECT, 
	// 						false,
	// 						BEZEL_OUTSIDE_SCALE,
	// 						BEZEL_OUTSIDE_COORD, 
	// 						BEZEL_OUTSIDE_CURVED_COORD, 
	// 						FRAME_OUTSIDE_CURVED_COORD);

	// 	FRAME_OUTSIDE_CURVED_COORD = (FRAME_OUTSIDE_CURVED_COORD - 0.5) * 0.9 + 0.5;
	// 	if (FRAME_OUTSIDE_CURVED_COORD.x < -0.01 || FRAME_OUTSIDE_CURVED_COORD.x > 1.01 || FRAME_OUTSIDE_CURVED_COORD.y < -0.01 || FRAME_OUTSIDE_CURVED_COORD.y > 1.01)
	// 	{
	// 			FragColor = vec4(0);
	// 			return;
	// 	}
	// }

	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);

	FragColor = texture(Source, UNFLIPPED_VIEWPORT_COORD);

#ifdef IS_POTATO_PRESET
	FragColor = HSM_GetPostCrtPreppedColorPotato(FragColor, vTexCoord, screen_curved_coord, BackgroundImage, BackgroundVertImage);
#else
	FragColor = HSM_GetPostCrtPreppedColor(FragColor, vTexCoord, screen_curved_coord, IntroPass, false, TubeDiffuseImage, TubeColoredGelImage, TubeStaticReflectionImage, BackgroundImage, BackgroundVertImage, NightLightingImage);
#endif

}
