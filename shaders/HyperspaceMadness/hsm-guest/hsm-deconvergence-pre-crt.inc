/*
   CRT - Guest - Advanced - Deconvergence pass + noise
   
   Copyright (C) 2021 guest(r) - guest.r@gmail.com
   
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

#include "../hsm/hsm-globals-and-screen-scale-params.inc"

layout(push_constant) uniform Push
{
	vec4 FinalViewportSize;
	uint FrameCount;
	float GDV_PRECRT_DECONVERGENCE_ON;
	float precrt_decons;
	float precrt_deconrr;
	float precrt_deconrg;
	float precrt_deconrb;
	float precrt_deconrry;
	float precrt_deconrgy;
	float precrt_deconrby;	
	float precrt_deconsmooth;
	float precrt_dctypex;
	float precrt_dctypey;
} params;

// #pragma parameter TATE "          TATE Mode" 0.0 0.0 1.0 1.0
// #define TATE         params.TATE     // Screen orientation

#pragma parameter bogus_deconvergence_pre_crt 	"[ PRE-CRT DECONVERGENCE ]: " 0.0 0.0 1.0 1.0

#pragma parameter GDV_PRECRT_DECONVERGENCE_ON 	"          Deconvergence ON" 0 0 1 1
#define GDV_PRECRT_DECONVERGENCE_ON params.GDV_PRECRT_DECONVERGENCE_ON

#pragma parameter precrt_dctypex 				"          Type X : 0.0 - static, other - dynamic -- precrt_dctypex" 0.0 0.0 1.0 0.05

#pragma parameter precrt_dctypey 				"          Type Y : 0.0 - static, other - dynamic -- precrt_dctypey" 0.0 0.0 1.0 0.05

#pragma parameter precrt_deconrr 				"          Horizontal Red Range -- precrt_deconrr" -1.0 -7.0 7.0 0.25

#pragma parameter precrt_deconrg 				"          Horizontal Green Range -- precrt_deconrg" 0.0 -7.0 7.0 0.25

#pragma parameter precrt_deconrb 				"          Horizontal Blue Range -- precrt_deconrb" 1.0 -7.0 7.0 0.25

#pragma parameter precrt_deconrry 				"          Vertical Red Range -- precrt_deconrry" -1.0 -12.0 12.0 0.25

#pragma parameter precrt_deconrgy 				"          Vertical Green Range -- precrt_deconrgy" 0.0 -12.0 12.0 0.25

#pragma parameter precrt_deconrby 				"          Vertical Blue Range -- precrt_deconrby" 1.0 -12.0 12.0 0.25

// Default was 0.5
#pragma parameter precrt_decons 				"          Strength (and Type) -- precrt_decons" 2 -4.0 4.0 0.10
#define precrt_decons        params.precrt_decons     // Horizontal deconvergence colors strength

#pragma parameter precrt_deconsmooth 			"          Smoothing -- precrt_decons" 0.0 0.0 1.0 0.10


/*
layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
	vec2 FinalViewportSize;
} global;
*/

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
layout(set = 0, binding = 1) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 2) uniform sampler2D Source;

#define COMPAT_TEXTURE(c,d) texture(c,d)


const float masksizes[10] = float[10](2.0, 2.0, 3.0, 3.0, 3.0, 3.0, 2.0, 3.0, 2.0, 4.0);

vec3 plant (vec3 tar, float r)
{
	float t = max(max(tar.r,tar.g),tar.b) + 0.00001;
	return tar * r / t;
}

void main()
{
	if (GDV_PRECRT_DECONVERGENCE_ON < 0.5)
	{
		FragColor = texture(Source, vTexCoord);
		return;
	}

	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, vTexCoord);
	float TATE = USE_VERTICAL_SCANLINES;

	vec3 color = COMPAT_TEXTURE(Source, vTexCoord).rgb;
	vec3 result = color;
	
	//GDV_DECONVERGENCE_IS_PRE_CRT

	if ((abs(params.precrt_deconrr) + abs(params.precrt_deconrg) + abs(params.precrt_deconrb) + abs(params.precrt_deconrry) + abs(params.precrt_deconrgy) + abs(params.precrt_deconrby)) > 0.20)
	{	
		float step = 1;

		step*= (TATE < 0.5) ? (params.FinalViewportSize.z) : (params.FinalViewportSize.w);
		float stepy = (TATE < 0.5) ? (params.FinalViewportSize.w) : (params.FinalViewportSize.z);
		float stepx = (TATE < 0.5) ? (params.FinalViewportSize.z) : (params.FinalViewportSize.w);
		
		vec2 sx = mix(vec2(stepx, 0.0), vec2(0.0, stepx), TATE);
		
		float ds = precrt_decons;

		vec2 dx = (TATE < 0.5) ? vec2(step, 0.0) :  vec2(0.0, step);
		vec2 dy = (TATE > 0.5) ? vec2(stepy, 0.0) :  vec2(0.0, stepy);		

		float posx = 2.0*vTexCoord.x - 1.0;
		float posy = 2.0*vTexCoord.y - 1.0;

		if (params.precrt_dctypex > 0.0)
		{
			posx = sign(posx)*pow(abs(posx), 1.05-params.precrt_dctypex);
			dx = posx * dx;
		}

		if (params.precrt_dctypey > 0.0)
		{

			posy = sign(posy)*pow(abs(posy), 1.05-params.precrt_dctypey);
			dy = posy * dy;
		}

		if (params.precrt_dctypex > 0.0 || params.precrt_dctypey > 0.0) ds *= sqrt(posx*posx*sign(params.precrt_dctypex) + posy*posy*sign(params.precrt_dctypey));

		vec2 rc = params.precrt_deconrr * dx + params.precrt_deconrry*dy;
		vec2 gc = params.precrt_deconrg * dx + params.precrt_deconrgy*dy;
		vec2 bc = params.precrt_deconrb * dx + params.precrt_deconrby*dy;		

		dx = (dx+dy) * params.precrt_deconsmooth;
		
		float r1 = COMPAT_TEXTURE(Source, vTexCoord + rc    ).r;
		float g1 = COMPAT_TEXTURE(Source, vTexCoord + gc    ).g;
		float b1 = COMPAT_TEXTURE(Source, vTexCoord + bc    ).b;

		float r2 = COMPAT_TEXTURE(Source, vTexCoord + rc -dx).r;
		float g2 = COMPAT_TEXTURE(Source, vTexCoord + gc -dx).g;
		float b2 = COMPAT_TEXTURE(Source, vTexCoord + bc -dx).b;

		float r3 = COMPAT_TEXTURE(Source, vTexCoord + rc +dx).r;
		float g3 = COMPAT_TEXTURE(Source, vTexCoord + gc +dx).g;
		float b3 = COMPAT_TEXTURE(Source, vTexCoord + bc +dx).b;

		vec3 result1 = vec3(r1,g1,b1);
		vec3 result2 = vec3(r2,g2,b2);
		vec3 result3 = vec3(r3,g3,b3);
		result = (result1+result2+result3)/3.0;
		
		vec3 dcolor = max(max(COMPAT_TEXTURE(Source, vTexCoord + sx).rgb, COMPAT_TEXTURE(Source, vTexCoord - sx).rgb), color);
		
		float mc = max(max(dcolor.r, dcolor.g), dcolor.b);
		if (precrt_decons < 0.0) mc = 0.9;
		
		result = clamp(mix(color, sqrt(mix(result*result, color*result, sqrt(mc))), abs(ds)), min(result,color), max(result, color));
	}

	// HSM Added
	FragColor = vec4(result, 1.0);
}