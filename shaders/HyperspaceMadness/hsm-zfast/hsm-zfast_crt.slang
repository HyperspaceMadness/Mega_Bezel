#version 450

/*
    zfast_crt_standard - A simple, fast CRT shader.

    Copyright (C) 2017 Greg Hogan (SoltanGris42)

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.


Notes:  This shader does scaling with a weighted linear filter for adjustable
	sharpness on the x and y axes based on the algorithm by Inigo Quilez here:
	http://http://www.iquilezles.org/www/articles/texture/texture.htm
	but modified to be somewhat sharper.  Then a scanline effect that varies
	based on pixel brighness is applied along with a monochrome aperture mask.
	This shader runs at 60fps on the Raspberry Pi 3 hardware at 2mpix/s
	resolutions (1920x1080 or 1600x1200).
*/

#include "../hsm/hsm-globals-and-screen-scale-params.inc"

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
   float BLURSCALEX, LOWLUMSCAN, HILUMSCAN, BRIGHTBOOST, MASK_DARK, MASK_FADE;
} params;

#pragma parameter ZFS_ZFAST_TITLE       "[ --- ZFAST--- ]:" 0 0 0 1
#pragma parameter BLURSCALEX 			"          Blur Amount X-Axis" 0.30 0.0 1.0 0.05
#pragma parameter LOWLUMSCAN 			"          Scanline Darkness - Low" 6.0 0.0 10.0 0.5
#pragma parameter HILUMSCAN 			"          Scanline Darkness - High" 8.0 0.0 50.0 1.0
#pragma parameter BRIGHTBOOST 			"          Dark Pixel Brightness Boost" 1.25 0.5 1.5 0.05
#pragma parameter MASK_DARK 			"          Mask Effect Amount" 0.25 0.0 1.0 0.05
#pragma parameter MASK_FADE 			"          Mask/Scanline Fade" 0.8 0.0 1.0 0.05

#define BLURSCALEX params.BLURSCALEX
#define LOWLUMSCAN params.LOWLUMSCAN
#define HILUMSCAN params.HILUMSCAN
#define BRIGHTBOOST params.BRIGHTBOOST
#define MASK_DARK params.MASK_DARK
#define MASK_FADE params.MASK_FADE

//For testing compilation 
//#define FRAGMENT
//#define VERTEX

//This can't be an option without slowing the shader down
//Comment this out for a coarser 3 pixel mask...which is currently broken
//on SNES Classic Edition due to Mali 400 gpu precision
#define FINEMASK
//Some drivers don't return black with texture coordinates out of bounds
//SNES Classic is too slow to black these areas out when using fullscreen
//overlays.  But you can uncomment the below to black them out if necessary
//#define BLACK_OUT_BORDER

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out float maskFade;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.0001;
	
	if (HSM_FLIP_VIEWPORT_VERTICAL == 1)
		vTexCoord.y = 1 - vTexCoord.y;
		
	if (HSM_FLIP_VIEWPORT_HORIZONTAL > 0.5)
		vTexCoord.x =  1 - vTexCoord.x;

	maskFade = 0.3333 * MASK_FADE;
	
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in float maskFade;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 1) uniform sampler2D Source;

layout(set = 0, binding = 2) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 3) uniform sampler2D InfoCachePassFeedback;

void main()
{
    // HSM Added
    HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, InfoCachePassFeedback, vTexCoord);
	
	vec2 invDims = 1.0 / CROPPED_ROTATED_SIZE_WITH_RES_MULT;

    vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);
	screen_curved_coord = HSM_GetMirrorWrappedCoord(screen_curved_coord);

	//This is just like "Quilez Scaling" but sharper
	vec2 p = screen_curved_coord * CROPPED_ROTATED_SIZE_WITH_RES_MULT;
	vec2 i = floor(p) + 0.50;
	vec2 f = p - i;
	p = (i + 4.0*f*f*f)*invDims;
	p.x = mix( p.x , screen_curved_coord.x, BLURSCALEX);
	float Y = f.y*f.y;
	float YY = Y*Y;
	
#if defined(FINEMASK) 
	float whichmask = fract(floor(vTexCoord.x*params.OutputSize.x*-0.4999));
	float mask = 1.0 + float(whichmask < 0.5) * -MASK_DARK;
#else
	float whichmask = fract(floor(vTexCoord.x*params.OutputSize.x)*-0.3333);
	float mask = 1.0 + float(whichmask <= 0.33333) * -MASK_DARK;
#endif
	vec3 colour = HSM_GetCroppedTexSample(Source, p).rgb;
	
	float scanLineWeight = (BRIGHTBOOST - LOWLUMSCAN*(Y - 2.05*YY));
	float scanLineWeightB = 1.0 - HILUMSCAN*(YY-2.8*YY*Y);	
	
#if defined(BLACK_OUT_BORDER)
	colour.rgb*=float(tc.x > 0.0)*float(tc.y > 0.0); //why doesn't the driver do the right thing?
#endif

	FragColor.rgb = colour.rgb*mix(scanLineWeight*mask, scanLineWeightB, dot(colour.rgb,vec3(maskFade)));
}