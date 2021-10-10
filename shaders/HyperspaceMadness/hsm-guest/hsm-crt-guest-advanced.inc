/*
   CRT - Guest - Advanced
   
   Copyright (C) 2018-2021 guest(r) - guest.r@gmail.com

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
	float brightboost;
	float brightboost1;
	float glow;
	float gsl;
	float scanline1;
	float scanline2;
	float beam_min;
	float beam_max;
	float beam_size;
    float h_sharp;
	float s_sharp;
	float bloom;
	float halation;
	float scans;
	// float scansub;
	float slotms;
	float mclip;
	float gamma_c;
	float mask_gamma;
	float smart_ei;
	float ei_limit;
	float sth;
	// float gamma_out;
	// float overscanY;
	// float intres;
	// float prescalex;
	// float c_shape;
	float shadowMask;
	float masksize;
	float mcut;
	float maskDark;
	float maskLight;
	float maskstr;
	float vertmask;
	float slotwidth;
	float double_slot;
	float spike;

	// HSM Removed because over push constant limit
	// float barspeed;
	// float barintensity;
	// float bardir;
} params;


#pragma parameter bogus_brightness "[ BRIGHTNESS SETTINGS ]:" 0.0 0.0 1.0 1.0

#pragma parameter glow "          Strength -- glow" 0.08 -2.0 2.0 0.01
#define glow         params.glow     // Glow Strength

#pragma parameter bloom "          Bloom Strength -- bloom" 0.0 0.0 2.0 0.05
#define bloom         params.bloom     // bloom effect

#pragma parameter halation "          Halation Strength -- halation" 0.0 0.0 2.0 0.025
#define halation         params.halation     // halation effect

#pragma parameter gamma_c "          Gamma Correct -- gamma_c" 1.0 0.50 2.0 0.02
#define gamma_c   params.gamma_c     // adjust brightness

#pragma parameter brightboost "          Bright Boost Dark Pixels -- brightboost" 1.40 0.25 10.0 0.05
#define brightboost  params.brightboost     // adjust brightness

#pragma parameter brightboost1 "          Bright Boost Bright Pixels -- brightboost1" 1.10 0.25 3.00 0.025
#define brightboost1  params.brightboost1     // adjust brightness

#pragma parameter bogus_scanline "[ SCANLINE OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter gsl "          Scanline Type -- gsl" 0.0 -1.0 2.0 1.0
#define gsl          params.gsl      // Alternate scanlines

#pragma parameter scanline1 "          Scanline Beam Shape Center -- scanline1" 6.0 0.0 20.0 0.5
#define scanline1    params.scanline1      // scanline param, vertical sharpness

#pragma parameter scanline2 "          Scanline Beam Shape Edges -- scanline2" 8.0 0.0 40.0 1.0 
#define scanline2    params.scanline2      // scanline param, vertical sharpness

#pragma parameter beam_min "          Scanline Shape Dark Pixels -- beam_min" 1.30 0.25 3.5 0.05
#define beam_min     params.beam_min     // dark area beam min - narrow

#pragma parameter beam_max "          Scanline Shape Bright Pixels -- beam_max" 1.00 0.4 2.5 0.05
#define beam_max     params.beam_max     // bright area beam max - wide

#pragma parameter beam_size "          Increased Bright Scanline Beam -- beam_size" 0.60 0.0 1.0 0.05
#define beam_size    params.beam_size     // increased max. beam size

#pragma parameter vertmask "          Scanline Color Deconvergence -- vertmask" 0.0 -1.0 1.0 0.1
#define vertmask     params.vertmask     // Scanline deconvergence colors

#pragma parameter scans "          Scanline Saturation -- scans" 0.60 0.0 1.0 0.05
#define scans        params.scans     // scanline saturation

// Scanline darken 'edges' effect - need to uncomment it.
	
// #pragma parameter scansub "          Scanline darken 'edges'" 0.0 0.0 0.30 0.005
// #define scansub      params.scansub   // scanline substraction

#pragma parameter spike "          Scanline Spike Removal -- spike" 1.0 0.0 2.0 0.10
#define spike params.spike

#pragma parameter bogus_filtering "[ FILTERING OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter h_sharp "          Horizontal sharpness -- h_sharp" 5.20 0.20 15.0 0.10
#define h_sharp      params.h_sharp     // pixel sharpness

#pragma parameter s_sharp "          Substractive sharpness (1.0 recommended) -- s_sharp" 0.50 0.0 1.5 0.10
#define s_sharp      params.s_sharp     // substractive sharpness

#pragma parameter smart_ei "          Smart Edges Effect Strength -- smart_ei" 0.0 0.0 0.75 0.01
#define smart_ei      params.smart_ei     // smart edge handling

#pragma parameter ei_limit "          Smart Edges Effect Strength Limit -- ei_limit" 0.25 0.0 0.75 0.01
#define ei_limit      params.ei_limit     // smart edge handling

#pragma parameter sth     "          Smart Edges Smoothing Threshold -- sth" 0.23 0.0 1.0 0.01
#define sth        params.sth     // corner size

// #pragma parameter bogus_screen "[ SCREEN OPTIONS ]: " 0.0 0.0 1.0 1.0

// #pragma parameter intres "          Internal Resolution Y: 224p/240p, 1.5...y-dowsample" 0.0 0.0 6.0 0.5 // Joint parameter with linearize pass, values must match

// #pragma parameter TATE "          TATE Mode" 0.0 0.0 1.0 1.0
// #define TATE         params.TATE     // Screen orientation

// #pragma parameter IOS "          Integer Scaling: Odd:Y, Even:'X'+Y" 0.0 0.0 4.0 1.0
// #define IOS          params.IOS     // Smart Integer Scaling

// #pragma parameter OS "          R. Bloom Overscan Mode" 1.0 0.0 2.0 1.0
// #define OS           params.OS     // Do overscan

// #pragma parameter BLOOM "          Raster bloom %" 0.0 0.0 20.0 1.0
// #define BLOOM        params.BLOOM     // Bloom overscan percentage

// #pragma parameter csize "          Corner size" 0.0 0.0 0.25 0.01
// #define csize        params.csize     // corner size

// #pragma parameter bsize "          Border smoothness" 600.0 100.0 700.0 10.0
// #define bsize        params.bsize     // border smoothness

// #pragma parameter barspeed "          Hum Bar Speed" 50.0 5.0 200.0 1.0

// #pragma parameter barintensity "          Hum Bar Intensity" 0.0 -1.0 1.0 0.01

// #pragma parameter bardir "          Hum Bar Direction" 0.0 0.0 1.0 1.0

// #pragma parameter warpX "          CurvatureX (default 0.03)" 0.0 0.0 0.25 0.01
// #define warpX        params.warpX     // Curvature X

// #pragma parameter warpY "          CurvatureY (default 0.04)" 0.0 0.0 0.25 0.01
// #define warpY        params.warpY     // Curvature Y

// #pragma parameter c_shape "          Curvature Shape" 0.25 0.05 0.60 0.05
// #define c_shape        global.c_shape     // curvature shape

// #pragma parameter overscanY "          Overscan Y original pixels" 0.0 -50.0 50.0 1.0
// #define overscanY        global.overscanY     // OverscanY pixels

// #pragma parameter prescalex "          Prescale-X Factor (for xBR...pre-shader)" 1.0 1.0 4.0 1.0
// #define prescalex        global.prescalex     // prescale-x factor

#pragma parameter bogus_masks "[ CRT MASK OPTIONS ]: " 0.0 0.0 1.0 1.0

#pragma parameter shadowMask "     CRT Mask: -1:None|0:CGWG|1-4:Lottes|5-9:Trin -- shadowMask" 0.0 -1.0 11.0 1.0
#define shadowMask   params.shadowMask     // Mask Style

#pragma parameter maskstr "          Mask Strength (0, 5-11) -- maskstr" 0.3 -0.5 1.0 0.05
#define maskstr         params.maskstr      // CGWG Mask Strength

#pragma parameter mcut "          Mask 5-9 Trinitron Low Strength -- mcut" 1.10 0.0 2.0 0.05
#define mcut         params.mcut      // Mask 5-9 cutoff

#pragma parameter masksize "          CRT Mask Size - 0:Auto | 2K | 4k -- masksize" 0 0 4 1
#define masksize     params.masksize     // Mask Size

#pragma parameter maskDark "          Lottes maskDark" 0.5 0.0 2.0 0.05
#define maskDark     params.maskDark     // Dark "Phosphor"

#pragma parameter maskLight "          Lottes maskLight" 1.5 0.0 2.0 0.05
#define maskLight    params.maskLight     // Light "Phosphor"

#pragma parameter mask_gamma "          Mask gamma -- mask_gamma" 2.40 1.0 5.0 0.05
#define mask_gamma   params.mask_gamma     // Mask application gamma

#pragma parameter slotmask "          Slot Mask Strength Bright Pixels -- slotmask" 0.0 0.0 1.0 0.05
#define slotmask     global.slotmask     // Slot Mask ON/OFF

#pragma parameter slotmask1 "          Slot Mask Strength Dark Pixels -- slotmask1" 0.0 0.0 1.0 0.05
#define slotmask1     global.slotmask1

#pragma parameter slotwidth "          Slot Mask Width -- slotwidth" 2.0 1.0 6.0 0.5
#define slotwidth    params.slotwidth     // Slot Mask Width

#pragma parameter double_slot "          Slot Mask Height: 2x1 or 4x1 -- double_slot" 1.0 1.0 2.0 1.0
#define double_slot  params.double_slot     // Slot Mask Height

#pragma parameter slotms "          Slot Mask Size - Auto | 2K | 4k -- slotms" 0 1 4 1
#define slotms  params.slotms     // Slot Mask Size

#pragma parameter mclip "          Keep Mask effect with clipping -- mclip" 0.50 0.0 1.0 0.05
#define mclip  params.mclip     // Slot Mask Size

// #pragma parameter gamma_out "Gamma out" 2.4 1.0 5.0 0.05
#define gamma_out    HSM_GAMMA_OUT_CRT     // output gamma

// #pragma parameter HSM_INTERLACE_EFFECT_SMOOTHNESS_INTERS "          Interlacing Effect Smoothness" 0.0 0.0 0.5 0.05 // Joint parameter with linearize pass, values must match
#define inters  HSM_INTERLACE_EFFECT_SMOOTHNESS_INTERS     // interlacing effect smoothing

/* HSM Removed
 #define COMPAT_TEXTURE(c,d) texture(c,d)
*/

// HSM Added
#define COMPAT_TEXTURE(c,d) HSM_GetCroppedTexSample(c,d)
// End Addition

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
   vTexCoord = TexCoord * 1.0001;
   
   // HSM Added
	if (HSM_FLIP_VIEWPORT_VERTICAL == 1)
		vTexCoord.y = 1 - vTexCoord.y;
		
	if (HSM_FLIP_VIEWPORT_HORIZONTAL > 0.5)
		vTexCoord.x =  1 - vTexCoord.x;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

// HSM Added
layout(set = 0, binding = 1) uniform sampler2D InfoCachePass;
layout(set = 0, binding = 2) uniform sampler2D LinearizePass;
layout(set = 0, binding = 3) uniform sampler2D AvgLumPass;
layout(set = 0, binding = 4) uniform sampler2D GlowPass;
layout(set = 0, binding = 5) uniform sampler2D BloomPass;

/* HSM Removed - Seemingly only used for vignette
layout(set = 0, binding = 11) uniform sampler2D PrePass;
*/

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

// Shadow mask (1-4 from PD CRT Lottes shader).

vec3 Mask(vec2 pos, float mx)
{
	/* HSM Removed
	pos = floor(pos/masksize);
	*/

	// HSM Added
	float final_mask_size = masksize;
	// If using automatic Mask Size
	if (masksize < 0.5)
	{
		final_mask_size = (global.FinalViewportSize.x + global.FinalViewportSize.y) / 3 > 1900 ? 2 : 1;
	}
	pos = floor(pos/final_mask_size);
	// End HSM Added

	vec3 mask = vec3(maskDark, maskDark, maskDark);
	vec3 one = vec3(1.0);
	float dark_compensate  = mix(max( clamp( mix (mcut, maskstr, mx),0.0, 1.0) - 0.3, 0.0) + 1.0, 1.0, mx);
	float mc = 1.0 - max(maskstr, 0.0);	
	
	// No mask
	if (shadowMask == -1.0)
	{
		mask = vec3(1.0);
	}       
	
	// Phosphor.
	else if (shadowMask == 0.0)
	{
		pos.x = fract(pos.x*0.5);
		if (pos.x < 0.49) { mask.r = 1.0; mask.g = mc; mask.b = 1.0; }
		else { mask.r = mc; mask.g = 1.0; mask.b = mc; }
	}    
   
	// Very compressed TV style shadow mask.
	else if (shadowMask == 1.0)
	{
		float line = maskLight;
		float odd  = 0.0;

		if (fract(pos.x/6.0) < 0.49)
			odd = 1.0;
		if (fract((pos.y + odd)/2.0) < 0.49)
			line = maskDark;

		pos.x = fract(pos.x/3.0);
    
		if      (pos.x < 0.3) mask.r = maskLight;
		else if (pos.x < 0.6) mask.g = maskLight;
		else                    mask.b = maskLight;
		
		mask*=line;  
	} 

	// Aperture-grille.
	else if (shadowMask == 2.0)
	{
		pos.x = fract(pos.x/3.0);

		if      (pos.x < 0.3) mask.r = maskLight;
		else if (pos.x < 0.6) mask.g = maskLight;
		else                    mask.b = maskLight;
	} 

	// Stretched VGA style shadow mask (same as prior shaders).
	else if (shadowMask == 3.0)
	{
		pos.x += pos.y*3.0;
		pos.x  = fract(pos.x/6.0);

		if      (pos.x < 0.3) mask.r = maskLight;
		else if (pos.x < 0.6) mask.g = maskLight;
		else                    mask.b = maskLight;
	}

	// VGA style shadow mask.
	else if (shadowMask == 4.0)
	{
		pos.xy = floor(pos.xy*vec2(1.0, 0.5));
		pos.x += pos.y*3.0;
		pos.x  = fract(pos.x/6.0);

		if      (pos.x < 0.3) mask.r = maskLight;
		else if (pos.x < 0.6) mask.g = maskLight;
		else                    mask.b = maskLight;
	}
	
	// Trinitron mask 5
	else if (shadowMask == 5.0)
	{
		mask = vec3(0.0);		
		pos.x = fract(pos.x/2.0);
		if  (pos.x < 0.49)
		{	mask.r  = 1.0;
			mask.b  = 1.0;
		}
		else     mask.g = 1.0;
		mask = clamp(mix( mix(one, mask, mcut), mix(one, mask, maskstr), mx), 0.0, 1.0) * dark_compensate;
	}    

	// Trinitron mask 6
	else if (shadowMask == 6.0)
	{
		mask = vec3(0.0);
		pos.x = fract(pos.x/3.0);
		if      (pos.x < 0.3) mask.r = 1.0;
		else if (pos.x < 0.6) mask.g = 1.0;
		else                    mask.b = 1.0;
		mask = clamp(mix( mix(one, mask, mcut), mix(one, mask, maskstr), mx), 0.0, 1.0) * dark_compensate;
	}
	
	// BW Trinitron mask 7 - 9
	else if (shadowMask > 6.5 && shadowMask < 9.5)
	{
		float fdivid = 0.49; if (shadowMask == 9.0) fdivid = 0.70;
		float mdivid = 2.00; if (shadowMask >  7.5) mdivid = 3.00;		
		float maskTmp = clamp(mix( mix(1.0, 0.0, mcut), mix(1.0, 0.0, maskstr), mx), 0.0, 1.0) * dark_compensate;
		mask = vec3(maskTmp);
		pos.x = fract(pos.x/mdivid);
		if  (pos.x < fdivid) mask = vec3(1.0);
	}    

	// 4k mask
	else if (shadowMask == 10.0)
	{
		mask = vec3(mc);
		pos.x = fract(pos.x * 0.25);
		if      (pos.x < 0.2)  mask.r  = 1.0;
		else if (pos.x < 0.4)  mask.rg = 1.0.xx;
		else if (pos.x < 0.7)  mask.gb = 1.0.xx;	
		else                   mask.b  = 1.0;	
	}     
	else
	{
		mask = vec3(mc);
		pos.x = fract(pos.x * 0.25);
		if      (pos.x < 0.2)  mask.r  = 1.0;
		else if (pos.x < 0.4)  mask.rb = 1.0.xx;
		else if (pos.x < 0.7)  mask.gb = 1.0.xx;	
		else                   mask.g  = 1.0;	
	}     
 
	return mask;
}  

float SlotMask(vec2 pos, float m)
{
	if ((slotmask + slotmask1) == 0.0) return 1.0;
	else
	{
	// HSM Added
	float final_mask_size = slotms;
	// If using automatic Mask Size
	if (slotms < 0.5)
	{
		final_mask_size = (global.FinalViewportSize.x + global.FinalViewportSize.y) / 3 > 1900 ? 2 : 1;
	}
	pos = floor(pos/final_mask_size);
	// End HSM Added

	/* HSM Removed
	pos = floor(pos/slotms);
	*/
	float mlen = slotwidth*2.0;
	float px = fract(pos.x/mlen);
	float py = floor(fract(pos.y/(2.0*double_slot))*2.0*double_slot);
	float slot_dark = mix(1.0-slotmask1, 1.0-slotmask, m);
	float slot = 1.0;
	if (py == 0.0 && px <  0.5) slot = slot_dark; else
	if (py == double_slot && px >= 0.5) slot = slot_dark;		
	
	return slot;
	}
}   
 
/* HSM Removed
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

float humbar(float pos)
{
	if (global.barintensity == 0.0) return 1.0; else
	{
		pos = (global.barintensity >= 0.0) ? pos : (1.0-pos);
		pos = fract(pos + mod(float(global.FrameCount),global.barspeed)/(global.barspeed-1.0));
		pos = (global.barintensity <  0.0) ? pos : (1.0-pos);	
		return (1.0-global.barintensity) + global.barintensity*pos;
	}	
}

// Borrowed from cgwg's crt-geom, under GPL

float corner(vec2 coord)
{
	coord = min(coord, vec2(1.0)-coord) * vec2(1.0, OutputSize.y/OutputSize.x);
	vec2 cdist = vec2(max(csize/3.0, max((1.0-smoothstep(100.0,600.0,bsize))*0.01,0.002)));
	coord = (cdist - min(coord,cdist));
	float dist = sqrt(dot(coord,coord));
	return clamp((cdist.x-dist)*bsize,0.0, 1.0);
}
*/

vec3 declip(vec3 c, float b)
{
	float m = max(max(c.r,c.g),c.b);
	if (m > b) c = c*b/m;
	return c;
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
	// HSM Added
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, vTexCoord);
	float TATE = USE_VERTICAL_SCANLINES;

	/* HSM Removed
	vec4 SourceSize = global.OriginalSize * vec4(prescalex, 1.0, 1.0/prescalex, 1.0);
	*/

	// HSM Added
	vec2 SourceSize = CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT;
	
	float lum = COMPAT_TEXTURE(AvgLumPass, vec2(0.5,0.5)).a;
	
	float gamma_in = 1.0/COMPAT_TEXTURE(LinearizePass, vec2(0.25,0.25)).a;
	float intera = COMPAT_TEXTURE(LinearizePass, vec2(0.75,0.25)).a;
	bool interb  = (intera < 0.5);
	bool notate  = (TATE < 0.5);

	if (USE_VERTICAL_SCANLINES > 0.5 && interb)
		notate = true;

	/* HSM Removed
	float SourceY = mix(SourceSize.y, SourceSize.x, TATE);
	float sy = 1.0;
	if (global.intres == 0.5) sy = SourceY/224.0; else
	if (global.intres == 1.0) sy = SourceY/240.0; else
	if (global.intres > 1.25) sy = global.intres;
	if (notate) SourceSize*=vec4(1.0, 1.0/sy, 1.0, sy); else SourceSize*=vec4(1.0/sy, 1.0, sy, 1.0);

	// Calculating texel coordinates
   
	vec2 texcoord = TEX0.xy;
	if (IOS > 0.0){
		vec2 ofactor = OutputSize.xy/SourceSize.xy;
		vec2 intfactor = (IOS < 2.5) ? floor(ofactor) : ceil(ofactor);
		vec2 diff = ofactor/intfactor;
		float scan = mix(diff.y, diff.x, TATE);
		texcoord = Overscan(texcoord, scan, scan);
		if (IOS == 1.0 || IOS == 3.0) texcoord = mix(vec2(TEX0.x, texcoord.y), vec2(texcoord.x, TEX0.y), TATE);
	}
   
	float factor  = 1.00 + (1.0-0.5*OS)*BLOOM/100.0 - lum*BLOOM/100.0;
	texcoord  = Overscan(texcoord, factor, factor);

	texcoord = Overscan(texcoord, 1.0, (SourceSize.y - overscanY)/SourceSize.y);

	vec2 pos  = Warp(texcoord);
	vec2 pos0 = Warp(TEX0.xy);
	*/

	// HSM Added
	vec2 screen_curved_coord = HSM_GetCurvedCoord(SCREEN_COORD, 1, SCREEN_ASPECT);
	vec2 pos = HSM_GetMirrorWrappedCoord(screen_curved_coord);

	bool smarte  = (smart_ei > 0.01); // smart edge interpolation on / off

	vec2 coffset = vec2(0.5, 0.5);
	
	/* HSM Removed
	vec2 ps = SourceSize.zw;
	*/

	// HSM Added
	vec2 ps = 1 / SourceSize.xy;

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
	
	float zero = exp2(-h_sharp);
	float sharp1 = s_sharp * zero;
	
	/* HSM Removed
	 float fdivider = min(prescalex, 2.0);
	*/
	
	// HSM Added
	float fdivider = min((1 - USE_VERTICAL_SCANLINES) * HSM_CORE_RES_SAMPLING_MULT_SCANLINE_DIR + 
			   				USE_VERTICAL_SCANLINES *  HSM_CORE_RES_SAMPLING_MULT_OPPOSITE_DIR, 
			   				2.0);

	// End Addition

	float wl3 = (2.0 + fpx)/fdivider;
	float wl2 = (1.0 + fpx)/fdivider;
	float wl1 = (      fpx)/fdivider;
	float wr1 = (1.0 - fpx)/fdivider;
	float wr2 = (2.0 - fpx)/fdivider;
	float wr3 = (3.0 - fpx)/fdivider;

	wl3*=wl3; wl3 = exp2(-h_sharp*wl3);
	wl2*=wl2; wl2 = exp2(-h_sharp*wl2);
	wl1*=wl1; wl1 = exp2(-h_sharp*wl1);
	wr1*=wr1; wr1 = exp2(-h_sharp*wr1);
	wr2*=wr2; wr2 = exp2(-h_sharp*wr2);
	wr3*=wr3; wr3 = exp2(-h_sharp*wr3);
	
	float fp1 = 1.-fpx;

	float twl3 = max(wl3 - sharp1, 0.0);
	float twl2 = max(wl2 - sharp1, mix(-0.12, 0.0, 1.0-fp1*fp1)); float scl2 = max(twl2, 0.0);
	float twl1 = max(wl1 - sharp1, 0.0);
	float twr1 = max(wr1 - sharp1, 0.0);
	float twr2 = max(wr2 - sharp1, mix(-0.12, 0.0, 1.0-fpx*fpx)); float scr2 = max(twr2, 0.0);
	float twr3 = max(wr3 - sharp1, 0.0);
	
	bool sharp = (sharp1 > 0.0);

	vec3 c1, c2;
	
	if (smarte)
	{
		twl3 = 0.0; twr3 = 0.0;
		c1 = COMPAT_TEXTURE(AvgLumPass, pC4       ).xyz;		
		c2 = COMPAT_TEXTURE(AvgLumPass, pC4 + offy).xyz;
		c1 = max(c1 - sth, 0.0);
		c2 = max(c2 - sth, 0.0);
	}

	vec3 l3, l2, l1, r1, r2, r3, sl2, sl1, sr1, sr2, color1, color2, colmin, colmax;

	l3 = COMPAT_TEXTURE(LinearizePass, pC4 -off2).rgb;
	l2 = COMPAT_TEXTURE(LinearizePass, pC4 -offx).rgb;
	l1 = COMPAT_TEXTURE(LinearizePass, pC4      ).rgb;
	r1 = COMPAT_TEXTURE(LinearizePass, pC4 +offx).rgb;
	r2 = COMPAT_TEXTURE(LinearizePass, pC4 +off2).rgb;
	r3 = COMPAT_TEXTURE(LinearizePass, pC4 +offx+off2).rgb;

	colmin = min(min(l1,r1), min(l2,r2));
	colmax = max(max(l1,r1), max(l2,r2));
	
	if (smarte)
	{	
		float pc = min(smart_ei*c1.y, ei_limit);
		float pl = min(smart_ei*max(c1.y,c1.x), ei_limit);
		float pr = min(smart_ei*max(c1.y,c1.z), ei_limit);
		twl1 = max(wl1-pc, 0.01*wl1); twr1 = max(wr1-pc, 0.01*wr1);
		twl2 = max(wl2-pl, 0.01*wl2); twr2 = max(wr2-pr, 0.01*wr2);
	}

	color1 = (l3*twl3 + l2*twl2 + l1*twl1 + r1*twr1 + r2*twr2 + r3*twr3)/(twl3+twl2+twl1+twr1+twr2+twr3);

	if (sharp) color1 = clamp(color1, colmin, colmax);
	float ts = 0.025;
	vec3 luma = vec3(0.2126, 0.7152, 0.0722);
	
	float lm2 = max(max(l2.r,l2.g),l2.b);
	float lm1 = max(max(l1.r,l1.g),l1.b);
	float rm1 = max(max(r1.r,r1.g),r1.b);
	float rm2 = max(max(r2.r,r2.g),r2.b);	
	
	float swl2 = max(twl2, 0.0) * (dot(l2,luma) + ts);
	float swl1 = twl1 * (dot(l1,luma) + ts);
	float swr1 = twr1 * (dot(r1,luma) + ts);
	float swr2 = max(twr2, 0.0) * (dot(r2,luma) + ts);
	
	float fscolor1 = (lm2*swl2 + lm1*swl1 + rm1*swr1 + rm2*swr2)/(swl2+swl1+swr1+swr2);
	vec3 mcolor1 = vec3(fscolor1);	
	vec3 scolor1 = vec3(clamp(mix(max(max(color1.r,color1.g),color1.b), fscolor1, spike), 0.0, 1.0));
	
	vec3 scolor2, mcolor2;
	
	if (!interb)
{	
	pC4+=offy;
	
	l3 = COMPAT_TEXTURE(LinearizePass, pC4 -off2).rgb;
	l2 = COMPAT_TEXTURE(LinearizePass, pC4 -offx).rgb;
	l1 = COMPAT_TEXTURE(LinearizePass, pC4      ).rgb;
	r1 = COMPAT_TEXTURE(LinearizePass, pC4 +offx).rgb;
	r2 = COMPAT_TEXTURE(LinearizePass, pC4 +off2).rgb;
	r3 = COMPAT_TEXTURE(LinearizePass, pC4 +offx+off2).rgb;

	colmin = min(min(l1,r1), min(l2,r2));
	colmax = max(max(l1,r1), max(l2,r2));
	
	if (smarte)
	{	
		float pc = min(smart_ei*c2.y, ei_limit);
		float pl = min(smart_ei*max(c2.y,c2.x), ei_limit);
		float pr = min(smart_ei*max(c2.y,c2.z), ei_limit);
		twl1 = max(wl1-pc, 0.01*wl1); twr1 = max(wr1-pc, 0.01*wr1);
		twl2 = max(wl2-pl, 0.01*wl2); twr2 = max(wr2-pr, 0.01*wr2);
	}

	color2 = (l3*twl3 + l2*twl2 + l1*twl1 + r1*twr1 + r2*twr2 + r3*twr3)/(twl3+twl2+twl1+twr1+twr2+twr3);
	
	if (sharp) color2 = clamp(color2, colmin, colmax);

	lm2 = max(max(l2.r,l2.g),l2.b);
	lm1 = max(max(l1.r,l1.g),l1.b);
	rm1 = max(max(r1.r,r1.g),r1.b);
	rm2 = max(max(r2.r,r2.g),r2.b);
	
	swl2 = max(twl2, 0.0) * (dot(l2,luma) + ts);
	swl1 = twl1 * (dot(l1,luma) + ts);
	swr1 = twr1 * (dot(r1,luma) + ts);
	swr2 = max(twr2, 0.0) * (dot(r2,luma) + ts);
	
	float fscolor2 = (lm2*swl2 + lm1*swl1 + rm1*swr1 + rm2*swr2)/(swl2+swl1+swr1+swr2);
	mcolor2 = vec3(fscolor2);
	scolor2 = vec3(clamp(mix(max(max(color2.r,color2.g),color2.b), fscolor2, spike), 0.0, 1.0));
}

	vec3 ctmp = color1; vec3 mcolor = scolor1; float w3 = 1.0; vec3 color = color1;
	vec3 one = vec3(1.0);

if (!interb)
{	
	// calculating scanlines

	float shape1 = mix(scanline1, scanline2, f);
	float shape2 = mix(scanline1, scanline2, 1.0-f);	
	
	float wt1 = st(f);
	float wt2 = st(1.0-f);

	vec3 color00 = color1*wt1 + color2*wt2;
	vec3 scolor0 = scolor1*wt1 + scolor2*wt2;
	mcolor  = (mcolor1*wt1 + mcolor2*wt2)/(wt1+wt2);
	
	ctmp = color00/(wt1+wt2);
	vec3 sctmp = scolor0/(wt1+wt2);
	mcolor = clamp(mix(ctmp, mcolor, 1.25), 0.0, 1.0);
	
	float wf1, wf2;
	
	vec3 cref1 = mix(sctmp, scolor1, beam_size); float creff1 = max(max(cref1.r,cref1.g),cref1.b);
	vec3 cref2 = mix(sctmp, scolor2, beam_size); float creff2 = max(max(cref2.r,cref2.g),cref2.b);

	float f1 = f; 
	float f2 = 1.0-f;
	
	if (gsl <  0.5) { wf1 = sw0(f1,creff1,shape1); wf2 = sw0(f2,creff2,shape2);} else
	if (gsl == 1.0) { wf1 = sw1(f1,creff1,shape1); wf2 = sw1(f2,creff2,shape2);} else
	                { wf1 = sw2(f1,creff1,shape1); wf2 = sw2(f2,creff2,shape2);}
	
	if ((wf1 + wf2) > 1.0) { float wtmp = 1.0/(wf1+wf2); wf1*=wtmp; wf2*=wtmp; }

	// Scanline darken 'edges' effect - need to uncomment it.
	
	// float ws1 = max(wf1 - scansub, 0.2*wf1*wf2); wf1 = ws1/(1.0 - wf1 + ws1);
	// float ws2 = max(wf2 - scansub, 0.2*wf2*wf1); wf2 = ws2/(1.0 - wf2 + ws2);
	
	// Scanline saturation application
	
	vec3 w1 = vec3(wf1); vec3 w2 = vec3(wf2);
	w3 = wf1+wf2;	
	
	cref1 = color1 / (max(max(color1.r,color1.g),color1.b) + 0.00001);
	cref2 = color2 / (max(max(color2.r,color2.g),color2.b) + 0.00001);	

	w1 = mix(w1*mix(one, cref1*cref1*cref1, scans), w1, wf1);
	w2 = mix(w2*mix(one, cref2*cref2*cref2, scans), w2, wf2);
	
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
		mcolor = clamp(mix(color1, mcolor1, 1.25), 0.0, 1.0);
	}

	float mx = max(max(mcolor.r,mcolor.g),mcolor.b);
	mx = pow(mx, 1.20/gamma_in);
   
	// Apply Mask
	
	vec3 orig1 = color;
	vec3 cmask = one;
	
	vec2 maskcoord = gl_FragCoord.yx * 1.000001;
	if (notate) maskcoord = maskcoord.yx;
	
	float smask = SlotMask(maskcoord, mx);	
	cmask*= Mask(maskcoord, mx);
	
	color = pow(color, vec3(mask_gamma/gamma_in));
	color = color*cmask;
	color = min(color,1.0);
	color = color*smask;
	color = pow(color, vec3(gamma_in/mask_gamma));

	cmask = min(cmask*smask, 1.0);

	if (interb) ctmp = color;
	float colmx = pow( max( max(ctmp.r, ctmp.g), ctmp.b), 1.40/gamma_out);
	float bb = mix(brightboost, brightboost1, colmx);
	if (interb) bb = (abs(intera-0.5)<0.1) ? pow(0.80*bb, 0.65) : pow(bb, 0.70);
	color*=bb;

	vec3  Glow = COMPAT_TEXTURE(GlowPass, pos).rgb;
	vec3  Bloom = COMPAT_TEXTURE(BloomPass, pos).rgb;
	float maxb = COMPAT_TEXTURE(BloomPass, pos).a;

	/* HSM Removed
	float vig  = COMPAT_TEXTURE(PrePass, clamp(pos, 0.0+0.5*global.OriginalSize.zw, 1.0-0.5*global.OriginalSize.zw)).a;
	*/

	vec3 Bloom1 = min(Bloom*(orig1+color), max(0.5*(colmx + orig1 - color),0.0));
	color = color + bloom*Bloom1;

	color = min(color, mix(one, cmask, mclip));
	if (!interb) color = declip(color, pow(w3,0.60));

	if (halation > 0.025) {
		Bloom = mix(0.5*(Bloom + Bloom*Bloom), Bloom*Bloom, colmx);	
		color = color + (1.1-0.25*colmx)*(0.75+maxb)*Bloom*(0.75 + 0.70*pow(colmx,0.33333))*mix(1.0,w3,0.5*colmx)*mix(one,cmask,0.35 + 0.4*maxb)*halation; }

	Glow = mix(Glow, 0.25*color, 0.7*colmx);
	if (glow >= 0.0) color = color + 0.5*Glow*glow; else { cmask*=cmask; cmask*=cmask; color = color + (-glow)*cmask*Glow; }

	color = min(color, 1.0);
	
	color = pow(color, vec3(1.0/gamma_out));
	
	/* HSM Removed
	FragColor = vec4(color*vig*humbar(mix(pos.y, pos.x, global.bardir)), corner(pos0));
	*/

	// HSM Added - Prep and add output gamma
	// color = pow(color * humbar(mix(pos.y, pos.x, params.bardir)), vec3(1.0/HSM_GAMMA_OUT_CRT));
	FragColor = vec4(color, 1);
	// End Addition
}
