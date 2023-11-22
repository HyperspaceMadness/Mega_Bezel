#version 450

/*
   CRT - Guest - Advanced - Lite
   
   Copyright (C) 2018-2023 guest(r) - guest.r@gmail.com

   Incorporates many good ideas and suggestions from Dr. Venom.
   I would also like give thanks to many Libretro forums members for continuous feedback, suggestions and caring about the shader.
   
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
	float TATE, brightboost, brightboost1, scanline1b, scanline2b, scanline1d, scanline2d, beam_min, beam_max, 
	h_sharp, s_sharp, csize, bsize1, warpX, warpY, glow, no_scanlines, clips, bloom, halation, scans, beam_size, IOS;
} params;

layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float gamma_c;
	float smart_ei;
	float ei_limit;
	float sth;
	float overscanX;
	float overscanY;
	float c_shape;
	float intres;
	float prescalex;
	float scan_falloff;
	float bloom_dist;
	float scangamma;
	float bmask1;
	float hmask1;
} global;


#pragma parameter bogus_brightness "[ BRIGHTNESS SETTINGS ]:" 0.0 0.0 1.0 1.0

#pragma parameter glow "          (Magic) Glow Strength" 0.08 -2.0 2.0 0.01
#define glow         params.glow     // Glow Strength

#pragma parameter bloom "          Bloom Strength" 0.0 -2.0 2.0 0.05
#define bloom         params.bloom     // bloom effect

#pragma parameter mask_bloom "          Mask Bloom" 0.0 -2.0 2.0 0.05
#define mask_bloom         params.mask_bloom     // bloom effect

#pragma parameter bloom_dist "          Bloom Distribution" 0.0 -2.0 3.0 0.05
#define bloom_dist         global.bloom_dist     // bloom effect distribution

#pragma parameter halation "          Halation Strength" 0.0 -2.0 2.0 0.025
#define halation         params.halation     // halation effect

#pragma parameter bmask1 "          Bloom Mask Strength" 0.0 -1.0 1.0 0.025
#define bmask1         global.bmask1     // bloom/halation mask strength

#pragma parameter hmask1 "          Halation Mask Strength" 0.5 0.0 1.0 0.025
#define hmask1         global.hmask1     // bloom/halation mask strength

#pragma parameter gamma_c "          Gamma correct" 1.0 0.50 2.0 0.025
#define gamma_c   global.gamma_c     // adjust brightness

#pragma parameter brightboost "          Bright Boost Dark Pixels" 1.40 0.25 10.0 0.05
#define brightboost  params.brightboost     // adjust brightness

#pragma parameter brightboost1 "          Bright Boost Bright Pixels" 1.10 0.25 3.00 0.025
#define brightboost1  params.brightboost1     // adjust brightness

#pragma parameter clips "          Clip Saturated Color Beams" 0.0 -1.0 1.0 0.05
#define clips  params.clips     // kinky effect

#pragma parameter bogus_scanline "[ SCANLINE OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter scanline1b "          Scanline Shape Center - Bright" 6.0 -20.0 40.0 0.5
#define scanline1b    params.scanline1b

#pragma parameter scanline2b "          Scanline Shape Edges  - Bright" 8.0 0.0 70.0 1.0 
#define scanline2b    params.scanline2b

#pragma parameter scanline1d "          Scanline Shape Center - Dark" 6.0 -20.0 40.0 0.5
#define scanline1d    params.scanline1d

#pragma parameter scanline2d "          Scanline Shape Edges  - Dark" 8.0 0.0 70.0 1.0 
#define scanline2d    params.scanline2d

#pragma parameter beam_min "          Scanline Thickness Dark Pixels" 1.30 0.25 10.0 0.05
#define beam_min     params.beam_min     // dark area beam min - narrow

#pragma parameter beam_max "          Scanline Thickness Bright Pixels" 1.00 0.2 3.5 0.025
#define beam_max     params.beam_max     // bright area beam max - wide

#pragma parameter beam_size "          Increased Bright Scanline Beam" 0.60 0.0 1.0 0.05
#define beam_size    params.beam_size     // increased max. beam size

#pragma parameter scans "          Scanline Saturation / Mask Falloff" 0.50 0.0 6.0 0.10
#define scans        1.5*params.scans     // scanline saturation

#pragma parameter scan_falloff "          Scanline Falloff" 1.0 0.10 2.0 0.025
#define scan_falloff        global.scan_falloff     // scanline falloff

#pragma parameter no_scanlines "          No-scanline mode" 0.0 0.0 1.5 0.05
#define no_scanlines params.no_scanlines

#pragma parameter bogus_filtering "[ FILTERING OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter h_sharp "          Horizontal Sharpness" 2.5 1.0 5.0 0.05
#define h_sharp      params.h_sharp     // pixel sharpness

#pragma parameter s_sharp "          Substractive Sharpness" 0.0 0.0 0.25 0.01
#define s_sharp      params.s_sharp     // pixel sharpness

#pragma parameter bogus_screen "[ SCREEN OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter intres "          Internal Resolution Y: 0.5...y-dowsample" 0.0 0.0 6.0 0.5 // Joint parameter with linearize pass, values must match

#pragma parameter IOS "          Integer Scaling: Odd:Y, Even:'X'+Y" 0.0 0.0 4.0 1.0
#define IOS          params.IOS     // Smart Integer Scaling

#pragma parameter TATE "          TATE Mode" 0.0 0.0 1.0 1.0
#define TATE         params.TATE     // Screen orientation

#pragma parameter warpX "          CurvatureX (default 0.03)" 0.0 0.0 0.25 0.01
#define warpX        params.warpX     // Curvature X

#pragma parameter warpY "          CurvatureY (default 0.04)" 0.0 0.0 0.25 0.01
#define warpY        params.warpY     // Curvature Y

#pragma parameter c_shape "          Curvature Shape" 0.25 0.05 0.60 0.05
#define c_shape        global.c_shape     // curvature shape

#pragma parameter overscanX "          Overscan X original pixels" 0.0 -200.0 200.0 1.0
#define overscanX        global.overscanX     // OverscanX pixels

#pragma parameter overscanY "          Overscan Y original pixels" 0.0 -200.0 200.0 1.0
#define overscanY        global.overscanY     // OverscanY pixels


#define COMPAT_TEXTURE(c,d) texture(c,d)
#define TEX0 vTexCoord

#define OutputSize global.OutputSize
#define gl_FragCoord (vTexCoord * OutputSize.xy)

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
   gl_Position = global.MVP * Position;
   vTexCoord = TexCoord * 1.00001;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D LinearizePass;
layout(set = 0, binding = 3) uniform sampler2D PrePass;

#define eps 1e-10 

float st(float x)
{
	return exp2(-8.0*x*x);
} 

float st1(float x)
{
	return exp2(-8.0*x*x);
}
   
vec3 sw(float x, vec3 color, float c, float scanline)
{
	float tmp = mix(beam_min, beam_max, c);
	vec3  sat = mix(1.0.xxx + scans, 1.0.xxx, color/c);
	float  ex = x*tmp;
	return exp2(-scanline*ex*ex*sat);
} 
 
vec2 Warp(vec2 pos)
{
	pos  = pos*2.0-1.0;    
	pos  = mix(pos, vec2(pos.x*inversesqrt(1.0-c_shape*pos.y*pos.y), pos.y*inversesqrt(1.0-c_shape*pos.x*pos.x)), vec2(warpX, warpY)/c_shape);
	return pos*0.5 + 0.5;
}

vec2 Overscan(vec2 pos, float dx, float dy){
	pos=pos*2.0-1.0;    
	pos*=vec2(dx,dy);
	return pos*0.5+0.5;
} 

vec3 gc(vec3 c)
{
	float mc = max(max(c.r,c.g),c.b);
	float mg = pow(mc, 1.0/gamma_c);
	return c * mg/(mc + eps);  
}

vec3 plant (vec3 tar, float r)
{
	float t = max(max(tar.r,tar.g),tar.b) + 0.00001;
	return tar * r / t;
}


void main()
{
	vec2 prescalex = vec2(vec2(textureSize(LinearizePass, 0))/global.OriginalSize.xy);
	
	vec4 SourceSize = global.OriginalSize * mix( vec4(prescalex.x, 1.0, 1.0/prescalex.x, 1.0), vec4(1.0, prescalex.y, 1.0, 1.0/prescalex.y), TATE);
	
	float gamma_in = 1.0/COMPAT_TEXTURE(LinearizePass, vec2(0.25,0.25)).a;
	float intera = COMPAT_TEXTURE(LinearizePass, vec2(0.75,0.25)).a;
	bool interb  = ((intera < 0.5) || (no_scanlines > 0.025));
	bool notate  = (TATE < 0.5);

	float SourceY = mix(SourceSize.y, SourceSize.x, TATE);
	
	float sy = 1.0;
	if (global.intres == 1.0) sy = SourceY/224.0;
	if (global.intres > 0.25 && global.intres != 1.0) sy = global.intres;
	if (notate) SourceSize*=vec4(1.0, 1.0/sy, 1.0, sy); else SourceSize*=vec4(1.0/sy, 1.0, sy, 1.0);
	
	// Calculating texel coordinates
   
	vec2 texcoord = TEX0.xy;
	if (IOS > 0.0 && !interb){
		vec2 ofactor = OutputSize.xy/global.OriginalSize.xy;
		vec2 intfactor = (IOS < 2.5) ? floor(ofactor) : ceil(ofactor);
		vec2 diff = ofactor/intfactor;
		float scan = mix(diff.y, diff.x, TATE);
		texcoord = Overscan(texcoord, scan, scan);
		if (IOS == 1.0 || IOS == 3.0) texcoord = mix(vec2(TEX0.x, texcoord.y), vec2(texcoord.x, TEX0.y), TATE);
	}

	texcoord = Overscan(texcoord, (global.OriginalSize.x - overscanX)/global.OriginalSize.x, (global.OriginalSize.y - overscanY)/global.OriginalSize.y);

	vec2 pos  = Warp(texcoord);

	vec2 coffset = vec2(0.5, 0.5);
	
	vec2 ps = SourceSize.zw;
	vec2 OGL2Pos = pos * SourceSize.xy - coffset;
	vec2 fp = fract(OGL2Pos);
	
	vec2 dx = vec2(ps.x,0.0);
	vec2 dy = vec2(0.0, ps.y);
   
	// Reading the texels

	vec2 offx = dx;
	vec2 offy = dy;
	float fpx = fp.x;
	if(!notate)
	{
		offx = dy;
		offy = dx;
		fpx = fp.y;
	}
	
	float  f = (notate) ? fp.y : fp.x;
	
	vec2 pC4 = floor(OGL2Pos) * ps + 0.5*ps; 
	
	if (global.intres == 0.5 &&  notate && prescalex.y < 1.5) pC4.y = floor(pC4.y * global.OriginalSize.y)*global.OriginalSize.w + 0.5*global.OriginalSize.w;
	if (global.intres == 0.5 && !notate && prescalex.x < 1.5) pC4.x = floor(pC4.x * global.OriginalSize.x)*global.OriginalSize.z + 0.5*global.OriginalSize.z;
	
	if (interb && no_scanlines < 0.025)  pC4.y = pos.y; else if (interb) pC4.y = pC4.y + smoothstep(0.40-0.5*no_scanlines, 0.60 + 0.5*no_scanlines, f)*mix(SourceSize.w, SourceSize.z, TATE);
	
	vec3 tl, tr, bl, br, color, color1l, color1r, color2l, color2r, color1, color2;
	
	tl = COMPAT_TEXTURE(LinearizePass, pC4      ).rgb;
	tr = COMPAT_TEXTURE(LinearizePass, pC4 +offx).rgb;

	float fp1 = 1.0-fpx;
	float lx = fpx;        lx = pow(lx, h_sharp);
	float rx = 1.0 - fpx;  rx = pow(rx, h_sharp);
	float w = 1.0/(lx+rx);
	
	float lf = max(lx-s_sharp, mix(-0.0175, 0.0, 1.0-fp1*fp1));
	float rf = max(rx-s_sharp, mix(-0.0175, 0.0, 1.0-fpx*fpx));
	float wf = 1.0/(lf+rf);
	
	if (!interb)
{	
	pC4+=offy;

	if (global.intres == 0.5 &&  notate && prescalex.y < 1.5) pC4.y = floor((pos.y + 0.33*offy.y) * global.OriginalSize.y)*global.OriginalSize.w + 0.5*global.OriginalSize.w;
	if (global.intres == 0.5 && !notate && prescalex.x < 1.5) pC4.x = floor((pos.x + 0.33*offy.x) * global.OriginalSize.x)*global.OriginalSize.z + 0.5*global.OriginalSize.z;
	
	bl = COMPAT_TEXTURE(LinearizePass, pC4      ).rgb;
	br = COMPAT_TEXTURE(LinearizePass, pC4 +offx).rgb;
}

	vec3 one = vec3(1.0); vec3 ctmp = tl;

	#define fmax(c) max(max(c.r,c.g),c.b)

if (!interb)
{	
	// calculating scanlines

	float f1 = f; 
	float f2 = 1.0-f;

	float wt1 = st(f);
	float wt2 = st(1.0-f);	
	float wts = 1.0/(wt1+wt2);

	vec3 scolorl = (wt1*tl + wt2*bl)*wts; float fscolorl = fmax(scolorl);
	vec3 scolorr = (wt1*tr + wt2*br)*wts; float fscolorr = fmax(scolorr);	

	float c1 = fmax(tl);
	float c2 = fmax(bl);
	float c3 = fmax(tr);
	float c4 = fmax(br);	

	c1 = mix(fscolorl, c1, beam_size); c2 = mix(fscolorl, c2, beam_size);
	c3 = mix(fscolorr, c3, beam_size); c4 = mix(fscolorr, c4, beam_size);
	
	c1 = pow(c1, scan_falloff) + eps;
	c2 = pow(c2, scan_falloff) + eps;
	c3 = pow(c3, scan_falloff) + eps;
	c4 = pow(c4, scan_falloff) + eps;

	vec3 cref1 = plant(tl, c1);
	vec3 cref2 = plant(bl, c2);
	vec3 cref3 = plant(tr, c3);
	vec3 cref4 = plant(br, c4);	

	float shape1 = mix(mix(scanline1d, scanline2d, f1), mix(scanline1b, scanline2b, f1), c1);
	float shape2 = mix(mix(scanline1d, scanline2d, f2), mix(scanline1b, scanline2b, f2), c2);
	float shape3 = mix(mix(scanline1d, scanline2d, f1), mix(scanline1b, scanline2b, f1), c3);
	float shape4 = mix(mix(scanline1d, scanline2d, f2), mix(scanline1b, scanline2b, f2), c4);

	vec3 w1 = sw(f1, cref1, c1, shape1);
	vec3 w2 = sw(f2, cref2, c2, shape2);
	vec3 w3 = sw(f1, cref3, c3, shape3);
	vec3 w4 = sw(f2, cref4, c4, shape4);	

	color1 = tl * w1 + bl * w2;
	color2 = tr * w3 + br * w4;
	
	vec3 colmax = max(max(tl,tr), max(bl,br));
	
	color = clamp((rf * color1 + lf * color2) * wf, 0.0.xxx, colmax);

	if (abs(clips) > 0.005)
	{
		vec3 c1 = plant(color,1.0); vec3 sat = 1.000001 - c1; float cmax = fmax(color);
		color = mix(color, plant(pow(c1, 0.75-0.4*sat), cmax), sqrt(sat)*0.5*(w1+w2+w3+w4)*abs(clips));
	}

	color1 = (wt1*tl + wt2*bl)*wts;
	color2 = (wt1*tr + wt2*br)*wts;

	ctmp = max((rf * color1 + lf * color2) * wf, 0.0);
	
}
	
	if (interb) { color = COMPAT_TEXTURE(LinearizePass, pos).rgb; ctmp = color; }
	
	color = gc(color);

	float colmx = fmax(ctmp);
	
	FragColor = vec4(color, colmx);
}
