#version 450

/*
   G-sharp resampler - horizontal pass, dynamic range, resizable
   
   Copyright (C) 2020 guest(r) - guest.r@gmail.com

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

   Anti-Ringing inspired by Hyllian
   
*/ 


layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float SIGMA_GH;
	float HSHARP0;
	float HAR;
	float HSHAR;
} params;

#pragma parameter HSHARP0 "Horizontal Filter Range" 1.2 1.0 8.0 0.1
#define HSHARP0 params.HSHARP0

#pragma parameter SIGMA_GH "Horizontal Blur Sigma" 0.75 0.1 7.0 0.05
#define SIGMA_GH params.SIGMA_GH

#pragma parameter HSHAR "Sharpness Definition - Horizontal" 0.5 0.0 2.0 0.05
#define HSHAR params.HSHAR

#pragma parameter HAR "Horizontal Anti-Ringing" 0.5 0.0 1.0 0.1
#define HAR params.HAR

layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
} global;

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
layout(set = 0, binding = 2) uniform sampler2D Source;

#define COMPAT_TEXTURE(c,d) texture(c,d)
#define SourceSize params.SourceSize

float invsqrsigma = 1.0/(2.0*SIGMA_GH*SIGMA_GH);

float gaussian(float x)
{
	return exp(-x*x*invsqrsigma);
}

void main()
{
	float f = fract(SourceSize.x * vTexCoord.x);
	f = 0.5 - f;
	vec2 tex = floor(SourceSize.xy * vTexCoord)*SourceSize.zw + 0.5*SourceSize.zw;
	vec3 color = 0.0.xxx;
	vec2 dx  = vec2(SourceSize.z, 0.0);

	float w;
	float wsum = 0.0;
	vec3 pixel;

	vec3 cmax = 0.0.xxx;
	vec3 cmin = 1.0.xxx;
	float sharp = gaussian(HSHARP0);
	float maxsharp = 0.21;
	float FPR = HSHARP0;
	float fpx;

	float LOOPSIZE = ceil(2.0*FPR);	
	float n = -LOOPSIZE;
	
	do
	{
		pixel  = COMPAT_TEXTURE(Source, tex + n*dx).rgb;
		w = gaussian(n+f) - sharp; 
		fpx = abs(n+f-sign(n)*FPR)/FPR;
		if (abs(n) <= FPR) { cmax = max(cmax, pixel); cmin = min(cmin, pixel); }
		if (w < 0.0) w = clamp(w, mix(-maxsharp, 0.0, pow(fpx, HSHAR)), 0.0);
	
		color = color + w * pixel;
		wsum  = wsum+ w;
		n = n + 1.0;
			
	} while (n <= LOOPSIZE);

	color = color / wsum;
	
	color = mix(clamp(color, 0.0, 1.0), clamp(color, cmin, cmax), HAR);
		
	FragColor = vec4(color, 1.0);
}