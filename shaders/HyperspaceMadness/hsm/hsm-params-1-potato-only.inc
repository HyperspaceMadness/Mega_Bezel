/*   
hsm-params-potato

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL
Description:
	-Parameters to be used by Mega Screen Scale and Mega Bezel
*/

//---------------------------------------------------------------------------------------------------
//    BASIC BACKGROUND IMAGE LAYER
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_BASIC_BG_IMG_TITLE	                    "[ BASIC BACKGROUND IMAGE LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_OPACITY 	                    "          Opacity" 100 0 100 0.1
float HSM_BG_OPACITY = global.HSM_BG_OPACITY / 100;

#pragma parameter HSM_BG_BLEND_MODE               "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 -1 3 1
float HSM_BG_BLEND_MODE = global.HSM_BG_BLEND_MODE;

#pragma parameter HSM_BG_BRIGHTNESS                "          Brightness" 100 0 200 1
float HSM_BG_BRIGHTNESS = global.HSM_BG_BRIGHTNESS / 100;


#pragma parameter HSM_STANDARD_BG_SHOW_OVER_SCREEN 	"          Show over Screen" 0 0 1 1
float HSM_STANDARD_BG_SHOW_OVER_SCREEN = global.HSM_STANDARD_BG_SHOW_OVER_SCREEN;


#pragma parameter HSM_BG_TEXTURE_ASPECT_MODE      "         Texture Aspect- FULL | EXPL. | 4:3 | 3:4 | 16:9 | 9:16" 4 0 5 1
float HSM_BG_TEXTURE_ASPECT_MODE = global.HSM_BG_TEXTURE_ASPECT_MODE;

#pragma parameter HSM_BG_EXPLICIT_TEXTURE_ASPECT  "          Explicit Texture Aspect" 1.33333 0.01 3 0.01
float HSM_BG_EXPLICIT_TEXTURE_ASPECT = global.HSM_BG_EXPLICIT_TEXTURE_ASPECT;


