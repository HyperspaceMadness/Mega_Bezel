/*
   CRT - Guest - Nomask w. Curvature 
   With work by DariusG to create a cut down extra fast version
   
   Copyright (C) 2017-2018 guest(r) - guest.r@gmail.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
   
*/

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;

    float scanline;
    float beam_min;
    float beam_max;
    float h_sharp;
    float gamma_out;
    float maskDark;
    float CGWG;
} params;

// Parameter lines go here:
#pragma parameter gdv_mini_title 	"[ GDV MINI - DariusG ]:" 0.0 0.0 1.0 1.0

#pragma parameter scanline          "          Scanline Adjust - scanline" 8 1 12 1
#define scanline params.scanline

#pragma parameter beam_min          "          Scanline Dark - beam_min" 1.7 0.5 3 0.05
#define beam_min params.beam_min

#pragma parameter beam_max          "          Scanline Bright - beam_max" 2.1 0.5 3 0.05
#define beam_max params.beam_max

#pragma parameter h_sharp           "          Horizontal Sharpness - h_sharp" 2 1 5 0.05
#define h_sharp params.h_sharp

// #pragma parameter gamma_out "Gamma out" 0.5 0.2 0.6 0.01
// #define gamma_out params.gamma_out

#pragma parameter maskDark          "          Lottes maskDark" 0.5 0 2 0.1
#define maskDark params.maskDark

#pragma parameter CGWG              "          CGWG Mask Str. - CGWG" 0.4 0 1 0.1
#define CGWG params.CGWG

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord * 1.0001;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in float maskFade;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 1) uniform sampler2D Source;

layout(set = 0, binding = 2) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 3) uniform sampler2D InfoCachePassFeedback;

float sw(float x, float l)
{
	float d = x;
	float bm = scanline;
	float b = mix(beam_min,beam_max,l);
	d = exp2(-bm*pow(d,b));
	return d;
}

vec3 Mask(vec2 pos)
{
   vec3 mask = vec3(maskDark, maskDark, maskDark);

   // Phosphor.
  
   {
      float mf = floor(mod(pos.x,2.0));
      float mc = 1.0 - CGWG;	
      if (mf == 0.0) { mask.r = 1.0; mask.g = mc; mask.b = 1.0; }
      else { mask.r = mc; mask.g = 1.0; mask.b = mc; };
   }    
   
      return mask;
} 

void main()
{
    // HSM Added
	vec2 viewportCoordTransformed = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, InfoCachePassFeedback, vTexCoord);

	vec2 cache_bounds_coord = SCREEN_COORD;

// If it's the potato preset render the whole frame
#ifndef IS_POTATO_PRESET
#ifndef IS_NO_REFLECT_PRESET
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
	cache_bounds_coord = (FRAME_OUTSIDE_CURVED_COORD - 0.5) * 0.9 + 0.5;
#endif
#endif

	if (cache_bounds_coord.x < -0.01 || cache_bounds_coord.x > 1.01 || cache_bounds_coord.y < -0.01 || cache_bounds_coord.y > 1.01)
	{
			FragColor = vec4(0);
			return;
	}
	
	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);
	vec2 pos = HSM_GetMirrorWrappedCoord(screen_curved_coord);

	// HSM Added
	vec4 SourceSize = vec4(CROPPED_ROTATED_SIZE_WITH_RES_MULT, 1/CROPPED_ROTATED_SIZE_WITH_RES_MULT);

	vec2 ps = SourceSize.zw;
	vec2 OGL2Pos = pos * SourceSize.xy;
	vec2 fp = fract(OGL2Pos);
	vec2 dx = vec2(ps.x,0.0);
	vec2 dy = vec2(0.0, ps.y);

	vec2 pC4 = floor(OGL2Pos) * ps + 0.5*ps;
	
	// Reading the texels
	vec3 ul = HSM_GetCroppedTexSample(Source, pC4     ).xyz; ul*=ul;
	vec3 ur = HSM_GetCroppedTexSample(Source, pC4 + dx).xyz; ur*=ur;
	vec3 dl = HSM_GetCroppedTexSample(Source, pC4 + dy).xyz; dl*=dl;
	vec3 dr = HSM_GetCroppedTexSample(Source, pC4 + ps).xyz; dr*=dr;
	
	float lx = fp.x;        lx = pow(lx, h_sharp);
	float rx = 1.0 - fp.x;  rx = pow(rx, h_sharp);
	
	vec3 color1 = (ur*lx + ul*rx)/(lx+rx);
	vec3 color2 = (dr*lx + dl*rx)/(lx+rx);

// calculating scanlines
	
	float f = fp.y;
	float luma1 = length(color1)*0.57735;
	float luma2 = length(color2)*0.57735;
	
	vec3 color = color1*sw(f,luma1) + color2*sw(1.0-f,luma2);
	
	color = min(color, 1.0);

	// color = pow(color, vec3(gamma_out, gamma_out, gamma_out));
	

    // TODO Add adjust to take into acount Gamma IN
    color = pow(color, vec3(1.0/(DEFAULT_SRGB_GAMMA * HSM_GAMMA_IN_LINEARIZE / (0.9 * HSM_GAMMA_OUT_CRT))));

	float l = length(color);
	color = color * Mask(vTexCoord * global.OutputSize.xy);	

    color = pow(color, vec3(DEFAULT_SRGB_GAMMA / (HSM_GAMMA_OUT_CRT * 0.8)));
	
    color *= 1.2;

    FragColor = vec4(color, 1.0);
}
