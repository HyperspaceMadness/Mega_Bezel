#version 450

#include "common/globals-and-screen-scale-params.inc"
#include "common/helper-functions.inc"
#include "common/base-functions.inc"
#include "common/params-5-intro.inc"

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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Import so all the mega bezel parameters are the first in the parameter list

#pragma name IntroPass

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
layout(set = 0, binding = 1) uniform sampler2D CacheInfoPass;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D IntroImage;

#define eps 1e-3

// -----------------------------------------------------------------------------
// TV Startup Static Animation
// -----------------------------------------------------------------------------
vec4 GetColorWithIntro(vec4 in_color, vec2 in_coord, in sampler2D in_logo_image_sampler2d)
{
   vec4 out_color = in_color;

   // If we are out of the region of time the intro plays just give back the same color
   if (CURRENT_FRAME_FROM_CACHE_INFO > (HSM_INTRO_SOLID_COLOR_HOLD + 
                                       HSM_INTRO_SOLID_COLOR_FADE_OUT + 
                                       HSM_INTRO_LOGO_WAIT + 
                                       HSM_INTRO_LOGO_FADE_IN +
                                       HSM_INTRO_LOGO_HOLD +
                                       HSM_INTRO_LOGO_FADE_OUT +
                                       HSM_INTRO_NOISE_HOLD +
                                       HSM_INTRO_NOISE_FADE_OUT + 
                                       HSM_INTRO_SOLID_BLACK_HOLD +
                                       HSM_INTRO_SOLID_BLACK_FADE_OUT) / HSM_INTRO_SPEED)
      return out_color;

   in_coord.y = HSM_FLIP_VIEWPORT_VERTICAL * HSM_FLIP_CORE_VERTICAL * in_coord.y;
   in_coord.x = HSM_FLIP_VIEWPORT_HORIZONTAL * HSM_FLIP_CORE_HORIZONTAL * in_coord.x;

   if (abs(HSM_ROTATE_CORE_IMAGE) == 1)
   {
      in_coord.xy = in_coord.yx;

      if (HSM_ROTATE_CORE_IMAGE == 1)
         in_coord.y = (in_coord.y - 0.5) * -1 + 0.5;
      else if (HSM_ROTATE_CORE_IMAGE == -1)
         in_coord.x = (in_coord.x - 0.5) * -1 + 0.5;
   }

   float frame_count = HSM_INTRO_WHEN_TO_SHOW == 2 ? CURRENT_FRAME_FROM_CACHE_INFO : global.FrameCount;

   float mask_wait = 20 / HSM_INTRO_SPEED;
   
	float mask_fade_horizontal = 40 / HSM_INTRO_SPEED;
	float mask_fade_vertical = 60 / HSM_INTRO_SPEED;
	float mask_vert_wait = mask_fade_horizontal * 0.4;

   vec4 solid_color = vec4(HSM_HSVtoRGB(vec3(HSM_INTRO_SOLID_COLOR_HUE, HSM_INTRO_SOLID_COLOR_SAT, HSM_INTRO_SOLID_COLOR_VALUE)), 1);
   // vec4 solid_color = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), DEFAULT_SRGB_GAMMA).rgb;

	float solid_color_hold = HSM_INTRO_SOLID_COLOR_HOLD / HSM_INTRO_SPEED;
	float solid_color_fade_out = HSM_INTRO_SOLID_COLOR_FADE_OUT / HSM_INTRO_SPEED;

   float intro_logo_wait = HSM_INTRO_LOGO_WAIT / HSM_INTRO_SPEED;
   float intro_logo_fade_in = HSM_INTRO_LOGO_FADE_IN / HSM_INTRO_SPEED;
   float intro_logo_hold = HSM_INTRO_LOGO_HOLD / HSM_INTRO_SPEED;
   float intro_logo_fade_out = HSM_INTRO_LOGO_FADE_OUT / HSM_INTRO_SPEED;

	float noise_hold = HSM_INTRO_NOISE_HOLD / HSM_INTRO_SPEED;
	float noise_fade_out = HSM_INTRO_NOISE_FADE_OUT / HSM_INTRO_SPEED;

   // Black Hold starts from end of Static Hold
   float black_hold = HSM_INTRO_SOLID_BLACK_HOLD / HSM_INTRO_SPEED;
	float black_fade_out = HSM_INTRO_SOLID_BLACK_FADE_OUT / HSM_INTRO_SPEED;

   float power_on_seq_length = mask_wait + mask_vert_wait + mask_fade_vertical;
   float fade_out_seq_length = max(noise_fade_out, black_hold + black_fade_out);
   float logo_seq_length     = intro_logo_wait + intro_logo_fade_in + intro_logo_hold + intro_logo_fade_out;
   float full_seq_length     = max(power_on_seq_length + noise_hold + fade_out_seq_length, logo_seq_length);

   // Loop the animation so we can see the results
   if (HSM_INTRO_WHEN_TO_SHOW == 3)
      frame_count = mod(frame_count, full_seq_length + 15);

	if (frame_count <= full_seq_length)
	{
      vec2 noise_res = global.SourceSize.xy;
      noise_res = min(noise_res, vec2(640, 480));

      if (abs(HSM_ROTATE_CORE_IMAGE) > 0.5)
         noise_res.xy = noise_res.yx;

		vec2 quantized_coord = vec2(floor((in_coord.x + 0.01) * noise_res.x) / noise_res.x, floor((in_coord.y + 0.01) * noise_res.y) / noise_res.y);

		float width = (frame_count - mask_wait) / (mask_fade_horizontal/2);
		float height = (clamp((frame_count - mask_wait - mask_vert_wait) / mask_fade_vertical, 0, 1) + 0.005) * 2;
		float mask = abs(in_coord.x - 0.5) > width * 0.5 ? 0 : 1;
		mask *= HHLP_EasePowerOut(abs(in_coord.y - 0.5), 1.5) > height ? 0 : 1;

		float area = width * height;

      float black_opacity = HHLP_EasePowerIn(clamp(1 - (frame_count - (power_on_seq_length + noise_hold + black_hold)) / black_fade_out, 0, 1), 1.5);
      out_color = mix(out_color, vec4(0, 0, 0, 1), black_opacity);

		float solid_color_opacity = HHLP_EasePowerIn(clamp(1 - (frame_count - (mask_wait + mask_fade_horizontal/2.5 + solid_color_hold)) / solid_color_fade_out * 2, 0, 1), 1.5);
      // out_color = mix(out_color, vec4(0, 0, 0, 1), solid_color_opacity);

		float noise_opacity =  1 - clamp((frame_count - (power_on_seq_length + noise_hold)) / noise_fade_out, 0, 1);
		noise_opacity = HHLP_EasePowerInOut(noise_opacity, 4);

		// Calculate Static Noise
		float e = 2.7182818284590452353602874713527;
		float G = e + (mod(frame_count, 30)/30 + 1000);
		vec2 r = (G * sin(G * quantized_coord.xy));
		vec4 noise_color = vec4(fract(r.x * r.y * (1.0 + quantized_coord.x)));
		noise_color.a = 1;

		noise_color.rgb *= 0.60;

		out_color = HSM_BlendModeLayerMix(out_color, noise_color, HSM_INTRO_NOISE_BLEND_MODE, noise_opacity);

      vec2 intro_image_res = textureSize(in_logo_image_sampler2d, 0);
      float intro_image_aspect = intro_image_res.x/intro_image_res.y;

      float intro_logo_height = HSM_INTRO_LOGO_HEIGHT;
      if (intro_logo_height == 0)
         intro_logo_height = abs(HSM_ROTATE_CORE_IMAGE) > 0.5 ? intro_image_res.y / global.OriginalSize.y
                                                         : intro_image_res.y / global.OriginalSize.x;

      vec2 intro_image_scale = vec2(intro_logo_height * intro_image_aspect / SCREEN_ASPECT, intro_logo_height) * min(SCREEN_ASPECT, 1);
      intro_image_scale = floor(intro_image_scale * global.SourceSize.xy) / global.SourceSize.xy;

      if (HSM_DUALSCREEN_MODE > 0.5)
      {
         if (HSM_GetCoreImageSplitDirection() == 1)
         {
            in_coord.y = in_coord.y * 2 - MAX_NEGATIVE_CROP;
         }
         if (HSM_GetCoreImageSplitDirection() == 2)
         {
            in_coord.x = in_coord.x * 2 - MAX_NEGATIVE_CROP;
         }
      }

      in_coord.x -= HSM_INTRO_LOGO_POS_X;
      in_coord.y += HSM_INTRO_LOGO_POS_Y;

      // Center
      vec2 coord_in_intro_image = (in_coord - 0.5) / intro_image_scale + 0.5;
      // Top Left
      if (HSM_INTRO_LOGO_PLACEMENT == 1)
         coord_in_intro_image = in_coord / intro_image_scale;
      // Top Right
      if (HSM_INTRO_LOGO_PLACEMENT == 2)
         coord_in_intro_image = (in_coord - vec2(1 - intro_image_scale.x, 0)) / intro_image_scale;
      // Bottom Left
      if (HSM_INTRO_LOGO_PLACEMENT == 3)
         coord_in_intro_image = (in_coord - vec2(0, 1 - intro_image_scale.y)) / intro_image_scale;
      // Bottom Right
      if (HSM_INTRO_LOGO_PLACEMENT == 4)
         coord_in_intro_image = (in_coord - (1 - intro_image_scale)) / intro_image_scale;

      // Quilez gives a better rescaling weighted more to the center of the pixel like bicubic
      vec4 intro_image = HSM_TextureQuilez(IntroImage, intro_image_res, coord_in_intro_image);
      float logo_opacity = 1 - HHLP_EasePowerOut(clamp(1 - (frame_count - mask_wait - mask_fade_horizontal/4) / intro_logo_fade_in, 0, 1), 1.5);
      logo_opacity *=  clamp(clamp((frame_count - intro_logo_wait) / intro_logo_fade_in, 0, 1) - 
                              clamp((frame_count - (intro_logo_wait + intro_logo_fade_in + intro_logo_hold)) / intro_logo_fade_out, 0, 1), 
                              0, 1);

      // Logo UNDER solid color
      if (HSM_INTRO_LOGO_OVER_SOLID_COLOR == 0)
         out_color = HSM_BlendModeLayerMix(out_color, intro_image, HSM_INTRO_LOGO_BLEND_MODE, logo_opacity);

		out_color = HSM_BlendModeLayerMix(out_color, solid_color, HSM_INTRO_SOLID_COLOR_BLEND_MODE, solid_color_opacity);

      // Logo OVER solid color
      if (HSM_INTRO_LOGO_OVER_SOLID_COLOR == 1)
         out_color = HSM_BlendModeLayerMix(out_color, intro_image, HSM_INTRO_LOGO_BLEND_MODE, logo_opacity);

		out_color *= mask;

      HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
	}
   return out_color;
}

void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);

	// Flip the coordinate vertically if desired
   vec2 viewport_coord_adjusted = vTexCoord;
	viewport_coord_adjusted.x = HSM_FLIP_VIEWPORT_HORIZONTAL * HSM_FLIP_CORE_HORIZONTAL * (viewport_coord_adjusted.x - 0.5) + 0.5;
	viewport_coord_adjusted.y = HSM_FLIP_VIEWPORT_VERTICAL * HSM_FLIP_CORE_VERTICAL * (viewport_coord_adjusted.y - 0.5) + 0.5;

   FragColor = texture(Source, vTexCoord);

   if (HSM_INTRO_WHEN_TO_SHOW > 0)
      FragColor = GetColorWithIntro(FragColor, viewport_coord_adjusted, IntroImage);
}

