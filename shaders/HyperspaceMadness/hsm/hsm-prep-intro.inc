// Import so all the mega bezel parameters are the first in the parameter list

#include "hsm-text.inc"

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 VIEWPORT_COORD;

void main()
{
   gl_Position = global.MVP * Position;
   VIEWPORT_COORD = TexCoord * 1.00001;
}

#pragma stage fragment
layout(location = 0) in vec2 VIEWPORT_COORD;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D DerezedPass;

#ifndef IS_POTATO_PRESET
layout(set = 0, binding = 3) uniform sampler2D OriginalHistory1;
layout(set = 0, binding = 4) uniform sampler2D OriginalHistory2;
layout(set = 0, binding = 5) uniform sampler2D OriginalHistory3;
layout(set = 0, binding = 7) uniform sampler2D IntroImage;
#endif

layout(set = 0, binding = 8) uniform sampler2D InfoCachePass;

#define eps 1e-3

vec4 AddText(vec2 in_coord, vec4 in_color)
{
   in_coord.y = abs(HSM_FLIP_VIEWPORT_VERTICAL - HSM_FLIP_CORE_VERTICAL) > 0.5 ? 1 - in_coord.y : in_coord.y;
   in_coord.x = abs(HSM_FLIP_VIEWPORT_HORIZONTAL - HSM_FLIP_CORE_HORIZONTAL) > 0.5 ? 1 - in_coord.x : in_coord.x;

   float screen_aspect = SCREEN_ASPECT;
   vec2 screen_scale = SCREEN_SCALE;
   vec2 screen_size = HSM_GetScreenSize(screen_scale);
   vec2 int_scale_mult = HSM_GetIntegerScaleMult(screen_size, SCREEN_INDEX);
   vec2 original_size = HSM_GetRotatedScreenDerezedSize(SCREEN_INDEX);
   vec2 cropped_original_size = HSM_GetCroppedRotatedDerezedSize(SCREEN_INDEX);
   vec2 cropped_original_size_with_core_res_mult = CROPPED_ROTATED_ORIGINAL_SIZE_WITH_CORE_RES_MULT;
   
   vec4 out_color = in_color;
   out_color = mix(out_color, vec4(0, 0, 0, 1), GetTextMask(in_coord + 2 * vec2(1/global.OutputSize.x, 0), global.FinalViewportSize.xy, screen_size, original_size, cropped_original_size, cropped_original_size_with_core_res_mult, int_scale_mult, global.OutputSize.xy));
   out_color = mix(out_color, vec4(0, 0, 0, 1), GetTextMask(in_coord - 2 * vec2(1/global.OutputSize.x, 0), global.FinalViewportSize.xy, screen_size, original_size, cropped_original_size, cropped_original_size_with_core_res_mult, int_scale_mult, global.OutputSize.xy));
   out_color = mix(out_color, vec4(0, 0, 0, 1), GetTextMask(in_coord + 1 * vec2(0, 1/global.OutputSize.y), global.FinalViewportSize.xy, screen_size, original_size, cropped_original_size, cropped_original_size_with_core_res_mult, int_scale_mult, global.OutputSize.xy));
   out_color = mix(out_color, vec4(0, 0, 0, 1), GetTextMask(in_coord - 1 * vec2(0, 1/global.OutputSize.y), global.FinalViewportSize.xy, screen_size, original_size, cropped_original_size, cropped_original_size_with_core_res_mult, int_scale_mult, global.OutputSize.xy));
   out_color = mix(out_color, vec4(1, 1, 0, 1), GetTextMask(in_coord,                                      global.FinalViewportSize.xy, screen_size, original_size, cropped_original_size, cropped_original_size_with_core_res_mult, int_scale_mult, global.OutputSize.xy));
   return clamp(out_color, 0, 1);
}

#ifndef IS_POTATO_PRESET
vec4 AntiFlicker(vec2 in_coord, vec4 current)
{
	// sample the textures
	vec4 prev1 = texture(OriginalHistory1, in_coord);
	vec4 prev2 = texture(OriginalHistory2, in_coord);
	vec4 prev3 = texture(OriginalHistory3, in_coord);
	
	// get luma for comparison
	float cur_lum = dot(current.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev1_lum = dot(prev1.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev2_lum = dot(prev2.rgb, vec3(0.2125, 0.7154, 0.0721));
	float prev3_lum = dot(prev3.rgb, vec3(0.2125, 0.7154, 0.0721));
	
	// Test whether the luma difference between the pixel in the current frame and that of
	// the previous frame exceeds the threshold while the difference between the current frame
	// and 2 frames previous is below the threshold.
	// Repeat the process for the previous frame's pixel to reduce false-positives
	bool flicker = (abs(cur_lum - prev1_lum) > HSM_ANTI_FLICKER_THRESHOLD && abs(cur_lum - prev2_lum) < HSM_ANTI_FLICKER_THRESHOLD) &&
		(abs(prev1_lum - prev2_lum) > HSM_ANTI_FLICKER_THRESHOLD && abs(prev1_lum - prev3_lum) < HSM_ANTI_FLICKER_THRESHOLD);
	
	// Average the current frame with the previous frame in linear color space to avoid over-darkening
	vec4 blended = (pow(current, vec4(2.2)) + pow(prev1, vec4(2.2))) / 2.0;
	
	// delinearize the averaged result
	blended = pow(blended, vec4(1.0 / 2.2));
	
	return (!flicker) ? current : blended;
}

// -----------------------------------------------------------------------------
// TV Startup Static Animation
// -----------------------------------------------------------------------------
vec4 GetColorWithIntro(vec4 in_color, vec2 in_coord)
{
   vec4 out_color = in_color;

   // in_coord.y = HSM_INTRO_LOGO_FLIP_VERTICAL > 0.5 ? 1 - in_coord.y : in_coord.y;

   in_coord.y = abs(HSM_FLIP_VIEWPORT_VERTICAL - HSM_FLIP_CORE_VERTICAL) > 0.5 ? 1 - in_coord.y : in_coord.y;
   in_coord.x = abs(HSM_FLIP_VIEWPORT_HORIZONTAL - HSM_FLIP_CORE_HORIZONTAL) > 0.5 ? 1 - in_coord.x : in_coord.x;

   float frame_count = global.FrameCount;

   float mask_wait = 20 / HSM_INTRO_SPEED;
   
	float mask_fade_horizontal = 40 / HSM_INTRO_SPEED;
	float mask_fade_vertical = 60 / HSM_INTRO_SPEED;
	float mask_vert_wait = mask_fade_horizontal * 0.4;

   vec4 solid_color = vec4(HSM_HSVtoRGB(vec3(HSM_INTRO_SOLID_COLOR_HUE, HSM_INTRO_SOLID_COLOR_SAT, HSM_INTRO_SOLID_COLOR_VALUE)), 1);
   // vec4 solid_color = HSM_Linearize(vec4(HSM_HSVtoRGB(vec3(HSM_BZL_COLOR_HUE, HSM_BZL_COLOR_SATURATION, HSM_BZL_COLOR_VALUE)), 1), HSM_DEFAULT_SRGB_GAMMA).rgb;

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
   if (HSM_INTRO_WHEN_TO_SHOW == 2)
      frame_count = mod(frame_count, full_seq_length + 15);

	if (frame_count <= full_seq_length)
	{
		vec2 noise_res = global.OriginalSize.xy;
      noise_res = min(noise_res, vec2(640, 480));
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

      vec2 intro_image_res = vec2(HSM_INTRO_LOGO_RES_X, HSM_INTRO_LOGO_RES_Y);
      float intro_image_aspect = intro_image_res.x/intro_image_res.y;
      // vec2 intro_image_scale = intro_image_res / global.OriginalSize.xy / SCREEN_ASPECT;

      float intro_logo_height = HSM_INTRO_LOGO_HEIGHT;
      if (intro_logo_height == 0)
         intro_logo_height = intro_image_res.y / global.OriginalSize.y;

      vec2 intro_image_scale = vec2(intro_logo_height * intro_image_aspect / SCREEN_ASPECT, intro_logo_height);

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

      HSM_Delinearize(out_color, HSM_DEFAULT_SRGB_GAMMA);
	}
   return out_color;
}
#endif

void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(InfoCachePass, VIEWPORT_COORD);
   
	// Flip the coordinate vertically if desired
   vec2 viewport_coord_adjusted = VIEWPORT_COORD;
	viewport_coord_adjusted.x = abs(HSM_FLIP_VIEWPORT_HORIZONTAL - HSM_FLIP_CORE_HORIZONTAL) * (1 - viewport_coord_adjusted.x) + (1 - abs(HSM_FLIP_VIEWPORT_HORIZONTAL - HSM_FLIP_CORE_HORIZONTAL)) * viewport_coord_adjusted.x;
	viewport_coord_adjusted.y = abs(HSM_FLIP_VIEWPORT_VERTICAL - HSM_FLIP_CORE_VERTICAL) * (1 - viewport_coord_adjusted.y) + (1 - abs(HSM_FLIP_VIEWPORT_VERTICAL - HSM_FLIP_CORE_VERTICAL)) * viewport_coord_adjusted.y;

   FragColor = texture(DerezedPass, viewport_coord_adjusted);

#ifndef IS_POTATO_PRESET
   if (HSM_ANTI_FLICKER_ON == 1)
      FragColor = AntiFlicker(viewport_coord_adjusted, FragColor);

   // HSM_Linearize(FragColor, HSM_DEFAULT_SRGB_GAMMA);

   if (HSM_INTRO_WHEN_TO_SHOW > 0)
      FragColor = GetColorWithIntro(FragColor, viewport_coord_adjusted);
#endif

   // Apply black level & gamma with input as non linearized
   // FragColor = HSM_ApplyBlackLevel(FragColor, 0);

	if (HSM_LIVE_UPDATE_ON == 1 )
	{
		float final_aspect = SCREEN_ASPECT;
		vec2 corner_offset = vec2(0.08, 0.1);
		vec2 center_coord = vec2(1 - corner_offset.x * final_aspect, 1 - corner_offset.y);
		vec2 test_coord = vec2(1 - (1 - viewport_coord_adjusted.x) * final_aspect, viewport_coord_adjusted.y);
		float distance =  length(test_coord - center_coord);

		if (distance < 0.05)
		{
			float modulation = clamp(0.4 + abs(mod(global.FrameCount, 90) / 90 - 0.5) * 1.2, 0, 1);
			// modulation = HHLP_EasePowerInOut(modulation, 2);
			// float scanline_mask = 1 - abs(scan - 0.5) * 2;
			FragColor *= 1 - modulation;
			FragColor += modulation * vec4(1, 0, 0, 1);
		}
	}
   
   //////// Draw text to show out_colorolutions ////////// 
   if (HSM_RESOLUTION_DEBUG_ON == 1)
      FragColor = AddText(viewport_coord_adjusted, FragColor);

   // FragColor = HSM_Delinearize(FragColor, HSM_DEFAULT_SRGB_GAMMA);
}