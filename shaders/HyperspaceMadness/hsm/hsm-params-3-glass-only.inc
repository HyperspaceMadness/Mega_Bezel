/*   
hsm-params-basic

Author: HyperspaceMadness - remimcgill@hotmail.com
License: GPL
Description:
	-Parameters to be used by Mega Screen Scale and Mega Bezel
*/

//---------------------------------------------------------------------------------------------------
//    STANDARD BACKGROUND IMAGE LAYER
//---------------------------------------------------------------------------------------------------
#pragma parameter HSM_GLASS_IMG_TITLE	            "[ GLASS IMAGE LAYER ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_IMG_TITLE	            "[ BACKGROUND IMAGE ]:" 0 0 0.01 0.01

#pragma parameter HSM_BG_OPACITY                  "          Opacity" 100 0 100 0.1
float HSM_BG_OPACITY = global.HSM_BG_OPACITY / 100;

#pragma parameter HSM_BG_BLEND_MODE               "          Blend Mode - OFF | NORMAL | ADD | MULTIPLY"  1 -1 3 1
float HSM_BG_BLEND_MODE = global.HSM_BG_BLEND_MODE;
