#version 450

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
// TODO Gameboy Preset Stopped working at Commit 8e4e77927cdd578923ea430d0c0480e11b42c71d on 2021/02/13
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------

// HSM Added
#include "../hsm/hsm-globals-and-screen-scale-params.inc"
// End Addition

layout(push_constant) uniform Push
{
	vec4 OutputSize;
	vec4 OriginalSize;
	vec4 SourceSize;
	float gameboy_color_palette;
	float baseline_alpha;
	float grey_balance;
	float response_time;
	float video_scale;
} params;

////////////////////////////////////////////////////////////////////////////////
// Config                                                                     //
////////////////////////////////////////////////////////////////////////////////

#pragma parameter GBY_TITLE	"[ --- GAMEBOY --- ]:" 0 0 0.01 0.01

#pragma parameter gameboy_color_palette    "          Palette - DMG0 | DMG1 | DMG2 | Pocket | Light | Custom" 0 0 5 1

// The alpha value of dots in their "off" state
// Does not affect the border region of the screen - [0, 1]
#pragma parameter baseline_alpha    "          Baseline Alpha" 0.10 0.0 1.0 0.01

// Fine-tune the balance between the different shades of grey
#pragma parameter grey_balance      "          Grey Balance" 3.0 2.0 4.0 0.1

// Simulate response time
// Higher values result in longer color transition periods - [0, 1]
#pragma parameter response_time     "          LCD Response Time" 0.333 0.0 0.777 0.111

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

////////////////////////////////////////////////////////////////////////////////
// Vertex shader                                                              //
////////////////////////////////////////////////////////////////////////////////

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 vTexCoord;
layout(location = 2) out vec2 dot_size;
layout(location = 3) out vec2 one_texel;

////////////////////////////////////////////////////////////////////////////////
// Vertex definitions                                                         //
////////////////////////////////////////////////////////////////////////////////

void main()
{
	// TODO swap out with HSM screen scale
    // HSM Removed
	// float video_scale_factor = floor(params.OutputSize.y * params.SourceSize.w);
	// vec2 scaled_video_out = (params.SourceSize.xy * vec2(video_scale_factor));
    // // Remaps position to integer scaled output
    // gl_Position = global.MVP * Position / vec4( vec2(params.OutputSize.xy / scaled_video_out), 1.0, 1.0 );   
    // vTexCoord = TexCoord * 1.0001;// + half_pixel;
    // dot_size = params.SourceSize.zw;
    // one_texel = 1.0 / (params.SourceSize.xy * video_scale_factor);

    // // HSM Added
    // pixels per source texel
    float video_scale_factor = floor(MY_SCREEN_SCALE.y * params.OutputSize.y * params.SourceSize.w);

    // Size of screen pixels in output pixels
	vec2 scaled_video_out = params.OutputSize.xy * MY_SCREEN_SCALE;

    // Remaps position to integer scaled output
    gl_Position = global.MVP * Position / vec4( vec2(MY_SCREEN_SCALE * params.OutputSize.xy / scaled_video_out), 1.0, 1.0 );   
    vTexCoord = TexCoord * 1.0001;
    MY_SCREEN_SCALE = HSM_GetVTexCoordWithArgs(vTexCoord, MY_SCREEN_SCALE, HSM_GetPositionOffset(vec2(0.5, 0.5)));

    // Pixel size inside source
    dot_size = 1 / MY_CROPPED_ORIGINAL_SIZE;
    
    // Pixel size relative to final
    one_texel = 1.0 / scaled_video_out;
}

////////////////////////////////////////////////////////////////////////////////
// Fragment shader                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 2) in vec2 dot_size;
layout(location = 3) in vec2 one_texel;
layout(location = 4) in vec2 MY_SCREEN_COORD;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D OriginalHistory1;
layout(set = 0, binding = 4) uniform sampler2D OriginalHistory2;
layout(set = 0, binding = 5) uniform sampler2D OriginalHistory3;
layout(set = 0, binding = 6) uniform sampler2D OriginalHistory4;
layout(set = 0, binding = 7) uniform sampler2D OriginalHistory5;
layout(set = 0, binding = 8) uniform sampler2D OriginalHistory6;
layout(set = 0, binding = 9) uniform sampler2D OriginalHistory7;
layout(set = 0, binding = 10) uniform sampler2D DMG0ColorPaletteImage;
layout(set = 0, binding = 11) uniform sampler2D DMG1ColorPaletteImage;
layout(set = 0, binding = 12) uniform sampler2D DMG2ColorPaletteImage;
layout(set = 0, binding = 13) uniform sampler2D PocketColorPaletteImage;
layout(set = 0, binding = 14) uniform sampler2D LightColorPaletteImage;
layout(set = 0, binding = 15) uniform sampler2D CustomColorPaletteImage;

////////////////////////////////////////////////////////////////////////////////
//fragment definitions                                                        //
////////////////////////////////////////////////////////////////////////////////

// HSM Removed
// #define foreground_color texture(DMG0ColorPaletteImage, vec2(0.75, 0.5)).rgb                 //hardcoded to look up the foreground color from the right half of the palette image
//#define rgb_to_alpha(rgb) ( ((rgb.r + rgb.g + rgb.b) / 3.0) + (is_on_dot * vec2(params.baseline_alpha), 1.0) )       //averages rgb values (allows it to work with color games), modified for contrast and base alpha


// Frame sampling definitions
#define curr_rgb  abs(1.0 - texture(Source,           screen_curved_coord_mirrored).rgb)
#define prev0_rgb abs(1.0 - texture(OriginalHistory1, screen_curved_coord_mirrored).rgb)
#define prev1_rgb abs(1.0 - texture(OriginalHistory2, screen_curved_coord_mirrored).rgb)
#define prev2_rgb abs(1.0 - texture(OriginalHistory3, screen_curved_coord_mirrored).rgb)
#define prev3_rgb abs(1.0 - texture(OriginalHistory4, screen_curved_coord_mirrored).rgb)
#define prev4_rgb abs(1.0 - texture(OriginalHistory5, screen_curved_coord_mirrored).rgb)
#define prev5_rgb abs(1.0 - texture(OriginalHistory6, screen_curved_coord_mirrored).rgb)
#define prev6_rgb abs(1.0 - texture(OriginalHistory7, screen_curved_coord_mirrored).rgb)

void main()
{
    vec3 foreground_color = vec3(0, 0, 0);
    if (params.gameboy_color_palette == 0)
        foreground_color = texture(DMG0ColorPaletteImage, vec2(0.75, 0.5)).rgb;
    if (params.gameboy_color_palette == 1)
        foreground_color = texture(DMG1ColorPaletteImage, vec2(0.75, 0.5)).rgb;
    if (params.gameboy_color_palette == 2)
        foreground_color = texture(DMG2ColorPaletteImage, vec2(0.75, 0.5)).rgb;
    if (params.gameboy_color_palette == 3)
        foreground_color = texture(PocketColorPaletteImage, vec2(0.75, 0.5)).rgb;
    if (params.gameboy_color_palette == 4)
        foreground_color = texture(LightColorPaletteImage, vec2(0.75, 0.5)).rgb;
    if (params.gameboy_color_palette == 5)
        foreground_color = texture(CustomColorPaletteImage, vec2(0.75, 0.5)).rgb;

    vec2 screen_curved_coord = HSM_GetCurvedCoord(MY_SCREEN_SCALE, 1, MY_SCREEN_ASPECT);
	vec2 screen_curved_coord_mirrored = HSM_GetMirrorWrappedCoord(screen_curved_coord);

    // Determine if the corrent texel lies on a dot or in the space between dots
    float is_on_dot = 0.0;                   
    if ( mod(screen_curved_coord_mirrored.x, dot_size.x) > one_texel.x && mod(screen_curved_coord_mirrored.y, dot_size.y * 1.0001) > one_texel.y )
        is_on_dot = 1.0;

    // Sample color from the current and previous frames, apply response time modifier
    // Response time effect implmented through an exponential dropoff algorithm
    vec3 input_rgb = curr_rgb;
    input_rgb += (prev0_rgb - input_rgb) * params.response_time;
    input_rgb += (prev1_rgb - input_rgb) * pow(params.response_time, 2.0);
    input_rgb += (prev2_rgb - input_rgb) * pow(params.response_time, 3.0);
    input_rgb += (prev3_rgb - input_rgb) * pow(params.response_time, 4.0);
    input_rgb += (prev4_rgb - input_rgb) * pow(params.response_time, 5.0);
    input_rgb += (prev5_rgb - input_rgb) * pow(params.response_time, 6.0);
    input_rgb += (prev6_rgb - input_rgb) * pow(params.response_time, 7.0);

    float rgb_to_alpha = (input_rgb.r + input_rgb.g + input_rgb.b) / params.grey_balance
                        + (is_on_dot * params.baseline_alpha);

    // Apply foreground color and assign alpha value
    // Apply the foreground color to all texels -
    // the color will be modified by alpha later - and assign alpha based on rgb input
    vec4 out_color = vec4(foreground_color, rgb_to_alpha);  

    // Overlay the matrix
    // If the fragment is not on a dot, set its alpha value to 0
    out_color.a *= is_on_dot;

    FragColor = out_color;
}
