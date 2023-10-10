/*
    Mega Bezel - Creates a graphic treatment for the game play area to give a retro feel
    Copyright (C) 2019-2023 HyperspaceMadness - HyperspaceMadness@outlook.com

    Incorporates much great feedback from the libretro forum, and thanks 
    to Hunterk who helped me get started

    See more at the libretro forum
    https://forums.libretro.com/t/hsm-mega-bezel-reflection-shader-feedback-and-updates

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

#define HAS_IMAGE_LAYER_PARAMS

//---------------------------------------------------------------------------------------------------
//    IMAGE LAYERING START
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_LAYER_ORDER_EMPTY_LINE	  " " 0 0 0.001 0.001
#pragma parameter HSM_LAYER_ORDER_TITLE			  "[ LAYER ORDER ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_LAYER_ORDER              "          Background Layer"  0 0 12 1
float HSM_BG_LAYER_ORDER = global.HSM_BG_LAYER_ORDER;

#pragma parameter HSM_VIEWPORT_VIGNETTE_LAYER_ORDER "          Viewport Vignette"  1 0 12 1
float HSM_VIEWPORT_VIGNETTE_LAYER_ORDER = global.HSM_VIEWPORT_VIGNETTE_LAYER_ORDER;

#pragma parameter HSM_LED_LAYER_ORDER             "          LED Layer"  2 0 12 1
float HSM_LED_LAYER_ORDER = global.HSM_LED_LAYER_ORDER;

#pragma parameter HSM_MIDDLE_LAYER_GROUP_FIRST_LAYER "          ---Middle Layer Group FIRST Layer"  3 0 12 1
float HSM_MIDDLE_LAYER_GROUP_FIRST_LAYER = global.HSM_MIDDLE_LAYER_GROUP_FIRST_LAYER;

#pragma parameter HSM_DEVICE_LAYER_ORDER         "          Device Layer"  3 0 12 1
float HSM_DEVICE_LAYER_ORDER = global.HSM_DEVICE_LAYER_ORDER;

#pragma parameter HSM_DEVICELED_LAYER_ORDER      "          Device LED Layer"  4 0 12 1
float HSM_DEVICELED_LAYER_ORDER = global.HSM_DEVICELED_LAYER_ORDER;

#pragma parameter HSM_BEZEL_LAYER_ORDER_OFFSET_FROM_TUBE "          Bezel Layer Offset from CRT Tube"  0 -5 0 1
float HSM_BEZEL_LAYER_ORDER_OFFSET_FROM_TUBE = global.HSM_BEZEL_LAYER_ORDER_OFFSET_FROM_TUBE;

// TODO need to resolve when CRT Tube is set at 0. 
// Everything under the CRT must be in the middle or bottom layer
// How to put reflection on top of layers above the tube?
#pragma parameter HSM_CRT_LAYER_ORDER             "          ---CRT Tube (Middle Layer Group LAST Layer)"  5 0 12 1
float HSM_CRT_LAYER_ORDER = global.HSM_CRT_LAYER_ORDER;

#pragma parameter HSM_DECAL_LAYER_ORDER           "          Decal Layer"  7 0 12 1
float HSM_DECAL_LAYER_ORDER = global.HSM_DECAL_LAYER_ORDER;

#pragma parameter HSM_CAB_GLASS_LAYER_ORDER       "          Cabinet Glass Layer"  9 0 12 1
float HSM_CAB_GLASS_LAYER_ORDER = global.HSM_CAB_GLASS_LAYER_ORDER;

#pragma parameter HSM_TOP_LAYER_ORDER             "          Top Layer"  12 0 12 1
float HSM_TOP_LAYER_ORDER = global.HSM_TOP_LAYER_ORDER;


//---------------------------------------------------------------------------------------------------
//    COMPOSITING LAYER GROUPS
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_LAYER_GROUP_EMPTY_LINE			                                " " 0 0 0.001 0.001
#pragma parameter HSM_LAYER_GROUP_TITLE			                                        "[ COMPOSITE BASE COLOR ]:" 0 0 0.01 0.01

#pragma parameter HSM_COMPOSITE_SHOW_ALPHA                                              "          Output Color - RGB | ALPHA"  0 0 1 1
float HSM_COMPOSITE_SHOW_ALPHA = global.HSM_COMPOSITE_SHOW_ALPHA;

#pragma parameter HSM_COMPOSITE_BASE_COLOR                                              "          Base Color - BLACK | RED | PINK | YELLOW | AQUA"  0 0 4 1
float HSM_COMPOSITE_BASE_COLOR = global.HSM_COMPOSITE_BASE_COLOR;

#pragma parameter HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS                                   "          Base Color Brightness"  10 0 100 1
float HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS = global.HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS / 100.0;


#pragma parameter HSM_LGHT_DEBUG_EMPTY_LINE			                                    " " 0 0 0.001 0.001
#pragma parameter HSM_LGHT_DEBUG_TITLE			                                     "[ STATIC LIGHTIING E.G. INCANDESCENT]:" 0 0 0.01 0.01

#pragma parameter HSM_LGHT_DEBUG_MAPS                                                "          Debug - SHADED | SHADOW | HEIGHT | NORMALS"  0 0 3 1
float HSM_LGHT_DEBUG_MAPS = global.HSM_LGHT_DEBUG_MAPS;

#pragma parameter HSM_LGHT_DEBUG_MATERIAL                                            "     Material Color - TEXTURE | GRAY | WHITE"  0 0 2 1
float HSM_LGHT_DEBUG_MATERIAL = global.HSM_LGHT_DEBUG_MATERIAL;

#pragma parameter HSM_LGHT_DEBUG_SHADING_MODE                                        "   Shading - ALL | AMB | DIFF | DIFF AMB | DIFF AMB SPEC | SPEC"  0 0 5 1
float HSM_LGHT_DEBUG_SHADING_MODE = global.HSM_LGHT_DEBUG_SHADING_MODE;


#pragma parameter HSM_LGHT_EMPTY_LINE			                                    " " 0 0 0.001 0.001
#pragma parameter HSM_LGHT_PARAMETRIC_TITLE			                                     "[ STATIC LIGHTIING E.G. INCANDESCENT]:" 0 0 0.01 0.01

#pragma parameter HSM_LGHT_NATURAL_AMBIENT_HUE                                              "          Natural Ambient Hue"  230 0 360 1
float HSM_LGHT_NATURAL_AMBIENT_HUE = global.HSM_LGHT_NATURAL_AMBIENT_HUE / 360.0;

#pragma parameter HSM_LGHT_NATURAL_AMBIENT_SATURATION                                       "          Natural Ambient Saturation"  60 0 100 1
float HSM_LGHT_NATURAL_AMBIENT_SATURATION = global.HSM_LGHT_NATURAL_AMBIENT_SATURATION / 100.0;

#pragma parameter HSM_LGHT_NATURAL_AMBIENT_VALUE                                            "          Natural Ambient Value"  10 0 100 1
float HSM_LGHT_NATURAL_AMBIENT_VALUE = global.HSM_LGHT_NATURAL_AMBIENT_VALUE / 100.0;

#pragma parameter HSM_LGHT_SPOT_BRIGHTNESS                                               "          Spot Brightness"  100 0 500 5
float HSM_LGHT_SPOT_BRIGHTNESS = global.HSM_LGHT_SPOT_BRIGHTNESS / 100;

#pragma parameter HSM_LGHT_SPOT_GLOBAL_SCALE                                             "          Spot Global Scale"  100 0 500 5
float HSM_LGHT_SPOT_GLOBAL_SCALE = global.HSM_LGHT_SPOT_GLOBAL_SCALE / 100;

#pragma parameter HSM_LGHT_SPOT_WIDTH                                                    "          Spot Width"  80 0 500 5
float HSM_LGHT_SPOT_WIDTH = global.HSM_LGHT_SPOT_WIDTH / 100;

#pragma parameter HSM_LGHT_SPOT_FALLOFF                                                 "          Spot Falloff"  4 0.1 20 0.1
float HSM_LGHT_SPOT_FALLOFF = global.HSM_LGHT_SPOT_FALLOFF;

// #pragma parameter HSM_LGHT_SPOT_HUE                                                  "          Spot 1 Hue"  43 0 360 1
// float HSM_LGHT_SPOT_HUE = global.HSM_LGHT_SPOT_HUE / 360.0;
float HSM_LGHT_SPOT_HUE = 43;

// #pragma parameter HSM_LGHT_SPOT_SATURATION                                           "          Spot 1 Saturation"  62 0 100 1
// float HSM_LGHT_SPOT_SATURATION = global.HSM_LGHT_SPOT_SATURATION / 100.0;
float HSM_LGHT_SPOT_SATURATION = 62;

// #pragma parameter HSM_LGHT_SPOT_VALUE                                                "          Spot 1 Value"  77 0 100 1
// float HSM_LGHT_SPOT_VALUE = global.HSM_LGHT_SPOT_VALUE / 100.0;
float HSM_LGHT_SPOT_VALUE = 77;

#pragma parameter HSM_LGHT_SPOT_AMBIENT                                                 "          Spot Ambient Amount"  5 0 100 0.1
float HSM_LGHT_SPOT_AMBIENT = global.HSM_LGHT_SPOT_AMBIENT / 100.0;

#pragma parameter HSM_LGHT_SPOT_SHADOW_STRENGTH                                         "          Spot Shadow Strength"  100 0 100 1
float HSM_LGHT_SPOT_SHADOW_STRENGTH = global.HSM_LGHT_SPOT_SHADOW_STRENGTH / 100;

#pragma parameter HSM_LGHT_SPOT1_POINTX                                                   "          Spot 1 Pos X"  -70 -200 200 1
float HSM_LGHT_SPOT1_POINTX = global.HSM_LGHT_SPOT1_POINTX / 100;

#pragma parameter HSM_LGHT_SPOT1_POINTY                                                   "          Spot 1 Pos Y"  70 -200 200 1
float HSM_LGHT_SPOT1_POINTY = global.HSM_LGHT_SPOT1_POINTY / 100;

#pragma parameter HSM_LGHT_SPOT2_POINTX                                                   "          Spot 2 Pos X"  70 -200 200 1
float HSM_LGHT_SPOT2_POINTX = global.HSM_LGHT_SPOT2_POINTX / 100;

#pragma parameter HSM_LGHT_SPOT2_POINTY                                                   "          Spot 2 Pos Y"  -70 -200 200 1
float HSM_LGHT_SPOT2_POINTY = global.HSM_LGHT_SPOT2_POINTY / 100;



#pragma parameter HSM_BOUNCELIGHT_LAYER_GROUP_EMPTY_LINE			                    " " 0 0 0.001 0.001
#pragma parameter HSM_BOUNCELIGHT_LAYER_GROUP_TITLE			                            "[ GLOBAL ROOM BOUNCE LIGHT ]:" 0 0 0.01 0.01

// #pragma parameter HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS                                 "         Texture Brightness (When Fully Lit by Bounce 'Albedo')"  100 0 1000 0.5
// float HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS = global.HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS / 100;
float HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS = 1;

#pragma parameter HSM_LGHT_BOUNCE_BRIGHTNESS                                           "          Bouncelight Brightness"  15 0 500 1
float HSM_LGHT_BOUNCE_BRIGHTNESS = 10 * global.HSM_LGHT_BOUNCE_BRIGHTNESS / 100;

// #pragma parameter HSM_LGHT_BOUNCE_SCALE                                                "          Bouncelight Scale"  100 0 500 5
// float HSM_LGHT_BOUNCE_SCALE = global.HSM_LGHT_BOUNCE_SCALE / 100;
float HSM_LGHT_BOUNCE_SCALE = 1.0;

// #pragma parameter HSM_LGHT_BOUNCE_ANAMORPH                                          "        Anamorph - NEGATIVE: KEEP SIDES | POSITIVE: KEEP CENTER" 0.0 -500.0 500.0 20
// float HSM_LGHT_BOUNCE_ANAMORPH = global.HSM_LGHT_BOUNCE_ANAMORPH / 10.0;
float HSM_LGHT_BOUNCE_ANAMORPH = 0;

// #pragma parameter HSM_LGHT_BOUNCE_MIX_HORZ                                             "          Bouncelight Mix Horizontal"  20 0 100 1
// float HSM_LGHT_BOUNCE_MIX_HORZ = global.HSM_LGHT_BOUNCE_MIX_HORZ / 100;
float HSM_LGHT_BOUNCE_MIX_HORZ = 0.2;

// #pragma parameter HSM_LGHT_BOUNCE_MIX_VERT                                             "          Bouncelight Mix Vertical"  0 0 100 1
// float HSM_LGHT_BOUNCE_MIX_VERT = global.HSM_LGHT_BOUNCE_MIX_VERT / 100;
float HSM_LGHT_BOUNCE_MIX_VERT = 0;
// TODO since we aren't using this perhaps we should simplify in the final pass

#pragma parameter HSM_LGHT_BOUNCE_MIX_FULLSCREEN_GLOW                                  "          Bouncelight Mix Fullscreen Glow"  50 0 100 1
float HSM_LGHT_BOUNCE_MIX_FULLSCREEN_GLOW = global.HSM_LGHT_BOUNCE_MIX_FULLSCREEN_GLOW / 100;

// #pragma parameter HSM_LGHT_BOUNCE_SATURATION                                           "          Bouncelight Saturation"  100 0 1000 1
// float HSM_LGHT_BOUNCE_SATURATION = global.HSM_LGHT_BOUNCE_SATURATION / 100;
float HSM_LGHT_BOUNCE_SATURATION = 1.0;
// TODO since we aren't using this perhaps we should simplify in the final pass


// #pragma parameter HSM_LGHT_BOUNCE_SHADOW_STRENGTH                                      "          Bouncelight Shadow Strength"  70 0 100 1
// float HSM_LGHT_BOUNCE_SHADOW_STRENGTH = global.HSM_LGHT_BOUNCE_SHADOW_STRENGTH / 100;


#pragma parameter HSM_LGHT_EMPTY_LINE			                                    " " 0 0 0.001 0.001
#pragma parameter HSM_LGHT_TITLE			                                        "[ GENERATED LIGHTING & SHADING]:" 0 0 0.01 0.01

#pragma parameter HSM_LGHT_LIGHT_POS_X                                               "          Light Pos X"  100 -3000 3000 5
// float HSM_LGHT_LIGHT_POS_X = global.HSM_LGHT_LIGHT_POS_X / 100.0;

#pragma parameter HSM_LGHT_LIGHT_POS_Y                                               "          Light Pos Y"  100 -3000 3000 5
// float HSM_LGHT_LIGHT_POS_Y = global.HSM_LGHT_LIGHT_POS_Y / 100.0 * -1.0;

#pragma parameter HSM_LGHT_LIGHT_POS_Z                                               "          Light Pos Z (Depth)"  300 -3000 3000 5
// float HSM_LGHT_LIGHT_POS_Z = global.HSM_LGHT_LIGHT_POS_Z / 100.0;

vec3 HSM_LGHT_LIGHT_POINT = vec3(global.HSM_LGHT_LIGHT_POS_X / 100.0, global.HSM_LGHT_LIGHT_POS_Y / 100.0, global.HSM_LGHT_LIGHT_POS_Z / 100.0);

#pragma parameter HSM_LGHT_OMNI                                           "          Light Omnidirectional Amount"  0 0 100 0.5
float HSM_LGHT_OMNI = global.HSM_LGHT_OMNI / 100.0;

#pragma parameter HSM_LGHT_NORMAL_SOURCE                                             "          Normal Source - NONE | NORMAL MAP | HEIGHT MAP"  2 0 2 1
float HSM_LGHT_NORMAL_SOURCE = global.HSM_LGHT_NORMAL_SOURCE;

#pragma parameter HSM_LGHT_NORMAL_STRENGTH                                               "          Normal Strength"  100 0 1000 5
float HSM_LGHT_NORMAL_STRENGTH = global.HSM_LGHT_NORMAL_STRENGTH / 100.0;

// #pragma parameter HSM_LGHT_DIFFUSE_AO                                               "          Diffuse Ambient Occlusion"  50 0 100 1
// float HSM_LGHT_DIFFUSE_AO = global.HSM_LGHT_DIFFUSE_AO / 100.0;
float HSM_LGHT_DIFFUSE_AO = 0.0;

#pragma parameter HSM_LGHT_ROUGHNESS                                                "          Roughness"  25 0 100 1
float HSM_LGHT_ROUGHNESS = global.HSM_LGHT_ROUGHNESS / 100.0;

#pragma parameter HSM_LGHT_AO_HEIGHT_CUTOFF                                          "          Specular and Diffuse AO Height Cutoff"  100 0 1000 1
float HSM_LGHT_AO_HEIGHT_CUTOFF = global.HSM_LGHT_AO_HEIGHT_CUTOFF / 10000.0;

#pragma parameter HSM_LGHT_SPECULAR_AMOUNT                                          "          Specular Amount"  20 0 400 1
float HSM_LGHT_SPECULAR_AMOUNT = global.HSM_LGHT_SPECULAR_AMOUNT / 100.0;

// #pragma parameter HSM_LGHT_SPECULAR_AO                                              "          Specular Ambient Occlusion"  100 0 100 1
// float HSM_LGHT_SPECULAR_AO = global.HSM_LGHT_SPECULAR_AO / 100.0;
float HSM_LGHT_SPECULAR_AO = 1.0;

// TODO need to add Shadow Strength?

#pragma parameter HSM_HEIGHTMAP_SHADOWS_EMPTY_LINE			                            " " 0 0 0.001 0.001
#pragma parameter HSM_HEIGHTMAP_SHADOWS_TITLE			                                "[ HEIGHT MAP SHADOWS ]:" 0 0 0.01 0.01

#pragma parameter HSM_LGHT_SHADOW_PROJECTED_AMOUNT                                          "          Shadow Projected from Middleground onto BG Amount"  100 0 100 1
float HSM_LGHT_SHADOW_PROJECTED_AMOUNT = global.HSM_LGHT_SHADOW_PROJECTED_AMOUNT / 100.0;

#pragma parameter HSM_LGHT_SHADOW_FROM_MIDDLE_AMOUNT                                          "          Shadow From Middle Amount"  80 0 100 1
float HSM_LGHT_SHADOW_FROM_MIDDLE_AMOUNT = global.HSM_LGHT_SHADOW_FROM_MIDDLE_AMOUNT / 100.0;

#pragma parameter HSM_LGHT_SHADOW_FROM_RIGHT_AMOUNT                                          "          Shadow From Right Amount"  80 0 100 1
float HSM_LGHT_SHADOW_FROM_RIGHT_AMOUNT = global.HSM_LGHT_SHADOW_FROM_RIGHT_AMOUNT / 100.0;

#pragma parameter HSM_LGHT_SHADOW_FROM_LEFT_AMOUNT                                          "          Shadow From Left Amount"  80 0 100 1
float HSM_LGHT_SHADOW_FROM_LEFT_AMOUNT = global.HSM_LGHT_SHADOW_FROM_LEFT_AMOUNT / 100.0;

#pragma parameter HSM_HSHADOW_SAMPLES                                                   "          Samples"  50 5 600 1 // at 140 artifacts seem to disappear 
float HSM_HSHADOW_SAMPLES = global.HSM_HSHADOW_SAMPLES;

// #pragma parameter HSM_HSHADOW_NOISE                                                     "          Noise Amount"  100 0 600 5
// float HSM_HSHADOW_NOISE = global.HSM_HSHADOW_NOISE / 100;
float HSM_HSHADOW_NOISE = 100 / 100;

#pragma parameter HSM_HSHADOW_DEPTH_MULT                                                "          Shadow Light Distance Multiplier"  1000 0 5000 20
float HSM_HSHADOW_DEPTH_MULT = global.HSM_HSHADOW_DEPTH_MULT / 100.0;

#pragma parameter HSM_HSHADOW_DIST_UMBRA                                                "          Umbra Distance (Solid Shadow)"  45 0 3000 5
float HSM_HSHADOW_DIST_UMBRA = global.HSM_HSHADOW_DIST_UMBRA / 1000.0;

#pragma parameter HSM_HSHADOW_DIST_PENUMBRA                                             "          Penumbra Distance (Where Faded Shadow is Gone)"  45 0 1000 1
float HSM_HSHADOW_DIST_PENUMBRA = max(global.HSM_HSHADOW_DIST_PENUMBRA / 100.0, HSM_HSHADOW_DIST_UMBRA);

// #pragma parameter HSM_HSHADOW_SPREAD_NOISE                                            "          Shadow Spread Noise"  50 0 500 5
// float HSM_HSHADOW_SPREAD_NOISE = global.HSM_HSHADOW_SPREAD_NOISE / 100.0;
float HSM_HSHADOW_SPREAD_NOISE = 0;

#pragma parameter HSM_HSHADOW_GAMMA                                                     "          Shadow Gamma"  100 1 1000 1
float HSM_HSHADOW_GAMMA = global.HSM_HSHADOW_GAMMA / 100.0;

#pragma parameter HSM_HSHADOW_BG_CONTRAST_CENTER                                        "          BG Height Contrast Center"  0 0 100 1
float HSM_HSHADOW_BG_CONTRAST_CENTER = global.HSM_HSHADOW_BG_CONTRAST_CENTER / 100.0;

#pragma parameter HSM_HSHADOW_BG_CONTRAST                                               "          BG Height Contrast (Depth Details Multiplier)"  100 0 300 1
float HSM_HSHADOW_BG_CONTRAST = global.HSM_HSHADOW_BG_CONTRAST / 100.0;

#pragma parameter HSM_HSHADOW_BG_OFFSET_Z                                               "          BG Height Offset Z (Depth)"  0 -100 300 1
float HSM_HSHADOW_BG_OFFSET_Z = global.HSM_HSHADOW_BG_OFFSET_Z / 100.0;


#pragma parameter HSM_HSHADOW_DEVICE_CONTRAST_CENTER                                    "          Device Height Contrast Center"  100 0 100 1
float HSM_HSHADOW_DEVICE_CONTRAST_CENTER = global.HSM_HSHADOW_DEVICE_CONTRAST_CENTER / 100.0;

#pragma parameter HSM_HSHADOW_DEVICE_CONTRAST                                           "          Device Height Contrast (Depth Details Multiplier)"  100 0 300 1
float HSM_HSHADOW_DEVICE_CONTRAST = global.HSM_HSHADOW_DEVICE_CONTRAST / 100.0;

#pragma parameter HSM_HSHADOW_DEVICE_OFFSET_Z                                           "          Device Height Offset Z"  0 -100 300 1
float HSM_HSHADOW_DEVICE_OFFSET_Z = global.HSM_HSHADOW_DEVICE_OFFSET_Z / 100.0;


#pragma parameter HSM_BACKGROUND_LAYER_GROUP_EMPTY_LINE			            " " 0 0 0.001 0.001
#pragma parameter HSM_BACKGROUND_LAYER_GROUP_TITLE			                "[ COMPOSITE BACKGROUND LAYER GROUP ]:" 0 0 0.01 0.01

#pragma parameter HSM_COMPOSITE_BACKGROUND_ON                               "          Layer Group ON"  1 0 1 1
float HSM_COMPOSITE_BACKGROUND_ON = global.HSM_COMPOSITE_BACKGROUND_ON;

// #pragma parameter HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER         "          Static Ambient Brightness Multiplier"  100 0 300 1
// float HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER = global.HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER / 100;
float HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER = 1;

// #pragma parameter HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER           "          Night Lighting 1st  Multiplier"  0 0 100 1
// float HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER = global.HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER / 100;
float HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER = 0;

// #pragma parameter HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER           "          Night Lighting 2nd  Multiplier"  100 0 100 1
// float HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER = global.HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER / 100; 
float HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER = 1.0; 

// Not sure this is helpful, could have params to control how much Background Shadow, Device Shadow, etc
// #pragma parameter HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH           "          Static Shadow Strength"  100 0 300 1
// float HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH = global.HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH / 100; 
float HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH = 1; 

#pragma parameter HSM_COMPOSITE_BACKGROUND_BOUNCELIGHT_MULTIPLIER           "          Bouncelight Brightness Multiplier"  50 0 300 1
float HSM_COMPOSITE_BACKGROUND_BOUNCELIGHT_MULTIPLIER = global.HSM_COMPOSITE_BACKGROUND_BOUNCELIGHT_MULTIPLIER / 100;


#pragma parameter HSM_MIDDLEGROUND_LAYER_GROUP_EMPTY_LINE			        " " 0 0 0.001 0.001
#pragma parameter HSM_MIDDLEGROUND_LAYER_GROUP_TITLE			            "[ COMPOSITE MIDDLEGROUND LAYER GROUP ]:" 0 0 0.01 0.01

#pragma parameter HSM_COMPOSITE_MIDDLEGROUND_ON                             "          Layer Group ON"  1 0 1 1
float HSM_COMPOSITE_MIDDLEGROUND_ON = global.HSM_COMPOSITE_MIDDLEGROUND_ON;

// #pragma parameter HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER      "          Static Ambient Brightness Multiplier"  100 0 300 1
// float HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER = global.HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER / 100;
float HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER = 1;

// #pragma parameter HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER         "          Night Lighting 1st  Multiplier"  100 0 100 1
// float HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER = global.HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER / 100;
float HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER = 100.0;

// #pragma parameter HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER         "          Night Lighting 2nd  Multiplier"  0 0 100 1
// float HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER = global.HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER / 100;
float HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER = 1.0;

#pragma parameter HSM_COMPOSITE_MIDDLEGROUND_BOUNCELIGHT_MULTIPLIER         "          Bouncelight Brightness Multiplier"  100 0 300 1
float HSM_COMPOSITE_MIDDLEGROUND_BOUNCELIGHT_MULTIPLIER = global.HSM_COMPOSITE_MIDDLEGROUND_BOUNCELIGHT_MULTIPLIER / 100;

#pragma parameter HSM_FOREGROUND_LAYER_GROUP_EMPTY_LINE			            " " 0 0 0.001 0.001
#pragma parameter HSM_FOREGROUND_LAYER_GROUP_TITLE			                "[ COMPOSITE FOREGROUND LAYER GROUP ]:" 0 0 0.01 0.01

#pragma parameter HSM_COMPOSITE_FOREGROUND_ON                               "          Layer Group ON"  1 0 1 1
float HSM_COMPOSITE_FOREGROUND_ON = global.HSM_COMPOSITE_FOREGROUND_ON;

// #pragma parameter HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER         "          Static Ambient Brightness Multiplier"  100 0 300 1
// float HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER = global.HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER / 100;
float HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER = 1;

// #pragma parameter HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER           "          Night Lighting 1st  Multiplier"  100 0 100 1
// float HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER = global.HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER / 100;
float HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER = 1.0;

// #pragma parameter HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER           "          Night Lighting 2nd  Multiplier"  0 0 100 1
// float HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER = global.HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER / 100;
float HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER = 0;

#pragma parameter HSM_COMPOSITE_FOREGROUND_BOUNCELIGHT_MULTIPLIER           "          Bouncelight Brightness Multiplier"  100 0 300 1
float HSM_COMPOSITE_FOREGROUND_BOUNCELIGHT_MULTIPLIER = global.HSM_COMPOSITE_FOREGROUND_BOUNCELIGHT_MULTIPLIER / 100;


#pragma parameter HSM_ADDITIVE_LAYER_GROUP_EMPTY_LINE			            " " 0 0 0.001 0.001
#pragma parameter HSM_ADDITIVE_LAYER_GROUP_TITLE			                "[ COMPOSITE ADDITIVE & CRT ]:" 0 0 0.01 0.01

#pragma parameter HSM_COMPOSITE_ADDITIVE_ON                                 "          Additive Composite ON"  1 0 1 1
float HSM_COMPOSITE_ADDITIVE_ON = global.HSM_COMPOSITE_ADDITIVE_ON;

#pragma parameter HSM_COMPOSITE_ADDITIVE_BRIGHTNESS                         "          Additive Brightness"  100 0 300 1
float HSM_COMPOSITE_ADDITIVE_BRIGHTNESS = global.HSM_COMPOSITE_ADDITIVE_BRIGHTNESS / 100;

#pragma parameter HSM_COMPOSITE_CRT_AND_REFLECTION_ON                       "          CRT & Reflection Composite ON"  1 0 1 1
float HSM_COMPOSITE_CRT_AND_REFLECTION_ON = global.HSM_COMPOSITE_CRT_AND_REFLECTION_ON;


//---------------------------------------------------------------------------------------------------
//    RENDER FOR SIMPLIFIED PRESETS
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_RENDER_FOR_SIMPLIFIED_EMPTY_LINE	" " 0 0 0.001 0.001
#pragma parameter HSM_RENDER_FOR_SIMPLIFIED_TITLE       "[ RENDER FOR SIMPLIFIED PRESETS ]:" 0 0 0.001 0.001

#pragma parameter HSM_RENDER_SIMPLE_MODE                "     Mode:  NORM | BLACK-SCREEN | BLACK-BG | PINK-MASK| BW-MASK" 0 0 4 1
float HSM_RENDER_SIMPLE_MODE = global.HSM_RENDER_SIMPLE_MODE;

#pragma parameter HSM_RENDER_SIMPLE_MASK_TYPE           "      Mask Type:  SCREEN | TUBE | BEZEL & FRAME | BEZEL | FRAME" 0 0 4 1
float HSM_RENDER_SIMPLE_MASK_TYPE = global.HSM_RENDER_SIMPLE_MASK_TYPE;


//---------------------------------------------------------------------------------------------------
//    MASK DEBUG
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_MASK_DEBUG_EMPTY_LINE		    " " 0 0 0.001 0.001
#pragma parameter HSM_MASK_DEBUG_TITLE			    "[ MASK DEBUG ]:" 0 0 0.01 0.01

#pragma parameter HSM_LAYERING_DEBUG_MASK_MODE      "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" -1 -2 8 1
float HSM_LAYERING_DEBUG_MASK_MODE = global.HSM_LAYERING_DEBUG_MASK_MODE;


//---------------------------------------------------------------------------------------------------
//    CutOut in  Layers
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_CUTOUT_EMPTY_LINE			 " " 0 0 0.001 0.001
#pragma parameter HSM_CUTOUT_TITLE			     "[ CUTOUT ]:" 0 0 0.01 0.01

#pragma parameter HSM_CUTOUT_ASPECT_MODE        "         Cutout Aspect- FULL | EXPL. | 4:3 | 3:4 | 16:9 | 9:16" 4 0 5 1
float HSM_CUTOUT_ASPECT_MODE = global.HSM_CUTOUT_ASPECT_MODE;

#pragma parameter HSM_CUTOUT_EXPLICIT_ASPECT    "          Explicit Aspect" 1.33333 0.01 3 0.01
float HSM_CUTOUT_EXPLICIT_ASPECT = global.HSM_CUTOUT_EXPLICIT_ASPECT;

#pragma parameter HSM_CUTOUT_FOLLOW_LAYER       " Follow Layer - FULL | TUBE | BZL | BG | DEVICE | DECAL"  0 0 5 1
float HSM_CUTOUT_FOLLOW_LAYER = global.HSM_CUTOUT_FOLLOW_LAYER;

#pragma parameter HSM_CUTOUT_FOLLOW_FULL_USES_ZOOM     "          Follow Full also follows Zoom" 1 0 1 1
float HSM_CUTOUT_FOLLOW_FULL_USES_ZOOM = global.HSM_CUTOUT_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_CUTOUT_SCALE 				"          Scale" 100 -100 200 0.1
float HSM_CUTOUT_SCALE = global.HSM_CUTOUT_SCALE / 100;

#pragma parameter HSM_CUTOUT_SCALE_X 		    "          Scale X" 100 -100 200 0.1
float HSM_CUTOUT_SCALE_X = global.HSM_CUTOUT_SCALE_X / 100;

#pragma parameter HSM_CUTOUT_POS_X 		        "          Position XY" 0 -100 200 0.05
float HSM_CUTOUT_POS_X = global.HSM_CUTOUT_POS_X / -100;

#pragma parameter HSM_CUTOUT_POS_Y 		        "          Position Y" 0 -100 200 0.05
float HSM_CUTOUT_POS_Y = global.HSM_CUTOUT_POS_Y / 100;

#pragma parameter HSM_CUTOUT_CORNER_RADIUS 		"          Corner Radius - Def 0" 0 0 70 1
float HSM_CUTOUT_CORNER_RADIUS = global.HSM_CUTOUT_CORNER_RADIUS;


//---------------------------------------------------------------------------------------------------
//    REFLECTION MASK
//---------------------------------------------------------------------------------------------------
#include "params-2-reflection-mask.inc"


//---------------------------------------------------------------------------------------------------
//    BG IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_BACKGROUND_EMPTY_LINE		  " " 0 0 0.001 0.001
#pragma parameter HSM_BACKGROUND_TITLE			  "[ BACKGROUND LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_OPACITY                  "          BG Opacity" 100 0 100 1
float HSM_BG_OPACITY = global.HSM_BG_OPACITY / 100;

#pragma parameter HSM_BG_COLORIZE_ON              "          Colorize On" 0 0 1 1
float HSM_BG_COLORIZE_ON = global.HSM_BG_COLORIZE_ON;

#pragma parameter HSM_BG_HUE                       "          Hue Offset" 0 0 360 1
float HSM_BG_HUE = global.HSM_BG_HUE / 360;

#pragma parameter HSM_BG_SATURATION                "          Saturation" 100 0 200 1
float HSM_BG_SATURATION = global.HSM_BG_SATURATION / 100;

#pragma parameter HSM_BG_BRIGHTNESS                "          BG Brightness" 100 0 200 1
float HSM_BG_BRIGHTNESS = global.HSM_BG_BRIGHTNESS / 100;

#pragma parameter HSM_BG_GAMMA                     "          Gamma Adjust" 1 0 5 0.01
float HSM_BG_GAMMA = global.HSM_BG_GAMMA;

#pragma parameter HSM_BG_BLEND_MODE               "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 -1 3 1
float HSM_BG_BLEND_MODE = global.HSM_BG_BLEND_MODE;

#pragma parameter HSM_BG_SOURCE_MATTE_TYPE        "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_BG_SOURCE_MATTE_TYPE = global.HSM_BG_SOURCE_MATTE_TYPE;

#pragma parameter HSM_BG_ADD_MODE_NIGHTLIGHTING_AMOUNT    "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_BG_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_BG_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_BG_MIPMAPPING_BLEND_BIAS    "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_BG_MIPMAPPING_BLEND_BIAS = global.HSM_BG_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_BG_MASK_MODE                "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" 0 0 8 1
float HSM_BG_MASK_MODE = global.HSM_BG_MASK_MODE;

#pragma parameter HSM_BG_CUTOUT_MODE              "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_BG_CUTOUT_MODE = global.HSM_BG_CUTOUT_MODE;

#pragma parameter HSM_BG_DUALSCREEN_VIS_MODE      "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_BG_DUALSCREEN_VIS_MODE = global.HSM_BG_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_BG2_EMPTY_LINE				" " 0 0 0.001 0.001
#pragma parameter HSM_BG_TITLE2			            "[ BACKGROUND SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_FOLLOW_LAYER               "          Follow Layer - FULL | TUBE | BEZEL" 0 0 2 1
float HSM_BG_FOLLOW_LAYER = global.HSM_BG_FOLLOW_LAYER;

#pragma parameter HSM_BG_FOLLOW_MODE                "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_BG_FOLLOW_MODE = global.HSM_BG_FOLLOW_MODE;

#pragma parameter HSM_BG_FOLLOW_FULL_USES_ZOOM      "          Follow Full also follows Zoom" 1 0 1 1
float HSM_BG_FOLLOW_FULL_USES_ZOOM = global.HSM_BG_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_BG_FILL_MODE                  "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_BG_FILL_MODE = global.HSM_BG_FILL_MODE;

#pragma parameter HSM_BG_SPLIT_PRESERVE_CENTER      "          Split Mode Preserve Center %"  0 0 500 1
float HSM_BG_SPLIT_PRESERVE_CENTER = global.HSM_BG_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_BG_SPLIT_REPEAT_WIDTH         "          Split Mode Repeat Width %"  0 0 100 1
float HSM_BG_SPLIT_REPEAT_WIDTH = global.HSM_BG_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_BG_SCALE                      "          Scale" 100 0 1000 0.1
float HSM_BG_SCALE = global.HSM_BG_SCALE / 100;

#pragma parameter HSM_BG_SCALE_X                    "          Scale X" 100 0 1000 0.1
float HSM_BG_SCALE_X = global.HSM_BG_SCALE_X / 100;

#pragma parameter HSM_BG_POS_X                      "          Position X"  0 -1000 1000 0.05
float HSM_BG_POS_X = global.HSM_BG_POS_X / -100;

#pragma parameter HSM_BG_POS_Y                      "          Position Y"  0 -1000 1000 0.05
float HSM_BG_POS_Y = global.HSM_BG_POS_Y / 100;

#pragma parameter HSM_BG_WRAP_MODE                  "          Wrap Mode: NONE | TILED | MIRRORED" 1 0 2 1
float HSM_BG_WRAP_MODE = global.HSM_BG_WRAP_MODE;


//---------------------------------------------------------------------------------------------------
//    VIEWPORT VIGNETTE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_VIEWPORT_VIGNETTE_EMPTY_LINE	" " 0 0 0.001 0.001
#pragma parameter HSM_VIEWPORT_VIGNETTE_TITLE		"[ VIEWPORT VIGNETTE LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_VIEWPORT_VIGNETTE_OPACITY     "          Opacity" 0 0 100 2
float HSM_VIEWPORT_VIGNETTE_OPACITY = global.HSM_VIEWPORT_VIGNETTE_OPACITY / 100;

#pragma parameter HSM_VIEWPORT_VIGNETTE_MASK_MODE   "Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" 5 0 8 1
float HSM_VIEWPORT_VIGNETTE_MASK_MODE = global.HSM_VIEWPORT_VIGNETTE_MASK_MODE;

#pragma parameter HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE  "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE = global.HSM_VIEWPORT_VIGNETTE_CUTOUT_MODE;

#pragma parameter HSM_VIEWPORT_VIGNETTE_FOLLOW_LAYER "          Follow Layer- FULL | BG | TUBE | BZL" 0 0 5 1
float HSM_VIEWPORT_VIGNETTE_FOLLOW_LAYER = global.HSM_VIEWPORT_VIGNETTE_FOLLOW_LAYER;

#pragma parameter HSM_VIEWPORT_VIGNETTE_SCALE        "          Scale" 100 0 1000 0.1
float HSM_VIEWPORT_VIGNETTE_SCALE = global.HSM_VIEWPORT_VIGNETTE_SCALE / 100;

#pragma parameter HSM_VIEWPORT_VIGNETTE_SCALE_X      "          Scale X" 100 0 1000 0.1
float HSM_VIEWPORT_VIGNETTE_SCALE_X = global.HSM_VIEWPORT_VIGNETTE_SCALE_X / 100;

#pragma parameter HSM_VIEWPORT_VIGNETTE_POS_X        "          Position X"  0 -1000 1000 0.05
float HSM_VIEWPORT_VIGNETTE_POS_X = global.HSM_VIEWPORT_VIGNETTE_POS_X / -100;

#pragma parameter HSM_VIEWPORT_VIGNETTE_POS_Y        "          Position Y"  0 -1000 1000 0.05
float HSM_VIEWPORT_VIGNETTE_POS_Y = global.HSM_VIEWPORT_VIGNETTE_POS_Y / 100;


//---------------------------------------------------------------------------------------------------
//    LED IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_LEDX_EMPTY_LINE				" " 0 0 0.001 0.001
#pragma parameter HSM_LED_TITLE		                "[ LED LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_LED_OPACITY                   "          Opacity"  100 0 100 1
float HSM_LED_OPACITY = global.HSM_LED_OPACITY / 100;

#pragma parameter HSM_LED_COLORIZE_ON               "          Colorize" 0 0 1 1
float HSM_LED_COLORIZE_ON = global.HSM_LED_COLORIZE_ON;

#pragma parameter HSM_LED_HUE                       "          Hue Offset" 0 0 360 1
float HSM_LED_HUE = global.HSM_LED_HUE / 360;

#pragma parameter HSM_LED_SATURATION                "          Saturation" 100 0 200 1
float HSM_LED_SATURATION = global.HSM_LED_SATURATION / 100;

#pragma parameter HSM_LED_BRIGHTNESS                "          Brightness" 100 0 200 1
float HSM_LED_BRIGHTNESS = global.HSM_LED_BRIGHTNESS / 100;

#pragma parameter HSM_LED_GAMMA                     "          Gamma Adjust" 1 0 5 0.01
float HSM_LED_GAMMA = global.HSM_LED_GAMMA;

#pragma parameter HSM_LED_BLEND_MODE                    "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  2 0 3 1
float HSM_LED_BLEND_MODE = global.HSM_LED_BLEND_MODE;

#pragma parameter HSM_LED_SOURCE_MATTE_TYPE             "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_LED_SOURCE_MATTE_TYPE = global.HSM_LED_SOURCE_MATTE_TYPE;

#pragma parameter HSM_LED_ADD_MODE_NIGHTLIGHTING_AMOUNT   "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_LED_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_LED_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_LED_MIPMAPPING_BLEND_BIAS         "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_LED_MIPMAPPING_BLEND_BIAS = global.HSM_LED_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_LED_MASK_MODE                     "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG"   0 0 8 1
float HSM_LED_MASK_MODE = global.HSM_LED_MASK_MODE;

#pragma parameter HSM_LED_CUTOUT_MODE                   "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_LED_CUTOUT_MODE = global.HSM_LED_CUTOUT_MODE;

#pragma parameter HSM_LED_DUALSCREEN_VIS_MODE           "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_LED_DUALSCREEN_VIS_MODE = global.HSM_LED_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_LED2_EMPTY_LINE					" " 0 0 0.001 0.001
#pragma parameter HSM_LED_TITLE2		                "[ LED SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_LED_FOLLOW_LAYER                  "             Follow Layer - FULL | TUBE | BEZEL | BG | DEVICE"  3 0 4 1
float HSM_LED_FOLLOW_LAYER = global.HSM_LED_FOLLOW_LAYER;

#pragma parameter HSM_LED_FOLLOW_MODE                   "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_LED_FOLLOW_MODE = global.HSM_LED_FOLLOW_MODE;

#pragma parameter HSM_LED_FOLLOW_FULL_USES_ZOOM         "          Follow Full also follows Zoom" 1 0 1 1
float HSM_LED_FOLLOW_FULL_USES_ZOOM = global.HSM_LED_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_LED_FILL_MODE                     "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_LED_FILL_MODE = global.HSM_LED_FILL_MODE;

#pragma parameter HSM_LED_SPLIT_PRESERVE_CENTER         "          Split Mode Preserve Center %"  0 0 500 1
float HSM_LED_SPLIT_PRESERVE_CENTER = global.HSM_LED_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_LED_SPLIT_REPEAT_WIDTH            "          Split Mode Repeat Width %"  0 0 100 1
float HSM_LED_SPLIT_REPEAT_WIDTH = global.HSM_LED_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_LED_SCALE                         "          Scale" 100 0 1000 0.1
float HSM_LED_SCALE = global.HSM_LED_SCALE / 100;

#pragma parameter HSM_LED_SCALE_X                       "          Scale X" 100 0 1000 0.1
float HSM_LED_SCALE_X = global.HSM_LED_SCALE_X / 100;

#pragma parameter HSM_LED_POS_X                         "          Position X"  0 -1000 1000 0.05
float HSM_LED_POS_X = global.HSM_LED_POS_X / -100;

#pragma parameter HSM_LED_POS_Y                         "          Position Y"  0 -1000 1000 0.05
float HSM_LED_POS_Y = global.HSM_LED_POS_Y / 100;


//---------------------------------------------------------------------------------------------------
//    DEVICE IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DEVICE_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_DEVICE_TITLE			           "[ DEVICE IMAGE LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_DEVICE_OPACITY                   "          Opacity" 100 0 100 1
float HSM_DEVICE_OPACITY = global.HSM_DEVICE_OPACITY / 100;

#pragma parameter HSM_DEVICE_COLORIZE_ON               "          Colorize" 0 0 1 1
float HSM_DEVICE_COLORIZE_ON = global.HSM_DEVICE_COLORIZE_ON;

#pragma parameter HSM_DEVICE_HUE                       "          Hue Offset" 0 0 360 1
float HSM_DEVICE_HUE = global.HSM_DEVICE_HUE / 360;

#pragma parameter HSM_DEVICE_SATURATION                "          Saturation" 100 0 200 1
float HSM_DEVICE_SATURATION = global.HSM_DEVICE_SATURATION / 100;

#pragma parameter HSM_DEVICE_BRIGHTNESS                "          Brightness" 100 0 200 1
float HSM_DEVICE_BRIGHTNESS = global.HSM_DEVICE_BRIGHTNESS / 100;

#pragma parameter HSM_DEVICE_GAMMA                     "          Gamma Adjust" 1 0 5 0.01
float HSM_DEVICE_GAMMA = global.HSM_DEVICE_GAMMA;

#pragma parameter HSM_DEVICE_BLEND_MODE                "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 0 3 1
float HSM_DEVICE_BLEND_MODE = global.HSM_DEVICE_BLEND_MODE;

#pragma parameter HSM_DEVICE_SOURCE_MATTE_TYPE         "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_DEVICE_SOURCE_MATTE_TYPE = global.HSM_DEVICE_SOURCE_MATTE_TYPE;

#pragma parameter HSM_DEVICE_ADD_MODE_NIGHTLIGHTING_AMOUNT    "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_DEVICE_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_DEVICE_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_DEVICE_MIPMAPPING_BLEND_BIAS     "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_DEVICE_MIPMAPPING_BLEND_BIAS = global.HSM_DEVICE_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_DEVICE_MASK_MODE                 "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" 0 0 8 1
float HSM_DEVICE_MASK_MODE = global.HSM_DEVICE_MASK_MODE;

#pragma parameter HSM_DEVICE_CUTOUT_MODE               "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_DEVICE_CUTOUT_MODE = global.HSM_DEVICE_CUTOUT_MODE;

#pragma parameter HSM_DEVICE_DUALSCREEN_VIS_MODE       "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_DEVICE_DUALSCREEN_VIS_MODE = global.HSM_DEVICE_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DEVICE2_EMPTY_LINE			   " " 0 0 0.001 0.001
#pragma parameter HSM_DEVICE_TITLE2			           "[ DEVICE SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_DEVICE_FOLLOW_LAYER              "          Follow Layer - FULL | TUBE | BEZEL | BG"  1 0 3 1
float HSM_DEVICE_FOLLOW_LAYER = global.HSM_DEVICE_FOLLOW_LAYER;

#pragma parameter HSM_DEVICE_FOLLOW_MODE               "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_DEVICE_FOLLOW_MODE = global.HSM_DEVICE_FOLLOW_MODE;

#pragma parameter HSM_DEVICE_FOLLOW_FULL_USES_ZOOM     "          Follow Full also follows Zoom" 1 0 1 1
float HSM_DEVICE_FOLLOW_FULL_USES_ZOOM = global.HSM_DEVICE_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_DEVICE_FILL_MODE                 "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_DEVICE_FILL_MODE = global.HSM_DEVICE_FILL_MODE;

#pragma parameter HSM_DEVICE_SPLIT_PRESERVE_CENTER     "          Split Mode Preserve Center %"  0 0 500 1
float HSM_DEVICE_SPLIT_PRESERVE_CENTER = global.HSM_DEVICE_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_DEVICE_SPLIT_REPEAT_WIDTH        "          Split Mode Repeat Width %"  0 0 100 1
float HSM_DEVICE_SPLIT_REPEAT_WIDTH = global.HSM_DEVICE_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_DEVICE_SCALE                     "          Scale" 100 0 1000 0.1
float HSM_DEVICE_SCALE = global.HSM_DEVICE_SCALE / 100;

#pragma parameter HSM_DEVICE_SCALE_X                   "          Scale X" 100 0 1000 0.1
float HSM_DEVICE_SCALE_X = global.HSM_DEVICE_SCALE_X / 100;

#pragma parameter HSM_DEVICE_POS_X                     "          Position X"  0 -1000 1000 0.05
float HSM_DEVICE_POS_X = global.HSM_DEVICE_POS_X / -100;

#pragma parameter HSM_DEVICE_POS_Y                     "          Position Y"  0 -1000 1000 0.05
float HSM_DEVICE_POS_Y = global.HSM_DEVICE_POS_Y / 100;



//---------------------------------------------------------------------------------------------------
//    DEVICELED IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DEVICELED_EMPTY_LINE				    " " 0 0 0.001 0.001
#pragma parameter HSM_DEVICELED_TITLE			          "[ DEVICE LED IMAGE LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_DEVICELED_OPACITY                   "          Opacity" 100 0 100 1
float HSM_DEVICELED_OPACITY = global.HSM_DEVICELED_OPACITY / 100;

#pragma parameter HSM_DEVICELED_COLORIZE_ON               "          Colorize" 0 0 1 1
float HSM_DEVICELED_COLORIZE_ON = global.HSM_DEVICELED_COLORIZE_ON;

#pragma parameter HSM_DEVICELED_HUE                       "          Hue Offset" 0 0 360 1
float HSM_DEVICELED_HUE = global.HSM_DEVICELED_HUE / 360;

#pragma parameter HSM_DEVICELED_SATURATION                "          Saturation" 100 0 200 1
float HSM_DEVICELED_SATURATION = global.HSM_DEVICELED_SATURATION / 100;

#pragma parameter HSM_DEVICELED_BRIGHTNESS                "          Brightness" 100 0 200 1
float HSM_DEVICELED_BRIGHTNESS = global.HSM_DEVICELED_BRIGHTNESS / 100;

#pragma parameter HSM_DEVICELED_GAMMA                     "          Gamma Adjust" 1 0 5 0.01
float HSM_DEVICELED_GAMMA = global.HSM_DEVICELED_GAMMA;

#pragma parameter HSM_DEVICELED_BLEND_MODE                "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  2 0 3 1
float HSM_DEVICELED_BLEND_MODE = global.HSM_DEVICELED_BLEND_MODE;

#pragma parameter HSM_DEVICELED_SOURCE_MATTE_TYPE         "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_DEVICELED_SOURCE_MATTE_TYPE = global.HSM_DEVICELED_SOURCE_MATTE_TYPE;

#pragma parameter HSM_DEVICELED_ADD_MODE_NIGHTLIGHTING_AMOUNT "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_DEVICELED_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_DEVICELED_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_DEVICELED_MIPMAPPING_BLEND_BIAS       "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_DEVICELED_MIPMAPPING_BLEND_BIAS = global.HSM_DEVICELED_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_DEVICELED_MASK_MODE                 "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" 0 0 8 1
float HSM_DEVICELED_MASK_MODE = global.HSM_DEVICELED_MASK_MODE;

#pragma parameter HSM_DEVICELED_CUTOUT_MODE               "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_DEVICELED_CUTOUT_MODE = global.HSM_DEVICELED_CUTOUT_MODE;

#pragma parameter HSM_DEVICELED_DUALSCREEN_VIS_MODE       "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_DEVICELED_DUALSCREEN_VIS_MODE = global.HSM_DEVICELED_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DEVICELED2_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_DEVICELED_TITLE2			          "[ DEVICE LED SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_DEVICELED_FOLLOW_LAYER              "          Follow Layer - FULL | TUBE | BEZEL | BG | DEVICE"  4 0 4 1
float HSM_DEVICELED_FOLLOW_LAYER = global.HSM_DEVICELED_FOLLOW_LAYER;

#pragma parameter HSM_DEVICELED_FOLLOW_MODE               "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 1 0 1 1
float HSM_DEVICELED_FOLLOW_MODE = global.HSM_DEVICELED_FOLLOW_MODE;

#pragma parameter HSM_DEVICELED_FOLLOW_FULL_USES_ZOOM     "          Follow Full also follows Zoom" 1 0 1 1
float HSM_DEVICELED_FOLLOW_FULL_USES_ZOOM = global.HSM_DEVICELED_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_DEVICELED_FILL_MODE                 "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_DEVICELED_FILL_MODE = global.HSM_DEVICELED_FILL_MODE;

#pragma parameter HSM_DEVICELED_SPLIT_PRESERVE_CENTER     "          Split Mode Preserve Center %"  0 0 500 1
float HSM_DEVICELED_SPLIT_PRESERVE_CENTER = global.HSM_DEVICELED_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_DEVICELED_SPLIT_REPEAT_WIDTH        "          Split Mode Repeat Width %"  0 0 100 1
float HSM_DEVICELED_SPLIT_REPEAT_WIDTH = global.HSM_DEVICELED_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_DEVICELED_SCALE                     "          Scale" 100 0 1000 0.1
float HSM_DEVICELED_SCALE = global.HSM_DEVICELED_SCALE / 100;

#pragma parameter HSM_DEVICELED_SCALE_X                   "          Scale X" 100 0 1000 0.1
float HSM_DEVICELED_SCALE_X = global.HSM_DEVICELED_SCALE_X / 100;

#pragma parameter HSM_DEVICELED_POS_X                     "          Position X"  0 -1000 1000 0.05
float HSM_DEVICELED_POS_X = global.HSM_DEVICELED_POS_X / -100;

#pragma parameter HSM_DEVICELED_POS_Y                     "          Position Y"  0 -1000 1000 0.05
float HSM_DEVICELED_POS_Y = global.HSM_DEVICELED_POS_Y / 100;


//---------------------------------------------------------------------------------------------------
//    DECAL IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DECAL_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_DECAL_TITLE			            "[ DECAL LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_DECAL_OPACITY                     "          Opacity"  100 0 100 0.1
float HSM_DECAL_OPACITY = global.HSM_DECAL_OPACITY / 100;

#pragma parameter HSM_DECAL_COLORIZE_ON                 "          Colorize" 0 0 1 1
float HSM_DECAL_COLORIZE_ON = global.HSM_DECAL_COLORIZE_ON;

#pragma parameter HSM_DECAL_HUE                         "          Hue Offset" 0 0 360 1
float HSM_DECAL_HUE = global.HSM_DECAL_HUE / 360;

#pragma parameter HSM_DECAL_SATURATION                  "          Saturation" 100 0 200 1
float HSM_DECAL_SATURATION = global.HSM_DECAL_SATURATION / 100;

#pragma parameter HSM_DECAL_BRIGHTNESS                  "          Brightness" 100 0 200 1
float HSM_DECAL_BRIGHTNESS = global.HSM_DECAL_BRIGHTNESS / 100;

#pragma parameter HSM_DECAL_GAMMA                       "          Gamma Adjust" 1 0 5 0.01
float HSM_DECAL_GAMMA = global.HSM_DECAL_GAMMA;

#pragma parameter HSM_DECAL_BLEND_MODE                  "          Blend Mode -  OFF | NORMAL | ADD | MULTIPLY"  1 0 3 1
float HSM_DECAL_BLEND_MODE = global.HSM_DECAL_BLEND_MODE;

#pragma parameter HSM_DECAL_SOURCE_MATTE_TYPE           "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_DECAL_SOURCE_MATTE_TYPE = global.HSM_DECAL_SOURCE_MATTE_TYPE;

#pragma parameter HSM_DECAL_ADD_MODE_NIGHTLIGHTING_AMOUNT "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_DECAL_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_DECAL_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_DECAL_MIPMAPPING_BLEND_BIAS       "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_DECAL_MIPMAPPING_BLEND_BIAS = global.HSM_DECAL_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_DECAL_MASK_MODE                   "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG"   0 0 8 1
float HSM_DECAL_MASK_MODE = global.HSM_DECAL_MASK_MODE;

#pragma parameter HSM_DECAL_CUTOUT_MODE                 "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_DECAL_CUTOUT_MODE = global.HSM_DECAL_CUTOUT_MODE;

#pragma parameter HSM_DECAL_DUALSCREEN_VIS_MODE         "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_DECAL_DUALSCREEN_VIS_MODE = global.HSM_DECAL_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_DECAL2_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_DECAL_TITLE2			            "[ DECAL SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_DECAL_FOLLOW_LAYER                "          Follow Layer - FULL | TUBE | BEZEL | BG | DEVICE"  0 0 4 1
float HSM_DECAL_FOLLOW_LAYER = global.HSM_DECAL_FOLLOW_LAYER;

#pragma parameter HSM_DECAL_FOLLOW_MODE                 "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_DECAL_FOLLOW_MODE = global.HSM_DECAL_FOLLOW_MODE;

#pragma parameter HSM_DECAL_FOLLOW_FULL_USES_ZOOM       "          Follow Full also follows Zoom" 1 0 1 1
float HSM_DECAL_FOLLOW_FULL_USES_ZOOM = global.HSM_DECAL_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_DECAL_FILL_MODE                   "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_DECAL_FILL_MODE = global.HSM_DECAL_FILL_MODE;

#pragma parameter HSM_DECAL_SPLIT_PRESERVE_CENTER       "          Split Mode Preserve Center %"  0 0 500 1
float HSM_DECAL_SPLIT_PRESERVE_CENTER = global.HSM_DECAL_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_DECAL_SPLIT_REPEAT_WIDTH          "          Split Mode Repeat Width %"  0 0 100 1
float HSM_DECAL_SPLIT_REPEAT_WIDTH = global.HSM_DECAL_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_DECAL_SCALE                       "          Scale"  100 1 500 0.1
float HSM_DECAL_SCALE = global.HSM_DECAL_SCALE / 100;

#pragma parameter HSM_DECAL_SCALE_X                     "          Scale X"  100 50 500 0.1
float HSM_DECAL_SCALE_X = global.HSM_DECAL_SCALE_X / 100;

#pragma parameter HSM_DECAL_POS_X                       "          Position X"  0 -1000 1000 0.05
float HSM_DECAL_POS_X = global.HSM_DECAL_POS_X / -100;

#pragma parameter HSM_DECAL_POS_Y                       "          Position Y"  0 -1000 1000 0.05
float HSM_DECAL_POS_Y = global.HSM_DECAL_POS_Y / 100;


//---------------------------------------------------------------------------------------------------
//    CAB GLS IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_CAB_GLASS_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_CAB_GLASS_TITLE			            "[ CABINET GLASS LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_CAB_GLASS_OPACITY                     "          Opacity" 100 0 100 1
float HSM_CAB_GLASS_OPACITY = global.HSM_CAB_GLASS_OPACITY / 100;

#pragma parameter HSM_CAB_GLASS_COLORIZE_ON                 "          Colorize" 0 0 1 1
float HSM_CAB_GLASS_COLORIZE_ON = global.HSM_CAB_GLASS_COLORIZE_ON;

#pragma parameter HSM_CAB_GLASS_HUE                         "          Hue Offset" 0 0 360 1
float HSM_CAB_GLASS_HUE = global.HSM_CAB_GLASS_HUE / 360;

#pragma parameter HSM_CAB_GLASS_SATURATION                  "          Saturation" 100 0 200 1
float HSM_CAB_GLASS_SATURATION = global.HSM_CAB_GLASS_SATURATION / 100;

#pragma parameter HSM_CAB_GLASS_BRIGHTNESS                  "          Brightness" 100 0 400 1
float HSM_CAB_GLASS_BRIGHTNESS = global.HSM_CAB_GLASS_BRIGHTNESS / 100;

#pragma parameter HSM_CAB_GLASS_GAMMA                       "          Gamma Adjust" 1 0 5 0.01
float HSM_CAB_GLASS_GAMMA = global.HSM_CAB_GLASS_GAMMA;

#pragma parameter HSM_CAB_GLASS_BLEND_MODE                  "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 0 3 1
float HSM_CAB_GLASS_BLEND_MODE = global.HSM_CAB_GLASS_BLEND_MODE;
    
#pragma parameter HSM_CAB_GLASS_SOURCE_MATTE_TYPE           "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_CAB_GLASS_SOURCE_MATTE_TYPE = global.HSM_CAB_GLASS_SOURCE_MATTE_TYPE;

#pragma parameter HSM_CAB_GLASS_ADD_MODE_NIGHTLIGHTING_AMOUNT "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_CAB_GLASS_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_CAB_GLASS_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_CAB_GLASS_MIPMAPPING_BLEND_BIAS       "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_CAB_GLASS_MIPMAPPING_BLEND_BIAS = global.HSM_CAB_GLASS_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_CAB_GLASS_MASK_MODE                   "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG" 0 0 8 1
float HSM_CAB_GLASS_MASK_MODE = global.HSM_CAB_GLASS_MASK_MODE;

#pragma parameter HSM_CAB_GLASS_CUTOUT_MODE                 "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_CAB_GLASS_CUTOUT_MODE = global.HSM_CAB_GLASS_CUTOUT_MODE;

#pragma parameter HSM_CAB_GLASS_DUALSCREEN_VIS_MODE         "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_CAB_GLASS_DUALSCREEN_VIS_MODE = global.HSM_CAB_GLASS_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_CAB_GLASS2_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_CAB_GLASS_TITLE2			            "[ CABINET GLASS SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_CAB_GLASS_FOLLOW_LAYER                "        Follow Layer- FULL | TUBE | BEZEL | BG | DEVICE | DECAL" 0 0 5 1
float HSM_CAB_GLASS_FOLLOW_LAYER = global.HSM_CAB_GLASS_FOLLOW_LAYER;

#pragma parameter HSM_CAB_GLASS_FOLLOW_MODE                 "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_CAB_GLASS_FOLLOW_MODE = global.HSM_CAB_GLASS_FOLLOW_MODE;

#pragma parameter HSM_CAB_GLASS_FOLLOW_FULL_USES_ZOOM       "          Follow Full also follows Zoom" 1 0 1 1
float HSM_CAB_GLASS_FOLLOW_FULL_USES_ZOOM = global.HSM_CAB_GLASS_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_CAB_GLASS_FILL_MODE                   "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_CAB_GLASS_FILL_MODE = global.HSM_CAB_GLASS_FILL_MODE;

#pragma parameter HSM_CAB_GLASS_SPLIT_PRESERVE_CENTER       "          Split Mode Preserve Center %"  0 0 500 1
float HSM_CAB_GLASS_SPLIT_PRESERVE_CENTER = global.HSM_CAB_GLASS_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_CAB_GLASS_SPLIT_REPEAT_WIDTH          "          Split Mode Repeat Width %"  0 0 100 1
float HSM_CAB_GLASS_SPLIT_REPEAT_WIDTH = global.HSM_CAB_GLASS_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_CAB_GLASS_SCALE                       "          Scale" 100 10 500 0.1
float HSM_CAB_GLASS_SCALE = global.HSM_CAB_GLASS_SCALE / 100;

#pragma parameter HSM_CAB_GLASS_SCALE_X                     "          Scale X" 100 0 500 0.1
float HSM_CAB_GLASS_SCALE_X = global.HSM_CAB_GLASS_SCALE_X / 100;

#pragma parameter HSM_CAB_GLASS_POS_X                       "          Position X"  0 -1000 1000 0.05
float HSM_CAB_GLASS_POS_X = global.HSM_CAB_GLASS_POS_X / -100;

#pragma parameter HSM_CAB_GLASS_POS_Y                       "          Position Y"  0 -1000 1000 0.05
float HSM_CAB_GLASS_POS_Y = global.HSM_CAB_GLASS_POS_Y / 100;


//---------------------------------------------------------------------------------------------------
//    TOP IMAGE
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_TOP_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_TOP_TITLE			                "[ TOP EXTRA LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_TOP_OPACITY                       "          Opacity"  100 0 100 0.1
float HSM_TOP_OPACITY = global.HSM_TOP_OPACITY / 100;

#pragma parameter HSM_TOP_COLORIZE_ON                   "          Colorize" 0 0 1 1
float HSM_TOP_COLORIZE_ON = global.HSM_TOP_COLORIZE_ON;

#pragma parameter HSM_TOP_HUE                           "          Hue Offset" 0 0 360 1
float HSM_TOP_HUE = global.HSM_TOP_HUE / 360;

#pragma parameter HSM_TOP_SATURATION                    "          Saturation" 100 0 200 1
float HSM_TOP_SATURATION = global.HSM_TOP_SATURATION / 100;

#pragma parameter HSM_TOP_BRIGHTNESS                    "          Brightness" 100 0 200 1
float HSM_TOP_BRIGHTNESS = global.HSM_TOP_BRIGHTNESS / 100;

#pragma parameter HSM_TOP_GAMMA                         "          Gamma Adjust" 1 0 5 0.01
float HSM_TOP_GAMMA = global.HSM_TOP_GAMMA;

#pragma parameter HSM_TOP_BLEND_MODE                    "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 0 3 1
float HSM_TOP_BLEND_MODE = global.HSM_TOP_BLEND_MODE;

#pragma parameter HSM_TOP_SOURCE_MATTE_TYPE             "          Source Matte Color - BLACK | WHITE | NONE"  2 0 2 1
float HSM_TOP_SOURCE_MATTE_TYPE = global.HSM_TOP_SOURCE_MATTE_TYPE;

#pragma parameter HSM_TOP_ADD_MODE_NIGHTLIGHTING_AMOUNT   "          Add Mode Night Lighting Amount"  0 0 100 1
float HSM_TOP_ADD_MODE_NIGHTLIGHTING_AMOUNT = global.HSM_TOP_ADD_MODE_NIGHTLIGHTING_AMOUNT / 100.0;

#pragma parameter HSM_TOP_MIPMAPPING_BLEND_BIAS         "          Mipmapping Blend Bias (Affects Sharpness)" 0 -5 5 0.1
float HSM_TOP_MIPMAPPING_BLEND_BIAS = global.HSM_TOP_MIPMAPPING_BLEND_BIAS;

#pragma parameter HSM_TOP_MASK_MODE                     "    Mask - ALL | SCRN | TUBE | BZ- | BZ | BZ+ | FRM | FRM+ | BG"   0 0 8 1
float HSM_TOP_MASK_MODE = global.HSM_TOP_MASK_MODE;

#pragma parameter HSM_TOP_CUTOUT_MODE                   "          Cutout Mask - OFF | ON | INVERT" 0 0 2 1
float HSM_TOP_CUTOUT_MODE = global.HSM_TOP_CUTOUT_MODE;

#pragma parameter HSM_TOP_DUALSCREEN_VIS_MODE           "          Dual Screen Visibility - BOTH | SCREEN 1 | SCREEN 2" 0 0 2 1
float HSM_TOP_DUALSCREEN_VIS_MODE = global.HSM_TOP_DUALSCREEN_VIS_MODE;

//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_TOP2_EMPTY_LINE							" " 0 0 0.001 0.001
#pragma parameter HSM_TOP_TITLE2			           "[ TOP SCALE & FOLLOW ]:" 0 0 0.01 0.01

#pragma parameter HSM_TOP_FOLLOW_LAYER                  "      Follow Layer - FULL | TUBE | BEZEL | BG | DEVICE | DECAL" 0 0 5 1
float HSM_TOP_FOLLOW_LAYER = global.HSM_TOP_FOLLOW_LAYER;

#pragma parameter HSM_TOP_FOLLOW_MODE                   "          Follow Mode - FOLLOW SCALE & POS | FOLLOW EXACTLY" 0 0 1 1
float HSM_TOP_FOLLOW_MODE = global.HSM_TOP_FOLLOW_MODE;

#pragma parameter HSM_TOP_FOLLOW_FULL_USES_ZOOM          "          Follow Full also follows Zoom" 1 0 1 1
float HSM_TOP_FOLLOW_FULL_USES_ZOOM = global.HSM_TOP_FOLLOW_FULL_USES_ZOOM;

#pragma parameter HSM_TOP_FILL_MODE                     "          Fill Mode - KEEP TEXURE ASPECT | SPLIT | STRETCH"  0 0 2 1
float HSM_TOP_FILL_MODE = global.HSM_TOP_FILL_MODE;

#pragma parameter HSM_TOP_SPLIT_PRESERVE_CENTER       "          Split Mode Preserve Center %"  0 0 500 1
float HSM_TOP_SPLIT_PRESERVE_CENTER = global.HSM_TOP_SPLIT_PRESERVE_CENTER / 1000;

#pragma parameter HSM_TOP_SPLIT_REPEAT_WIDTH            "          Split Mode Repeat Width %"  0 0 100 1
float HSM_TOP_SPLIT_REPEAT_WIDTH = global.HSM_TOP_SPLIT_REPEAT_WIDTH / 1000;

#pragma parameter HSM_TOP_SCALE                         "          Scale" 100 0 1000 0.1
float HSM_TOP_SCALE = global.HSM_TOP_SCALE / 100;

#pragma parameter HSM_TOP_SCALE_X                       "          Scale X" 100 0 1000 0.1
float HSM_TOP_SCALE_X = global.HSM_TOP_SCALE_X / 100;

#pragma parameter HSM_TOP_POS_X                         "          Position X"  0 -1000 1000 0.05
float HSM_TOP_POS_X = global.HSM_TOP_POS_X / -100;

#pragma parameter HSM_TOP_POS_Y                         "          Position Y"  0 -1000 1000 0.05
float HSM_TOP_POS_Y = global.HSM_TOP_POS_Y / 100;

#pragma parameter HSM_TOP_MIRROR_WRAP                   "          Mirror Wrap" 1 0 1 1
float HSM_TOP_MIRROR_WRAP = global.HSM_TOP_MIRROR_WRAP;
