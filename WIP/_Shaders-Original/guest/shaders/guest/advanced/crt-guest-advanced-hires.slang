#version 450

/*
   CRT - Guest - Advanced (Hi-Res version)
   
   Copyright (C) 2018-2022 guest(r) - guest.r@gmail.com

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
	float TATE, IOS, OS, BLOOM, brightboost, brightboost1, gsl, scanline1, scanline2, beam_min, beam_max, beam_size,
    h_sharp, s_sharp, warpX, warpY, glow, vertmask, spike, intres, ring;
} params;

layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
	float bloom;
	float halation;	
	float scans;
	float gamma_c;
	float overscanX;
	float overscanY;
	float c_shape;
	float scan_falloff;
	float bloom_dist;	
} global;

#pragma parameter bogus_brightness "[ BRIGHTNESS SETTINGS ]:" 0.0 0.0 1.0 1.0

#pragma parameter glow "          Glow Strength" 0.08 -2.0 2.0 0.01
#define glow         params.glow     // Glow Strength

#pragma parameter bloom "          Bloom Strength" 0.0 -2.0 2.0 0.05
#define bloom         global.bloom     // bloom effect

#pragma parameter bloom_dist "          Bloom Distribution" 0.0 0.0 2.0 0.05
#define bloom_dist         global.bloom_dist     // bloom effect distribution

#pragma parameter halation "          Halation Strength" 0.0 0.0 2.0 0.025
#define halation         global.halation     // halation effect 

#pragma parameter gamma_c "          Gamma correct" 1.0 0.50 2.0 0.02
#define gamma_c   global.gamma_c     // adjust brightness

#pragma parameter brightboost "          Bright Boost Dark Pixels" 1.40 0.25 10.0 0.05
#define brightboost  params.brightboost     // adjust brightness

#pragma parameter brightboost1 "          Bright Boost Bright Pixels" 1.10 0.25 3.00 0.025
#define brightboost1  params.brightboost1     // adjust brightness

#pragma parameter bogus_scanline "[ SCANLINE OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter gsl "          Scanline Type" 0.0 -1.0 2.0 1.0
#define gsl          params.gsl      // Alternate scanlines

#pragma parameter scanline1 "          Scanline Beam Shape Center" 6.0 -20.0 20.0 0.5
#define scanline1    params.scanline1      // scanline param, vertical sharpness

#pragma parameter scanline2 "          Scanline Beam Shape Edges" 8.0 3.0 70.0 1.0 
#define scanline2    params.scanline2      // scanline param, vertical sharpness

#pragma parameter beam_min "          Scanline Shape Dark Pixels" 1.30 0.25 5.0 0.05
#define beam_min     params.beam_min     // dark area beam min - narrow

#pragma parameter beam_max "          Scanline Shape Bright Pixels" 1.00 0.4 3.5 0.025
#define beam_max     params.beam_max     // bright area beam max - wide

#pragma parameter beam_size "          Increased Bright Scanline Beam" 0.60 0.0 1.0 0.05
#define beam_size    params.beam_size     // increased max. beam size

#pragma parameter vertmask "          Scanline Color Deconvergence" 0.0 -1.0 1.0 0.1
#define vertmask     params.vertmask     // Scanline deconvergence colors

#pragma parameter scans "          Scanline Saturation / Mask Falloff" 0.60 0.0 2.5 0.05
#define scans        global.scans     // scanline saturation

#pragma parameter scan_falloff "          Scanline Falloff" 1.0 0.25 2.0 0.05
#define scan_falloff        global.scan_falloff     // scanline falloff

#pragma parameter spike "          Scanline Spike Removal" 1.0 0.0 2.0 0.10
#define spike params.spike

#pragma parameter bogus_filtering "[ FILTERING OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter h_sharp "          Horizontal sharpness" 5.20 0.20 15.0 0.10
#define h_sharp      params.h_sharp     // pixel sharpness

#pragma parameter s_sharp "          Substractive sharpness (1.0 recommended)" 0.50 0.0 1.5 0.10
#define s_sharp      params.s_sharp     // substractive sharpness

#pragma parameter ring "          Substractive sharpness Ringing" 0.0 0.0 4.0 0.1
#define ring      params.ring     // substractive sharpness ringing 

#pragma parameter bogus_screen "[ SCREEN OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter intres "          Internal Resolution Y: 224p/240p, 1.5...y-dowsample" 0.0 0.0 6.0 0.5 // Joint parameter with linearize pass, values must match
#define intres         params.intres     // interlace resolution

#pragma parameter TATE "          TATE Mode" 0.0 0.0 1.0 1.0
#define TATE         params.TATE     // Screen orientation

#pragma parameter IOS "          Integer Scaling: Odd:Y, Even:'X'+Y" 0.0 0.0 4.0 1.0
#define IOS          params.IOS     // Smart Integer Scaling

#pragma parameter OS "          R. Bloom Overscan Mode" 1.0 0.0 2.0 1.0
#define OS           params.OS     // Do overscan

#pragma parameter BLOOM "          Raster bloom %" 0.0 0.0 20.0 1.0
#define BLOOM        params.BLOOM     // Bloom overscan percentage

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
layout(set = 0, binding = 3) uniform sampler2D AvgLumPass;
layout(set = 0, binding = 4) uniform sampler2D PrePass;

#define eps 1e-10 

float st(float x)
{
	return exp2(-10.0*x*x);
} 
   
float sw0(float x, float color, float scanline)
{
	float tmp = mix(beam_min, beam_max, color);
	float ex = x*tmp;
	ex = (gsl > -0.5) ? ex*ex : mix(ex*ex, ex*ex*ex, 0.4);
	return exp2(-scanline*ex);
} 

float sw1(float x, float color, float scanline)
{	
	x = mix (x, beam_min*x, max(x-0.4*color,0.0));
	float tmp = mix(1.2*beam_min, beam_max, color);
	float ex = x*tmp;
	return exp2(-scanline*ex*ex);
}    

float sw2(float x, float color, float scanline)
{
	float tmp = mix((2.5-0.5*color)*beam_min, beam_max, color);
	tmp = mix(beam_max, tmp, pow(x, color+0.3));
	float ex = x*tmp;
	return exp2(-scanline*ex*ex);
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


void main()
{
	vec4 SourceSize = global.OriginalSize;	
	
	float lum = COMPAT_TEXTURE(AvgLumPass, vec2(0.1,0.1)).a;
	float gamma_in = 1.0/COMPAT_TEXTURE(LinearizePass, vec2(0.25,0.25)).a;
	float intera = COMPAT_TEXTURE(LinearizePass, vec2(0.75,0.25)).a;
	bool interb  = (intera < 0.5);
	
	bool notate  = (TATE < 0.5);
	
	float SourceY = mix(SourceSize.y, SourceSize.x, TATE);
	float sy = 1.0;
	if (intres == 0.5) sy = SourceY/224.0; else
	if (intres == 1.0) sy = SourceY/240.0; else
	if (intres > 1.25) sy = intres;
	if (notate) SourceSize.yw*=vec2(1.0/sy, sy); else SourceSize.xz*=vec2(1.0/sy, sy);
	
	// Calculating texel coordinates
   
	vec2 texcoord = TEX0.xy;
	if (IOS > 0.0 && !interb){
		vec2 ofactor = OutputSize.xy/SourceSize.xy;
		vec2 intfactor = (IOS < 2.5) ? floor(ofactor) : ceil(ofactor);
		vec2 diff = ofactor/intfactor;
		float scan = mix(diff.y, diff.x, TATE);
		texcoord = Overscan(texcoord, scan, scan);
		if (IOS == 1.0 || IOS == 3.0) texcoord = mix(vec2(TEX0.x, texcoord.y), vec2(texcoord.x, TEX0.y), TATE);
	}
   
	float factor  = 1.00 + (1.0-0.5*OS)*BLOOM/100.0 - lum*BLOOM/100.0;
	texcoord  = Overscan(texcoord, factor, factor);
	texcoord = Overscan(texcoord, (global.OriginalSize.x - overscanX)/global.OriginalSize.x, (global.OriginalSize.y - overscanY)/global.OriginalSize.y);
	vec2 pos  = Warp(texcoord);
	vec2 pos0 = Warp(TEX0.xy);
   
	vec2 coffset = vec2(0.5, 0.5);
	
	vec2 ps = SourceSize.zw;
	vec2 OGL2Pos = pos * SourceSize.xy - coffset;
	vec2 fp = fract(OGL2Pos);
	
	vec2 dx = vec2(ps.x,0.0);
	vec2 dy = vec2(0.0, ps.y);
   
	// Reading the texels
	vec2 x2 = 2.0*dx;
	vec2 y2 = 2.0*dy;

	vec2 offx = dx;
	vec2 off2 = x2;
	vec2 offy = dy;
	float fpx = fp.x;
	if(!notate)
	{
		offx = dy;
		off2 = y2;
		offy = dx;
		fpx = fp.y;
	}
	float  f = (notate) ? fp.y : fp.x;
	
	vec2 pC4 = floor(OGL2Pos) * ps + 0.5*ps; 
	
	if (interb)  pC4.y = pos.y;
	
	float h_sharp1 = pow(h_sharp, 1.4);
	
	float zero = exp2(-h_sharp1);   
	
	float sharp1 = s_sharp * zero;
	
	float wl5 = 4.0 + fpx; wl5*=0.5;
	float wl4 = 3.0 + fpx; wl4*=0.5;
	float wl3 = 2.0 + fpx; wl3*=0.5;
	float wl2 = 1.0 + fpx; wl2*=0.5;
	float wl1 =       fpx; wl1*=0.5;
	float wr1 = 1.0 - fpx; wr1*=0.5;
	float wr2 = 2.0 - fpx; wr2*=0.5;
	float wr3 = 3.0 - fpx; wr3*=0.5;
	float wr4 = 4.0 - fpx; wr4*=0.5;
	float wr5 = 5.0 - fpx; wr5*=0.5;

	wl5*=wl5; wl5 = exp2(-h_sharp1*wl5);
	wl4*=wl4; wl4 = exp2(-h_sharp1*wl4);
	wl3*=wl3; wl3 = exp2(-h_sharp1*wl3);
	wl2*=wl2; wl2 = exp2(-h_sharp1*wl2);
	wl1*=wl1; wl1 = exp2(-h_sharp1*wl1);
	wr1*=wr1; wr1 = exp2(-h_sharp1*wr1);
	wr2*=wr2; wr2 = exp2(-h_sharp1*wr2);
	wr3*=wr3; wr3 = exp2(-h_sharp1*wr3);
	wr4*=wr4; wr4 = exp2(-h_sharp1*wr4);
	wr5*=wr5; wr5 = exp2(-h_sharp1*wr5);
	
	float fp1 = 1.-fpx;

	float twl5 = max(wl5 - sharp1, 0.0);
	float twl4 = max(wl4 - sharp1, mix(0.0,mix(-0.03,  0.00, fpx),float(s_sharp > 0.05))); float swl4 = max(wl4 - sharp1, 0.0);
	float twl3 = max(wl3 - sharp1, mix(0.0,mix(-0.15, -0.03, fpx),float(s_sharp > 0.05))); float swl3 = max(wl3 - sharp1, 0.0);
	float twl2 = max(wl2 - sharp1, 0.0);
	float twl1 = max(wl1 - sharp1, 0.0);
	float twr1 = max(wr1 - sharp1, 0.0);
	float twr2 = max(wr2 - sharp1, 0.0);
	float twr3 = max(wr3 - sharp1, mix(0.0,mix(-0.15, -0.03, fp1),float(s_sharp > 0.05))); float swr3 = max(wr3 - sharp1, 0.0);
	float twr4 = max(wr4 - sharp1, mix(0.0,mix(-0.03,  0.00, fp1),float(s_sharp > 0.05))); float swr4 = max(wr4 - sharp1, 0.0);
	float twr5 = max(wr5 - sharp1, 0.0);
	
	float wtt = 1.0/(twl5+twl4+twl3+twl2+twl1+twr1+twr2+twr3+twr4+twr5);
	float wt  = 1.0/(swl3+twl2+twl1+twr1+twr2+swr3);
	bool sharp = (s_sharp > 0.05);
	
	vec3 l5 = COMPAT_TEXTURE(LinearizePass, pC4 -2.0*off2).xyz;
	vec3 l4 = COMPAT_TEXTURE(LinearizePass, pC4 -3.0*offx).xyz;
	vec3 l3 = COMPAT_TEXTURE(LinearizePass, pC4 -off2).xyz;
	vec3 l2 = COMPAT_TEXTURE(LinearizePass, pC4 -offx).xyz;
	vec3 l1 = COMPAT_TEXTURE(LinearizePass, pC4      ).xyz;
	vec3 r1 = COMPAT_TEXTURE(LinearizePass, pC4 +offx).xyz;
	vec3 r2 = COMPAT_TEXTURE(LinearizePass, pC4 +off2).xyz;
	vec3 r3 = COMPAT_TEXTURE(LinearizePass, pC4 +3.0*offx).xyz;
	vec3 r4 = COMPAT_TEXTURE(LinearizePass, pC4 +4.0*offx).xyz;
	vec3 r5 = COMPAT_TEXTURE(LinearizePass, pC4 +5.0*offx).xyz;

	vec3 sl3 = l3*l3*l3; sl3*=sl3;
	vec3 sl2 = l2*l2*l2; sl2*=sl2;
	vec3 sl1 = l1*l1*l1; sl1*=sl1;
	vec3 sr1 = r1*r1*r1; sr1*=sr1;
	vec3 sr2 = r2*r2*r2; sr2*=sr2;
	vec3 sr3 = r3*r3*r3; sr3*=sr3;
	
	vec3 color1 = (l5*twl5+l4*twl4+l3*twl3+l2*twl2+l1*twl1+r1*twr1+r2*twr2+r3*twr3+r4*twr4+r5*twr5)*wtt;
	
	vec3 colmin1 = min(min(l1,r1), min(l2,r2));
	vec3 colmax1 = max(max(l1,r1), max(l2,r2));
	vec3 colmin2 = min(min(l3,r3), min(l4,r4));
	vec3 colmax2 = max(max(l3,r3), max(l4,r4));
	vec3 colmin = min(colmin1, colmin2);
	vec3 colmax = max(colmax1, colmax2);
	
	if (sharp) color1 = clamp(mix(clamp(color1, colmin, colmax), color1, ring), 0.0, 1.0);
	
	vec3 gtmp = vec3(1.0/6.0);
	
	vec3 scolor1 = (sl3*swl3 + sl2*twl2 + sl1*twl1 + sr1*twr1 + sr2*twr2 + sr3*swr3)*wt;
	scolor1 = pow(scolor1, gtmp);
	scolor1 = min(mix(color1, scolor1, spike),1.0);

	vec3 color2, scolor2;

	if (!interb)
{	
	pC4+=offy;
	
	l5 = COMPAT_TEXTURE(LinearizePass, pC4 -2.0*off2).xyz;
	l4 = COMPAT_TEXTURE(LinearizePass, pC4 -3.0*offx).xyz;
	l3 = COMPAT_TEXTURE(LinearizePass, pC4 -off2).xyz;
	l2 = COMPAT_TEXTURE(LinearizePass, pC4 -offx).xyz;
	l1 = COMPAT_TEXTURE(LinearizePass, pC4      ).xyz;
	r1 = COMPAT_TEXTURE(LinearizePass, pC4 +offx).xyz;
	r2 = COMPAT_TEXTURE(LinearizePass, pC4 +off2).xyz;
	r3 = COMPAT_TEXTURE(LinearizePass, pC4 +3.0*offx).xyz;
	r4 = COMPAT_TEXTURE(LinearizePass, pC4 +4.0*offx).xyz;
	r5 = COMPAT_TEXTURE(LinearizePass, pC4 +5.0*offx).xyz;

	sl3 = l3*l3*l3; sl3*=sl3;
	sl2 = l2*l2*l2; sl2*=sl2;
	sl1 = l1*l1*l1; sl1*=sl1;
	sr1 = r1*r1*r1; sr1*=sr1;
	sr2 = r2*r2*r2; sr2*=sr2;
	sr3 = r3*r3*r3; sr3*=sr3;
	
	color2 = (l5*twl5+l4*twl4+l3*twl3+l2*twl2+l1*twl1+r1*twr1+r2*twr2+r3*twr3+r4*twr4+r5*twr5)*wtt;
	
	colmin1 = min(min(l1,r1), min(l2,r2));
	colmax1 = max(max(l1,r1), max(l2,r2));
	colmin2 = min(min(l3,r3), min(l3,r3));
	colmax2 = max(max(l4,r4), max(l4,r4));
	colmin = min(colmin1, colmin2);
	colmax = max(colmax1, colmax2);
	
	if (sharp) color2 = clamp(mix(clamp(color2, colmin, colmax), color2, ring), 0.0, 1.0);
	
	scolor2 = (sl3*swl3 + sl2*twl2 + sl1*twl1 + sr1*twr1 + sr2*twr2 + sr3*swr3)*wt;
	scolor2 = pow(scolor2, gtmp);
	scolor2 = min(mix(color2, scolor2, spike),1.0);
}
	// calculating scanlines

	vec3 ctmp = color1; float w3 = 1.0; vec3 color = color1;
	vec3 one = vec3(1.0);

if (!interb)
{	
	float shape1 = mix(scanline1, scanline2, f);
	float shape2 = mix(scanline1, scanline2, 1.0-f);	
	
	float wt1 = st(f);
	float wt2 = st(1.0-f);

	vec3 color00 = color1*wt1 + color2*wt2;
	vec3 scolor0 = scolor1*wt1 + scolor2*wt2;
	
	ctmp = color00/(wt1+wt2);
	vec3 sctmp = scolor0/(wt1+wt2);
	
	float wf1, wf2;
	
	vec3 cref1 = mix(sctmp, scolor1, beam_size); float creff1 = pow(max(max(cref1.r,cref1.g),cref1.b), scan_falloff);
	vec3 cref2 = mix(sctmp, scolor2, beam_size); float creff2 = pow(max(max(cref2.r,cref2.g),cref2.b), scan_falloff);

	float f1 = f; 
	float f2 = 1.0-f;
	
	if (gsl <  0.5) { wf1 = sw0(f1,creff1,shape1); wf2 = sw0(f2,creff2,shape2);} else
	if (gsl == 1.0) { wf1 = sw1(f1,creff1,shape1); wf2 = sw1(f2,creff2,shape2);} else
	                { wf1 = sw2(f1,creff1,shape1); wf2 = sw2(f2,creff2,shape2);}

	if ((wf1 + wf2) > 1.0) { float wtmp = 1.0/(wf1+wf2); wf1*=wtmp; wf2*=wtmp; }
	
	// Scanline saturation application
	
	vec3 w1 = vec3(wf1); vec3 w2 = vec3(wf2);
	w3 = wf1+wf2;
	
	float mc1 = max(max(color1.r,color1.g),color1.b) + eps;
	float mc2 = max(max(color2.r,color2.g),color2.b) + eps;
	
	cref1 = color1 / mc1; cref1=cref1*cref1; cref1*=cref1;
	cref2 = color2 / mc2; cref2=cref2*cref2; cref2*=cref2;

	w1 = max( mix(w1*mix(one, cref1, scans), w1, wf1*min((1.0+0.15*scans), 1.2)), 0.0); w1 = min(w1*color1, mc1)/(color1 + eps);
	w2 = max( mix(w2*mix(one, cref2, scans), w2, wf2*min((1.0+0.15*scans), 1.2)), 0.0); w2 = min(w2*color2, mc2)/(color2 + eps);
	
	// Scanline Deconvergence
	
	vec3 cd1 = one; vec3 cd2 = one; float vm = sqrt(abs(vertmask));
	
	float v_high1 = 1.0 + 0.3*vm;
	float v_high2 = 1.0 + 0.6*vm;	
	float v_low  = 1.0 - vm;
	
	float ds1 = min(max(1.0-w3*w3, 2.5*f1), 1.0);
	float ds2 = min(max(1.0-w3*w3, 2.5*f2), 1.0);
	
	if (vertmask < 0.0) 
	{
		cd1 = mix(one, vec3(v_high2, v_low, v_low), ds1);
		cd2 = mix(one, vec3(v_low, v_high1, v_high1), ds2);
	}
	else
	{
		cd1 = mix(one, vec3(v_high1, v_low, v_high1), ds1);
		cd2 = mix(one, vec3(v_low, v_high2, v_low), ds2);
	}
	
	color = gc(color1)*w1*cd1 + gc(color2)*w2*cd2;
	color = min(color, 1.0);
}
	
	if (interb) 
	{
		color = gc(color1);
	}

	float colmx = pow(max(max(ctmp.r,ctmp.g),ctmp.b), 1.40/gamma_in);
   
	FragColor = vec4(color, colmx);
}
