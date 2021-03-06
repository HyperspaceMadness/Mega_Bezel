/*
   CRT - Guest
   
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

// Parameter lines go here:
#pragma parameter brightboost "Bright boost" 1.0 0.5 3.0 0.05
#pragma parameter saturation "Saturation adjustment" 1.3 0.0 2.0 0.05
#pragma parameter scanline "Scanline adjust" 8.0 1.0 12.0 1.0
#pragma parameter beam_min "Scanline dark" 1.4 0.5 3.0 0.05
#pragma parameter beam_max "Scanline bright" 2.5 0.5 3.0 0.05
#pragma parameter weightr "Scanline Red Weight" 0.25 0.0 0.8 0.01
#pragma parameter weightg "Scanline Green Weight" 0.60 0.0 0.8 0.01
#pragma parameter weightb "Scanline Blue Weight" 0.10 0.0 0.8 0.01
#pragma parameter glowstr "Glow Strength" 0.0 0.0 1.0 0.02
#pragma parameter h_sharp "Horizontal sharpness" 1.7 1.0 5.0 0.1
#pragma parameter mask "Mask -1:Off,0:CGWG,1-2:Lottes,3-4 Gray,5-6:CGWG slot,7:VGA" 5.0 -1.0 8.0 1.0
#pragma parameter msk_size "CRT Mask Size (2.0 is nice in 4k)" 1.0 1.0 2.0 1.0
#pragma parameter scale "VGA mask vertical scale" 2.0 2.0 10.0 1.0
#pragma parameter MaskDark "maskDark" 0.7 0.0 2.0 0.1
#pragma parameter MaskLight "maskLight" 1.3 0.0 2.0 0.1
#pragma parameter gamma_out "Gamma out" 2.2 1.0 4.0 0.1
#pragma parameter WP "Color Temperature %, 0.0:speedup" 0.0 -100.0 100.0 5.0 


#if defined(VERTEX)

#if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying 
#define COMPAT_ATTRIBUTE attribute 
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif

COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 COLOR;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 COL0;
COMPAT_VARYING vec4 TEX0;

vec4 _oPosition1; 
uniform mat4 MVPMatrix;
uniform COMPAT_PRECISION int FrameDirection;
uniform COMPAT_PRECISION int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;

void main()
{
    gl_Position = MVPMatrix * VertexCoord;
    COL0 = COLOR;
    TEX0.xy = TexCoord.xy * 1.00001;
}

#elif defined(FRAGMENT)

#if __VERSION__ >= 130
#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture
out vec4 FragColor;
#else
#define COMPAT_VARYING varying
#define FragColor gl_FragColor
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif

uniform COMPAT_PRECISION int FrameDirection;
uniform COMPAT_PRECISION int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
uniform sampler2D Texture;
COMPAT_VARYING vec4 TEX0;

// compatibility #defines
#define Source Texture
#define vTexCoord TEX0.xy

#define SourceSize vec4(TextureSize, 1.0 / TextureSize) //either TextureSize or InputSize
#define OutputSize vec4(OutputSize, 1.0 / OutputSize)

#ifdef PARAMETER_UNIFORM
// All parameter floats need to have COMPAT_PRECISION in front of them
uniform COMPAT_PRECISION float brightboost;
uniform COMPAT_PRECISION float saturation;
uniform COMPAT_PRECISION float scanline;
uniform COMPAT_PRECISION float beam_min;
uniform COMPAT_PRECISION float beam_max;
uniform COMPAT_PRECISION float weightr;
uniform COMPAT_PRECISION float weightg;
uniform COMPAT_PRECISION float weightb;
uniform COMPAT_PRECISION float glowstr;
uniform COMPAT_PRECISION float h_sharp;
uniform COMPAT_PRECISION float mask;
uniform COMPAT_PRECISION float msk_size;
uniform COMPAT_PRECISION float scale;
uniform COMPAT_PRECISION float MaskDark;
uniform COMPAT_PRECISION float MaskLight;
uniform COMPAT_PRECISION float gamma_out;
uniform COMPAT_PRECISION float WP;

#else
#define brightboost  1.20     // adjust brightness
#define saturation   1.10     // 1.0 is normal saturation
#define scanline     8.0      // scanline param, vertical sharpness
#define beam_min     1.70	  // dark area beam min - wide
#define beam_max     2.10 	  // bright area beam max - narrow
#define weightr  0.33
#define weightg  0.33
#define weightb  0.33
#define glowstr  0.2
#define h_sharp      1.25     // pixel sharpness
#define mask   0.0      // Shadow mask type 
#define msk_size     1.0      // Shadow mask size
#define scale     2.0     
#define MaskDark         0.50     // Dark "Phosphor"
#define MaskLight    1.50     // Light "Phosphor"
#define gamma_out    0.40     // output gamma
#define WP  0.0

#endif

float sw(float x, float l) 
{
	float d = x; 							// fract(OGL2Pos)
	float bm = scanline;			
	float b = mix(beam_min,beam_max,l);		// l=luma1

	d = exp2(-bm*pow(d,b));
	return d;
}



vec3 msk (vec2 p)
{		
		p = floor(p/msk_size);
		float mf=fract(p.x*0.5);
		float m=MaskDark;
		vec3 Mask = vec3 (MaskDark);


// Phosphor.
	if (mask==0.0)
	{
		if (mf < 0.5) return vec3 (MaskLight,m,MaskLight); 
		else return vec3 (m,MaskLight,m);
	}

// Very compressed TV style shadow mask.
	else if (mask == 1.0)
	{
		float line = MaskLight;
		float odd  = 0.0;

		if (fract(p.x/6.0) < 0.5)
			odd = 1.0;
		if (fract((p.y + odd)/2.0) < 0.5)
			line = MaskDark;

		p.x = fract(p.x/3.0);
    
		if      (p.x < 0.333) Mask.r = MaskLight;
		else if (p.x < 0.666) Mask.g = MaskLight;
		else                  Mask.b = MaskLight;
		
		Mask*=line;
		return vec3 (Mask.r, Mask.g, Mask.b);  
	} 

// Aperture-grille.
	else if (mask == 2.0)
	{
		p.x = fract(p.x/3.0);

		if      (p.x < 0.333) Mask.r = MaskLight;
		else if (p.x < 0.666) Mask.g = MaskLight;
		else                  Mask.b = MaskLight;
		return vec3 (Mask.r, Mask.g, Mask.b);  

	} 
// gray
	else if (mask==3.0)
	{
		
		if (mf < 0.5) return vec3 (MaskLight); 
		else return vec3 (m);
	}
//gray 3px
	else if (mask==4.0)
	{
		float mf=fract(p.x*0.3333);
		if (mf < 0.6666) return vec3 (MaskLight); 
		else return vec3 (m);
	}
//cgwg slot
	else if (mask == 5.0)
	{
		float line = MaskLight;
		float odd  = 0.0;

		if (fract(p.x/4.0) < 0.5)
			odd = 1.0;
		if (fract((p.y + odd)/2.0) < 0.5)
			line = MaskDark;

		p.x = fract(p.x/2.0);
    
		if  (p.x < 0.5) {Mask.r = MaskLight; Mask.b = MaskLight;}
		else  Mask.g = MaskLight;	
		Mask*=line;  
		return vec3 (Mask.r, Mask.g, Mask.b);  

	} 

//cgwg slot 1440p
	else if (mask == 6.0)
	{
		float line = MaskLight;
		float odd  = 0.0;

		if (fract(p.x/6.0) < 0.5)
			odd = 1.0;
		if (fract((p.y + odd)/3.0) < 0.5)
			line = MaskDark;

		p.x = fract(p.x/2.0);
    
		if  (p.x < 0.5) {Mask.r = MaskLight; Mask.b = MaskLight;}
			else  {Mask.g = MaskLight;}	
		
		Mask*=line; 
		return vec3 (Mask.r, Mask.g, Mask.b);   
	} 

//PC CRT VGA style mask
	else if (mask == 7.0)
	{
		float line = 1.0;
		p.x = fract(p.x/2.0);
		if (fract(p.y/scale) < 0.5)
			{
				if  (p.x < 0.5) {Mask.r = MaskLight; Mask.b = MaskLight;}
				else  {Mask.g = MaskLight;}	
			}
		else
			{
				if  (p.x < 0.5) {Mask.g = MaskLight;}	
				else   {Mask.r = MaskLight; Mask.b = MaskLight;}
	}
		Mask*=line;
		return vec3 (Mask.r, Mask.g, Mask.b);   
	} 

//cgwg slot
	else if (mask == 8.0)
	{
		float line = 1.0;
		float odd  = 0.0;

		if (fract(p.x/4.0) < 0.5)
			odd = scale/2.0;

		float m = fract((p.y + odd)/scale);			
		if ( m > 0.4 && m <0.75)
			line = MaskDark;

		p.x = fract(p.x/2.0);
    
		if  (p.x < 0.5) {Mask.r = MaskLight; Mask.b = MaskLight;}
		else  Mask.g = MaskLight;	
		Mask*=line;  
		return vec3 (Mask.r, Mask.g, Mask.b);  

	} 
		else return vec3(1.0);
}
// Code from https://www.shadertoy.com/view/XdcXzn
mat4 saturationMatrix( float saturation )
{
    vec3 luminance = vec3( 0.3086, 0.6094, 0.1520 );
    
    float oneMinusSat = 1.0 - saturation;
    
    vec3 red = vec3( luminance.x * oneMinusSat ); red+= vec3( saturation, 0, 0 );
    
    vec3 green = vec3( luminance.y * oneMinusSat ); green += vec3( 0, saturation, 0 );
    
    vec3 blue = vec3( luminance.z * oneMinusSat ); blue += vec3( 0, 0, saturation );
    
    return mat4( red,     0,
                 green,   0,
                 blue,    0,
                 0, 0, 0, 1 );
}
const mat3 D65_to_XYZ = mat3 (
           0.4306190,  0.2220379,  0.0201853,
           0.3415419,  0.7066384,  0.1295504,
           0.1783091,  0.0713236,  0.9390944);

const mat3 XYZ_to_D65 = mat3 (
           3.0628971, -0.9692660,  0.0678775,
          -1.3931791,  1.8760108, -0.2288548,
          -0.4757517,  0.0415560,  1.0693490);
		   
const mat3 D50_to_XYZ = mat3 (
           0.4552773,  0.2323025,  0.0145457,
           0.3675500,  0.7077956,  0.1049154,
           0.1413926,  0.0599019,  0.7057489);
		   
const mat3 XYZ_to_D50 = mat3 (
           2.9603944, -0.9787684,  0.0844874,
          -1.4678519,  1.9161415, -0.2545973,
          -0.4685105,  0.0334540,  1.4216174);		   



vec4 glow (vec2 fragCoord, vec4 res)
	{
	vec2 uv = fragCoord.xy / TextureSize.xy;        
    vec4 color = res;
    vec2 pixelToTextureCoords = 1.0 / uv;
    
    vec4 averageColor = vec4(0.0, 0.0, 0.0, 0.0);
    
    int dx = -5; int dy = -5;
              vec2 point = fragCoord + vec2(dx,dy) * pixelToTextureCoords;
              averageColor += COMPAT_TEXTURE(Texture, point);
         
    
    averageColor /= pow(5.0, 2.0);
    
    float amount = glowstr;
     
    // extra factor of 2.0 intensifies glow effect
    vec4 glowFactor = vec4( averageColor.rgb, averageColor.a );

       // Output to screen
    vec4 fragColor = color.rgba * (color + amount * 10.0)
        ; 
        return fragColor;
}
void main()
{
	vec2 ps = SourceSize.zw;
	vec2 OGL2Pos = TEX0.xy * SourceSize.xy;
	vec2 fp = fract(OGL2Pos);
	vec2 dx = vec2(ps.x,0.0);
	vec2 dy = vec2(0.0, ps.y);

	vec2 pC4 = floor(OGL2Pos)* 1.0001 * ps + 0.5*ps;	
	
	// Reading the texels
	vec3 ul = COMPAT_TEXTURE(Texture, pC4     ).xyz; 
	vec3 ur = COMPAT_TEXTURE(Texture, pC4 + dx).xyz; 
	vec3 dl = COMPAT_TEXTURE(Texture, pC4 + dy).xyz; 
	vec3 dr = COMPAT_TEXTURE(Texture, pC4 + ps).xyz; 
	
	float lx = fp.x;        lx = pow(lx, h_sharp);
	float rx = 1.0 - fp.x;  rx = pow(rx, h_sharp);
	
	vec3 color1 = (ur*lx + ul*rx)/(lx+rx);
	vec3 color2 = (dr*lx + dl*rx)/(lx+rx);

// calculating scanlines	
	float f = fp.y;
	float lum1 = (color1.r*weightr + color1.g*weightg + color1.b*weightb);
	lum1=pow(1.5,lum1)-1.0+lum1/2.0;
	

	float lum2 = (color2.r*weightr + color2.g*weightg + color2.b*weightb);
	lum2=pow(1.5,lum2)-1.0+lum2/2.0;

	vec3 color = color1*sw(f,lum1) + color2*sw(1.0-f,lum2);
	color*= msk(gl_FragCoord.xy*1.0001);

	if (WP !=0.0){
	vec3 warmer = D50_to_XYZ*color;
	warmer = XYZ_to_D65*warmer;	
	vec3 cooler = D65_to_XYZ*color;
	cooler = XYZ_to_D50*cooler;
	float m = abs(WP)/100.0;
	vec3 comp = (WP < 0.0) ? cooler : warmer;	
	color = mix(color, comp, m);
}

	color*=brightboost; color = min(color, 1.0);

	color = color * color;

	color = pow(color, vec3(1.0/gamma_out, 1.0/gamma_out, 1.0/gamma_out));
	
	vec4 res = vec4 (color.r, color.g, color.b, 1.0);
	res+=glow(gl_FragCoord.xy,res);
	res =saturationMatrix(saturation) * res;	

    FragColor = res;
} 
#endif
