#version 450

/*
   G-sharp deblur - dynamic range, resizable
   
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
   
*/ 

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float DRANGE;
	float DEBLUR1;
	float DSTEP;
	float ARR;
} params;

#pragma parameter bogus-line2 "-------------------------------------------------" 0.0 0.0 0.0 0.0 

#pragma parameter DRANGE "Deblur Kernel Size" 2.0 1.0 10.0 1.0
#define DRANGE params.DRANGE

#pragma parameter DSTEP "Deblur Range Step" 2.0 1.0 5.0 1.0
#define DSTEP params.DSTEP

#pragma parameter DEBLUR1 "Deblur Strength" 0.0 0.0 25.0 1.0
#define DEBLUR1 params.DEBLUR1

#pragma parameter ARR "Deblur Anti-ringing Radius" 1.0 0.5 4.0 0.5
#define ARR params.ARR

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

void main()
{
	vec3 color = COMPAT_TEXTURE(Source, vTexCoord).rgb;

	if (DEBLUR1 > 0.0)
	{
		vec2 dx  = vec2(SourceSize.z, 0.0);	
		vec2 dy  = vec2(0.0, SourceSize.w);

		vec3 pixel;
		float x;
		vec3  res;
		float w;
		vec3 cmax = 0.0.xxx;
		vec3 cmin = 1.0.xxx;
		
		float LOOPSIZE = DSTEP*DRANGE;
		float y = -LOOPSIZE;

		do
		{
			x = -LOOPSIZE;
	
			do
			{
				pixel  = COMPAT_TEXTURE(Source, vTexCoord + x*dx + y*dy).rgb;
				cmax = max(cmax, pixel); cmin = min(cmin, pixel);
				x = x + DSTEP;
			
			} while (x <= LOOPSIZE);
		
			y = y + DSTEP;
		
		} while (y <= LOOPSIZE);	
	
		vec3 contrast = cmax - cmin + 0.000001;
		vec3 df1 = max(color-cmin, 0.0);
		vec3 df2 = max(cmax-color, 0.0);
		vec3 ratio = abs(df1-df2)/(df1+df2+0.0000001);
	
		df1 = max(smoothstep( -0.0001, 1.0, df1/contrast), 0.0001);
		df2 = max(smoothstep( -0.0001, 1.0, df2/contrast), 0.0001);

		res = mix(color, (cmax*df1 + cmin*df2)/(df1+df2), DEBLUR1*ratio);
		
		vec3 c00 = COMPAT_TEXTURE(Source, vTexCoord  -dx  -dy).rgb;
		vec3 c20 = COMPAT_TEXTURE(Source, vTexCoord  +dx  -dy).rgb;		
		vec3 c02 = COMPAT_TEXTURE(Source, vTexCoord  -dx  +dy).rgb;		
		vec3 c22 = COMPAT_TEXTURE(Source, vTexCoord  +dx  +dy).rgb;	

		vec3 edge = abs(c00-c22) + abs(c20-c02);
		
		float e = max(max(edge.r,edge.g),edge.b);
		e = smoothstep(0.0, 5.0/255.0, e);
		
		color = mix(color, res, e);
		
		float mx = 0.0;
		float nx = 3.0;
		vec3 nmin;
		vec3 nmax;
		nmax = 0.0.xxx;
		nmin = 1.0.xxx;
		
		dx = vec2(1.0/1920.0, 0.0);
		dy = vec2(0.0, 1.0/1080.0);

		LOOPSIZE = ARR;
		
		y = -LOOPSIZE;
		
		do
		{
			x = -LOOPSIZE;
	
			do
			{
				pixel  = COMPAT_TEXTURE(Source, vTexCoord + x*dx + y*dy).rgb;
				nmin = min(nmin, pixel);
				nmax = max(nmax, pixel);
				x = x + 1.0;
			
			} while (x <= LOOPSIZE);
		
			y = y + 1.0;
		
		} while (y <= LOOPSIZE);

		color = clamp(color, nmin, nmax);		
	}	

	FragColor = vec4(color, 1.0);
}
