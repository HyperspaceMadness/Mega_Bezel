#version 450

// HSM Added
#include "../hsm/hsm-globals-and-screen-scale-params.inc"
// End Addition

layout(push_constant) uniform Push
{
	vec4 OutputSize;
	vec4 OriginalSize;
	vec4 SourceSize;
	vec4 PassOutputSize1;
	float gameboy_color_palette;
	float contrast;
	float screen_light;
	float pixel_opacity;
	float bg_smoothing;
	float shadow_opacity;
	float shadow_offset_x;
	float shadow_offset_y;
	float screen_offset_x;
	float screen_offset_y;
} params;

////////////////////////////////////////////////////////////////////////////////
// Config                                                                     //
////////////////////////////////////////////////////////////////////////////////

#pragma parameter gameboy_color_palette    "          Palette - DMG0 | DMG1 | DMG2 | Pocket | Light | Custom" 0 0 5 1

// Useful to fine-tune the colors.
// Higher values make the "black" color closer to black - [0, 1] [DEFAULT: 0.95]
#pragma parameter contrast          "          Contrast" 0.95 0.0 1. 0 0.05

// Controls the ambient light of the screen. 
// Lower values darken the screen - [0, 2] [DEFAULT: 1.00]
#pragma parameter screen_light      "          Ambient Screen Light" 1.0 0.0 2.0 0.05

// Controls the opacity of the dot-matrix pixels. 
// Lower values make pixels more transparent - [0, 1] [DEFAULT: 1.00]
#pragma parameter pixel_opacity     "          Pixel Opacity" 1.0 0.01 1.0 0.01

// Higher values suppress changes in background color directly beneath
// the foreground to improve image clarity - [0, 1] [DEFAULT: 0.75]
#pragma parameter bg_smoothing      "          Background Smooth" 0.75 0.0 1.0 0.05


#pragma parameter GBY_SHADOW_TITLE	"[ SHADOW ]:" 0 0 0.01 0.01

// How strongly shadows affect the background
// Higher values darken the shadows - [0, 1] [DEFAULT: 0.55]
#pragma parameter shadow_opacity    "          Shadow Opacity" 0.55 0.01 1.0 0.01

// How far the shadow should be shifted to the
// right in pixels - [-infinity, infinity] [DEFAULT: 1.0]
#pragma parameter shadow_offset_x   "          Shadow Offset Horiz" 1.0 -5.0 5.0 0.5

// How far the shadow should be shifted
// down in pixels - [-infinity, infinity] [DEFAULT: 1.5]
#pragma parameter shadow_offset_y   "          Shadow Offset Vert" 1.0 -5.0 5.0 0.5


#pragma parameter GBY_SCREENOFFSET_TITLE	"[ SCREEN OFFSET ]:" 0 0 0.01 0.01

// Screen offset - [-infinity, infinity] [DEFAULT: 0]
#pragma parameter screen_offset_x   "          Screen Offset Horiz" 0.0 -5.0 5.0 0.5

// Screen offset - [-infinity, infinity] [DEFAULT: 0]
#pragma parameter screen_offset_y   "          Screen Offset Vert" 0.0 -5.0 5.0 0.5    

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Gameboy Classic Shader v0.2.2                                         //
//                                                                       //
// Copyright (C) 2013 Harlequin : unknown92835@gmail.com                 //
//                                                                       //
// This program is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// This program is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with this program.  If not, see <http://www.gnu.org/licenses/>. //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

vec2 MY_CROPPED_ORIGINAL_SIZE = HSM_GetCroppedRotatedOriginalSizeWithCoreResMult();
float MY_SCREEN_ASPECT = HSM_GetScreenAspect();
vec2 MY_SCREEN_SCALE = HSM_GetScreenScale(MY_SCREEN_ASPECT, 0.83);

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 texel;
layout(location = 2) out vec2 MY_SCREEN_COORD;

void main()
{
    gl_Position = global.MVP * Position;
    vTexCoord = TexCoord * 1.0001;
    
	MY_SCREEN_COORD = HSM_GetScreenVTexCoord(vTexCoord, MY_SCREEN_SCALE, HSM_GetPositionOffset(vec2(0.5,0.5)));

    texel = params.SourceSize.zw;
}

////////////////////////////////////////////////////////////////////////////////
// Fragment definitions                                                       //
////////////////////////////////////////////////////////////////////////////////

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 texel;
layout(location = 2) in vec2 MY_SCREEN_COORD;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 1) uniform sampler2D Source;
layout(set = 0, binding = 2) uniform sampler2D PreDeDitheringPass;
layout(set = 0, binding = 3) uniform sampler2D DMG0ColorPaletteImage;
layout(set = 0, binding = 4) uniform sampler2D DMG1ColorPaletteImage;
layout(set = 0, binding = 5) uniform sampler2D DMG2ColorPaletteImage;
layout(set = 0, binding = 6) uniform sampler2D PocketColorPaletteImage;
layout(set = 0, binding = 7) uniform sampler2D LightColorPaletteImage;
layout(set = 0, binding = 8) uniform sampler2D CustomColorPaletteImage;
layout(set = 0, binding = 9) uniform sampler2D LCDBackgroundImage;
layout(set = 0, binding = 10) uniform sampler2D BackgroundImage;
layout(set = 0, binding = 11) uniform sampler2D BackgroundVertImage;
layout(set = 0, binding = 12) uniform sampler2D ColoredGelImage;
layout(set = 0, binding = 13) uniform sampler2D TubeStaticReflectionImage;
layout(set = 0, binding = 14) uniform sampler2D InfoCachePass;

// #define bg_color texture(DMG0ColorPaletteImage, vec2(0.25, 0.5)) 

// Sample the background color from the palette
#define shadow_alpha (params.contrast * params.shadow_opacity)

// Offset for the shadow
#define shadow_offset vec2(params.shadow_offset_x * texel.x, params.shadow_offset_y * texel.y)    

// Offset for the entire screen
#define screen_offset vec2(params.screen_offset_x * texel.x, params.screen_offset_y * texel.y)   

////////////////////////////////////////////////////////////////////////////////
// Fragment shader                                                            //
////////////////////////////////////////////////////////////////////////////////

void main()
{
    // HSM Added
    HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, vTexCoord);

    vec4 bg_color = vec4(0, 0, 0, 1);
    if (params.gameboy_color_palette == 0)
        bg_color = texture(DMG0ColorPaletteImage, vec2(0.25, 0.5));
    if (params.gameboy_color_palette == 1)
        bg_color = texture(DMG1ColorPaletteImage, vec2(0.25, 0.5));
    if (params.gameboy_color_palette == 2)
        bg_color = texture(DMG2ColorPaletteImage, vec2(0.25, 0.5));
    if (params.gameboy_color_palette == 3)
        bg_color = texture(PocketColorPaletteImage, vec2(0.25, 0.5));
    if (params.gameboy_color_palette == 4)
        bg_color = texture(LightColorPaletteImage, vec2(0.25, 0.5));
    if (params.gameboy_color_palette == 5)
        bg_color = texture(CustomColorPaletteImage, vec2(0.25, 0.5));

	vec2 screen_curved_coord = HSM_GetCurvedCoord(MY_SCREEN_COORD, 1, MY_SCREEN_ASPECT);
	vec2 screen_curved_coord_mirrored = HSM_GetMirrorWrappedCoord(screen_curved_coord);
    // End Addition

    vec2 tex = floor(params.PassOutputSize1.xy * vTexCoord);
    tex = (tex + 0.5) * params.PassOutputSize1.zw;
    
    // Sample all the relevant textures
    vec4 foreground = texture(PreDeDitheringPass, tex - screen_offset);
    vec4 background = texture(LCDBackgroundImage, MY_SCREEN_COORD);
    vec4 shadows    = texture(Source, vTexCoord - (shadow_offset + screen_offset));
    vec4 background_color = bg_color;

    // Foreground and background are blended with the background color
    foreground *= bg_color;
    
    float bg_test = 0.0;
    if ( foreground.a > 0.0 )
        bg_test = 1.0; 

    background -= (background - 0.5) * params.bg_smoothing * bg_test;

    // Allows for highlights,
    // background = bg_color when the background color is 0.5 gray
    background.rgb = clamp(
        vec3( 
            bg_color.r + mix(-1.0, 1.0, background.r), 
            bg_color.g + mix(-1.0, 1.0, background.g), 
            bg_color.b + mix(-1.0, 1.0, background.b)
        ), 
        0.0, 1.0
    );

    // Shadows are alpha blended with the background
    vec4 out_color = (shadows * shadows.a * shadow_alpha) + (background * (1 - shadows.a * shadow_alpha));  

    // Foreground is alpha blended with the shadowed background
    out_color = (foreground * foreground.a * params.contrast) + (out_color * (params.screen_light - foreground.a * params.contrast * params.pixel_opacity));
    
    // Linearize so it is linear when it goes to the post prep
    FragColor = HSM_Linearize(out_color, HSM_DEFAULT_CRT_GAMMA);

    // Prep and add output gamma
	FragColor = HSM_GetPostCrtPreppedColor(FragColor, vTexCoord, screen_curved_coord, LinearizePass, true, ColoredGelImage, TubeStaticReflectionImage, BackgroundImage, BackgroundVertImage);
	// End Addition
}
