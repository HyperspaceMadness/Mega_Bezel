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

#pragma name FinalCompositePass

/////////////// IMPORTS ///////////////
#include "common/globals-and-image-layers-params.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

#define RENDER_SIMPLE_MODE_NONE 0
#define RENDER_SIMPLE_MODE_BLACK_SCREEN 1
#define RENDER_SIMPLE_MODE_BLACK_SCREEN_AND_BG 2
#define RENDER_SIMPLE_MODE_PINK_MASK 3
#define RENDER_SIMPLE_MODE_BLACK_WHITE_MASK 4

#define RENDER_SIMPLE_MASK_TYPE_SCREEN 0
#define RENDER_SIMPLE_MASK_TYPE_TUBE 1
#define RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME 2
#define RENDER_SIMPLE_MASK_TYPE_BEZEL 3
#define RENDER_SIMPLE_MASK_TYPE_FRAME 4


// const float PI = 3.14159265359;
// #define LIGHT_RADIANCE 300.0
// #define LIGHT_COLOR vec3(1.0, 1.0, 1.0)

// #define ALBEDO vec3(1.0, 1.0, 1.0)
// #define METALNESS 0.2
// #define ROUGHNESS 0.2

// #define AO_SAMPLES 8.0
// #define AO_ATTENUATION 1.0
// #define AO_RADIUS 0.0001
// #define AO_POWER 4.0

vec2 FLIPPED_VIEWPORT_COORD = vec2(0.5);

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage vertex

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 6) out vec2 vTexCoord;

//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	gl_Position = global.MVP * Position;
	vTexCoord = TexCoord;

	// Not sure why we need linearize this but it seems to have a smoother range this way
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#pragma stage fragment

layout(location = 6) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

// Pass Framebuffer Textures
layout(set = 0, binding = 1) uniform sampler2D CacheInfoPass;

layout(set = 0, binding = 2) uniform sampler2D Original;
layout(set = 0, binding = 3) uniform sampler2D FetchDrezPass;
layout(set = 0, binding = 4) uniform sampler2D DeditherPass;
// layout(set = 0, binding = 5) uniform sampler2D ColorCorrectPass;
// layout(set = 0, binding = 6) uniform sampler2D LinearizePass;
layout(set = 0, binding = 7) uniform sampler2D CrtPass;
layout(set = 0, binding = 8) uniform sampler2D NightLighting;
layout(set = 0, binding = 9) uniform sampler2D NightLighting2;

#ifdef IS_NO_REFLECT_PRESET
#define MiddlePass CrtPass
#else
layout(set = 0, binding = 10) uniform sampler2D ReflectPass;
#define MiddlePass ReflectPass
#endif

layout(set = 0, binding = 11) uniform sampler2D SceneLayersPass;
layout(set = 0, binding = 12) uniform sampler2D BounceLightPass;
layout(set = 0, binding = 13) uniform sampler2D ReflectPrepFullscreenGlowPass;

layout(set = 0, binding = 14) uniform sampler2D NormalsAndHeightPass;
#define heightMapChannel NormalsAndHeightPass

layout(set = 0, binding = 15) uniform sampler2D FinalCompositePassFeedback;
#define PassFeedback FinalCompositePassFeedback


layout(set = 0, binding = 5) uniform sampler2D DitherNoise;


vec3 PrepGraphicsForHDR(vec3 in_color)
{
	vec3 out_color = in_color.rgb;
	
	out_color *= HSM_GLOBAL_GRAPHICS_BRIGHTNESS;
	
	#ifdef IS_HDR_PRESET
	if((HCRT_HDR < 1.0f) && (HCRT_COLOUR_ACCURATE < 1.0f))
		if(HCRT_OUTPUT_COLOUR_SPACE == 2.0f)
			out_color= (out_color * k709_to_XYZ) * kXYZ_to_DCIP3; 

	if(HCRT_HDR > 0.5f)
		out_color = InverseTonemap(out_color, HCRT_MAX_NITS, HCRT_PAPER_WHITE_NITS);

	if((HCRT_HDR >= 1.0f) && (HCRT_COLOUR_ACCURATE < 1.0f))
		out_color  *= k2020Gamuts[uint(HCRT_EXPAND_GAMUT)] * (HCRT_PAPER_WHITE_NITS / kMaxNitsFor2084);
	#endif

	return out_color;
}

float GetHeightValue(in sampler2D in_heightmap, vec2 coords)
{
	return clamp(texture(in_heightmap, coords).a, 0.0, 1.0);
}

float Random(vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

#define pow2(x) (x * x)

const float pi = atan(1.0) * 4.0;
const int samples = 10; // Was 35 originally
const float sigma = float(samples) * 0.25;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec3 blur(sampler2D sp, vec2 uv, vec2 scale) {
    vec3 col = vec3(0.0);
    float accum = 0.0;
    float weight;
    vec2 offset;
    
    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset);
            col += texture(sp, uv + scale * offset).rgb * weight;
            accum += weight;
        }
    }
    
    return col / accum;
}

float GetBlurredMiddleShadow(sampler2D sp, vec2 uv, float scale_multiplier, float noise_amount) {
    float blurred_alpha = 0.0;
    float accum = 0.0;
    float weight;
    vec2 offset;
    
	vec2 scale = scale_multiplier * 1 / global.OutputSize.xy;

	float max_blur_size = noise_amount;
	float p = max_blur_size / global.SourceSize.y;
	// srand
	float radius = sin(dot(uv, vec2(1233.224, 1743.335)));
	
	// The following are all unrolled otherwise they won't compile in D3D11
	uv += vec2(HSM_rand(radius), HSM_rand(radius)) * p;

    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset);

			vec4 precomp_background = vec4(0);
			vec4 precomp_middleground = vec4(0);
			vec4 precomp_foreground = vec4(0);
			vec4 precomp_additive = vec4(0);

			vec4 image_layers_packed = texture(sp, uv + scale * offset);
			HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
										precomp_foreground, 
										precomp_middleground, 
										precomp_background, 
										precomp_additive);

            blurred_alpha += precomp_middleground.a * weight;
            accum += weight;
        }
    }
    
    return 1 - blurred_alpha / accum;
}


// Shadertoy by consoleartist
// https://www.shadertoy.com/view/Xlsfzl#
vec3 GetHeightShadow(in sampler2D in_heightmap, 
						float valid_height_area_mask,
						vec2 uv,
						vec3 light_point3, 
						float directional_amount,
						float in_samples,
						float in_sample_bias,
						float in_noise,
						float in_height_noise_amount,
						float shadow_dist_umbra,
						float shadow_dist_penumbra,
						float shadow_falloff_spread,
						float shadow_gamma)
{
	light_point3.y *= -1;
	vec3 light_vector_directional = normalize(light_point3);
	vec3 light_vector = normalize(vec3(light_point3.xy, light_point3.z) - vec3(uv * 2.0 - 1.0, GetHeightValue(in_heightmap, uv)));
	light_vector = normalize(mix(light_vector, light_vector_directional, directional_amount));

	vec3 light_vector_orig = light_vector;
	vec3 light_vector_solid_orig = normalize(vec3(light_vector.x, light_vector.y, light_vector.z + 0.05));

	vec2 random_xy = vec2(0); //global.OutputSize.zw * in_noise / 4;
	vec3 world_pos = vec3(uv + random_xy, GetHeightValue(in_heightmap, uv + random_xy));

	// float random_sample_offset = (Random(world_pos.xy) * in_height_noise_amount) / in_samples;

	// Adjust Noise by the number of samples
	float random_sample_offset_orig = in_noise * in_height_noise_amount / in_samples;

	float height_range = 1.1 - world_pos.z;
	float sample_dist = 1 / in_samples;

	float falloff_shadow = 1.0;

	vec3 light_vector_falloff = vec3(0);

	float dist_to_occlusion = 1000000;

	// Make sure falloff start is before the end
	float in_height_noise_falloff_start = 0.5;
	float in_height_noise_falloff_end = 1.0;
	in_height_noise_falloff_start = min(in_height_noise_falloff_start, in_height_noise_falloff_end - 0.0001);

	vec3 ray_sample_pos_random;
	for (float i = 1; i <= in_samples; i += 1)
	{
		float position_in_noise_falloff = smoothstep(in_height_noise_falloff_start, in_height_noise_falloff_end, world_pos.z);
		float random_sample_offset = mix(random_sample_offset_orig, 0.0, position_in_noise_falloff);

		light_vector_falloff = light_vector_orig;
		light_vector_falloff.xy += shadow_falloff_spread * (Random( world_pos.xy + i * vec2(0.0005)) - 0.5) * i * sample_dist * vec2(0.1) ;
		light_vector_falloff = normalize(light_vector_falloff);
		ray_sample_pos_random = world_pos + light_vector_falloff * (i * sample_dist + random_sample_offset);
		float height_at_sample = GetHeightValue(in_heightmap, vec2(ray_sample_pos_random.x, ray_sample_pos_random.y));

		if (dist_to_occlusion > 100000 && ray_sample_pos_random.z < (height_at_sample - in_sample_bias))
			dist_to_occlusion = length(ray_sample_pos_random - world_pos);

		if (ray_sample_pos_random.z > 1.0 + 2 * sample_dist)
			break;
	}

	falloff_shadow = dist_to_occlusion > 999999 ? 1 : clamp(smoothstep(shadow_dist_umbra, shadow_dist_penumbra, dist_to_occlusion), 0, 1);

	float final_shadow = falloff_shadow;

	return vec3(final_shadow);
}

float EnvBRDFApproxNonmetal( float Roughness, float NdotV )
{
	// Same as EnvBRDFApprox( 0.04, Roughness, NoV )
	const vec2 c0 = { -1, -0.0275 };
	const vec2 c1 = { 1, 0.0425 };
	vec2 r = Roughness * c0 + c1;
	return min( r.x * r.x, exp2( -9.28 * NdotV ) ) * r.x + r.y;
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(vec3 specularColor, float roughness, float ndotv)
{
	const vec4 c0 = vec4(-1, -0.0275, -0.572, 0.022);
	const vec4 c1 = vec4(1, 0.0425, 1.04, -0.04);
	vec4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * ndotv)) * r.x + r.y;
	vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
	return specularColor * AB.x + AB.y;
}

vec3 EnvRemap(vec3 c)
{
	return pow(2. * c, vec3(2.2));
}

// https://www.shadertoy.com/view/7djBDh kaylent
float AmbientOcclusion(in sampler2D normal_height_map, vec2 in_coord){
    const int numRotateStep = 10;
    const int numDistanceStep = 10;
    const float maxDistance = 0.015;
    const float expStrength = 3.0;

    float rotateStep = 2.0 * 3.14 / float(numRotateStep);
    float distanceStep = float(maxDistance) / float(numDistanceStep);
    float multiplier = 1.0 ;
    float dMAO = 1.0 / (float(numRotateStep) * float(numDistanceStep));
    float fragmentHeight = texture(normal_height_map, in_coord).a;

    for (int i = 0; i < numRotateStep; ++i){
        for(int j = 0; j < numDistanceStep; ++j){
            vec2 pRandVec = vec2(0);
            pRandVec.x = cos(rotateStep * float(i));
            pRandVec.y = sin(rotateStep * float(i));
            pRandVec *= float(j) * distanceStep;

			float sample_height = texture(normal_height_map, in_coord + pRandVec).a;
			// float sample_height = HHLP_GetAveragedTextureSample(normal_height_map, in_coord + pRandVec, global.OutputSize).a;
			float height_difference = sample_height - fragmentHeight;
            if (texture(normal_height_map, in_coord + pRandVec).a > fragmentHeight)
			{
                multiplier -= dMAO * height_difference * 6;
            }
        }
    }
    return smoothstep(0.1, 0.8, pow(multiplier, expStrength));
}

float luminance(vec3 col) {
    return dot(col, vec3(0.299, 0.587, 0.114));
}

// vec3 blinn_phong(vec3 normal, vec3 , in Sphere sphere, in Light light, float t, float delta) {
//     fragment.color = vec3(0.15);
    
//     if(t > 0.0) {
//         float ambient = clamp(0.5 + 0.5 * contact.normal.y, 0.0, 1.0);
//         float diffuse = clamp(dot(light.direction, contact.normal), 0.0, 1.0);
//         vec3 half_way = normalize(-eye.direction + light.direction);
//         float specular = pow(clamp(dot(half_way, contact.normal), 0.0, 1.0), sphere.material.high_light);
        
//         fragment.color = ambient * sphere.material.ambient * light.occlusion;
//         fragment.color += diffuse * sphere.material.diffuse * light.occlusion;
//         fragment.color += diffuse * specular * sphere.material.specular * light.occlusion;
// #if TEXTURING
//         fragment.color *= spherical_texturing(contact.normal, iChannel0, delta * 0.05);
// #endif
//     }

vec3 ApplyAmbient( vec3 albedo_color, vec3 natural_ambient_color)
{
	return albedo_color * natural_ambient_color;
}

vec3 ApplySpotlight(vec2 uv, 
						vec4 normal_and_height_color,
						vec3 light_point, 
						float light_omni_amount,
						float global_scale_offset, 
						float spot_width,
						float spot_falloff,
						vec3 light_color, 
						float ao,
						vec3 albedo_color,
						float roughness, 
						float bounce_specular_amount, 
						float diffuse_ao_amount, 
						float shadow)
{
	float output_aspect = global.OutputSize.x / global.OutputSize.y;

	// Get world normal from tangent normal map
    vec3 normal = normal_and_height_color.rgb * 2 - 1;
 	light_point *= global_scale_offset;
	spot_width *= global_scale_offset;

	// light_point is in -1 to 1 space, move to UV 0 to 1 aspect corrected space
 	vec2 spot_uv = light_point.xy;
	spot_uv.x /= output_aspect;
	spot_uv.y *= -1;
	spot_uv = (spot_uv + 1) / 2;

	vec3 planePoint = vec3(vTexCoord * 2 - 1, normal_and_height_color.a);
	planePoint.y *= 1 - planePoint.y;

	vec3 view_vector = normalize(vec3(0, 0, -3.5) - (1 - planePoint));
	vec3 rd = -view_vector;

	vec3 light_dir_pointlight = normalize(light_point - planePoint);
	vec3 light_dir_directional = normalize(light_point);
	vec3 light_dir = mix(light_dir_pointlight, light_dir_directional, light_omni_amount);

	// Light color/shadow for diffuse and specular
	float light_distance = length(view_vector);
	light_color *= shadow * 1 / (light_distance * light_distance * light_distance);
	// light_color *= HHLP_EasePowerIn(smoothstep(spot_width, 0, length((uv - spot_uv) * vec2(output_aspect, 1))), spot_falloff);
	light_color *= HHLP_EasePowerIn(smoothstep(spot_width, 0, length((uv - spot_uv) * vec2(output_aspect, 1))), spot_falloff);

	// Start with the color as black
	vec3 out_color = vec3(0);

	// Add Diffuse
	vec3 half_vector = normalize(light_dir + rd);
	float ao_diffuse = mix(1.0, ao, diffuse_ao_amount);
	out_color += albedo_color * ao_diffuse * light_color * dot(half_vector, normal);

	// Specular
	// Get a reflected vector and position in the uv of the reflected light texture
	vec3 speCol = light_color;

	// Get a fresnel-esque multiplier based on normal angles
	// TODO should we try to use EnvRemap on EnvBRDFApproxNonmetal
	// float env_spec_mult = pow(EnvBRDFApproxNonmetal( roughness, dot(normal_and_height_color.rgb, vec3(0, 0, 1)) ) * 12, 3);
	float light_brightness = 1;
	float specular_incidence_mult = pow(clamp(dot(half_vector, normal), 0.0, 1.0), light_brightness);

	// AO so we do not reflect from inside deep grooves
	float ao_specular = bounce_specular_amount * ao;

	// Add the specular
	out_color += speCol * ao_specular * bounce_specular_amount * specular_incidence_mult; // * env_spec_mult;

	return out_color;
}



vec3 ApplyLighting(vec2 uv, 
					vec4 normal_and_height_color,
					vec3 light_point, 
					float light_omni_amount, 
					vec3 light_color, 
					float ao,
					vec3 albedo_color,
					float roughness, 
					float bounce_specular_amount,
					in sampler2D specular_color_map,
					float diffuse_ao_amount, 
					float shadow)
{
	// Get world normal from tangent normal map
    vec3 normal = normal_and_height_color.rgb * 2 - 1;

	vec3 planePoint = vec3(vTexCoord * 2 - 1, normal_and_height_color.a);
	planePoint.y *= -1;

	vec3 view_vector = normalize(vec3(0, 0, -3.5) - (1 - planePoint));
	vec3 rd = -view_vector;

	vec3 light_dir_pointlight = normalize(light_point - planePoint);
	vec3 light_dir_directional = normalize(light_point);
	vec3 light_dir = mix(light_dir_pointlight, light_dir_directional, light_omni_amount);

	// Start with the color as black
	vec3 out_color = vec3(0);

	// Light color/shadow for diffuse and specular
	light_color *= shadow;

	// Add Diffuse
	vec3 half_vector = normalize(light_dir + rd);
	float ao_diffuse = mix(1.0, ao, diffuse_ao_amount);
	out_color += albedo_color * ao_diffuse * light_color * dot(half_vector, normal);

	// Specular
	// Get a reflected vector and position in the uv of the reflected light texture
	vec3 refl = reflect(vec3(0, 0, 1), normal);
	vec2 refl_uv = vec2(dot(refl, vec3(-1, 0, 0)) / 2 + 0.5, 1 - (dot(refl, vec3(0, -1, 0)) / 2 + 0.5));
	vec3 speCol = texture(specular_color_map, refl_uv).rgb;

	// Get a fresnel-esque multiplier based on normal angles
	// TODO should we try to use EnvRemap on EnvBRDFApproxNonmetal
	float env_spec_mult = pow(EnvBRDFApproxNonmetal( roughness, dot(normal_and_height_color.rgb, vec3(0, 0, 1)) ) * 12, 3);

	// Add the specular but multiply by AO so we do not reflect from inside deep grooves
	out_color += speCol * ao * bounce_specular_amount * env_spec_mult;

	return out_color;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord);
	vec2 VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord);

	if (HSM_SHOW_PASS_INDEX > 0.5)
	{
		FragColor = vec4(0);

		bool pass_needs_delinearize = true;
		vec2 coord_for_raw_passes = HSM_SHOW_PASS_APPLY_SCREEN_COORD > 0.5 ? HSM_GetCRTShaderCurvedCoord(SCREEN_COORD) : vTexCoord;

		// TODO Can we make this simpler without the #ifdef?
		#ifndef IS_NO_REFLECT_PRESET
		// REFLECTION
		if (HSM_SHOW_PASS_INDEX == 1)
		{
			FragColor = texture(ReflectPass, vTexCoord);
			pass_needs_delinearize = HSM_NON_USER_DELINEARIZE_REFLECT_PASS < 0.5;
		}
		#endif

		// TUBE
		if (HSM_SHOW_PASS_INDEX == 2)
			FragColor = texture(CrtPass, vTexCoord);

		// CRT
		if (HSM_SHOW_PASS_INDEX == 3)
			FragColor = texture(CrtPass, vTexCoord);

		// // INTERLACE & LINEARIZE
		// if (HSM_SHOW_PASS_INDEX == 4)
		// {
		// 	FragColor = texture(LinearizePass, (floor(coord_for_raw_passes * textureSize(LinearizePass, 0)) + 0.5) / textureSize(LinearizePass, 0));
		// 	pass_needs_delinearize = HSM_NON_USER_LINEARIZE_INTERLACE_PASS > 0.5;
		// }

		// // COLOR CORRECT & UPSCALE
		// if (HSM_SHOW_PASS_INDEX == 5)
		// {
		// 	FragColor = texture(ColorCorrectPass, (floor(coord_for_raw_passes * textureSize(ColorCorrectPass, 0)) + 0.5) / textureSize(ColorCorrectPass, 0));
		// 	pass_needs_delinearize = false;
		// }

		// DEDITHER
		if (HSM_SHOW_PASS_INDEX == 6)
		{
			FragColor = texture(DeditherPass, (floor(coord_for_raw_passes * textureSize(DeditherPass, 0)) + 0.5) / textureSize(DeditherPass, 0));
			pass_needs_delinearize = false;
		}

		// DREZ
		if (HSM_SHOW_PASS_INDEX == 7)
		{
			FragColor = texture(FetchDrezPass, coord_for_raw_passes);
			pass_needs_delinearize = false;
		}

		// CORE
		if (HSM_SHOW_PASS_INDEX == 8)
		{
			FragColor = texture(Original, coord_for_raw_passes);
			pass_needs_delinearize = false;
		}

		// IMAGE LAYERS
		if (HSM_SHOW_PASS_INDEX > 8)
		{
			vec4 image_layers_packed = texture(SceneLayersPass, vTexCoord);

			vec4 precomp_background = vec4(0);
			vec4 precomp_middleground = vec4(0);
			vec4 precomp_foreground = vec4(0);
			vec4 precomp_additive = vec4(0);

			HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
								precomp_foreground, 
								precomp_middleground, 
								precomp_background, 
								precomp_additive);

			if (HSM_SHOW_PASS_INDEX == 9)
				FragColor = precomp_additive;
			if (HSM_SHOW_PASS_INDEX == 10)
				FragColor = precomp_foreground;
			if (HSM_SHOW_PASS_INDEX == 11)
				FragColor = precomp_middleground;
			if (HSM_SHOW_PASS_INDEX == 12)
				FragColor = precomp_background;

			pass_needs_delinearize = false;
		}

		// Linearize passes which are not already in linear space
		if (pass_needs_delinearize)
			FragColor = HSM_Delinearize(FragColor, DEFAULT_SRGB_GAMMA);

		if (HSM_SHOW_PASS_ALPHA > 0.5)
			FragColor = vec4(FragColor.a);

		return;
	}

	// // Seems to make it slower
	// if (HSM_IsOutsideReflectionBoundary())
	// {
	// 	vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	// 	if (HSM_CACHE_GRAPHICS_ON > 0.5 && feedback_color_test.a < 0 && !CACHE_INFO_CHANGED && HSM_RENDER_SIMPLE_MODE < 0.5)
	// 	{
	// 		FragColor = texture(PassFeedback, vTexCoord);
	// 		return;
	// 	}
	// }

	HSM_GetBezelCoords(TUBE_DIFFUSE_COORD, 
						TUBE_DIFFUSE_SCALE, 
						TUBE_SCALE, 
						TUBE_DIFFUSE_ASPECT,
						HSM_RENDER_SIMPLE_MODE > 0.5,
						BEZEL_OUTSIDE_SCALE,
						BEZEL_OUTSIDE_COORD, 
						BEZEL_OUTSIDE_CURVED_COORD, 
						FRAME_OUTSIDE_CURVED_COORD);

	
	vec4 image_layers_packed = texture(SceneLayersPass, vTexCoord, 0);

	vec4 precomp_background = vec4(0);
	vec4 precomp_middleground = vec4(0);
	vec4 precomp_foreground = vec4(0);
	vec4 precomp_additive = vec4(0);

	HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed, 
						precomp_foreground, 
						precomp_middleground, 
						precomp_background, 
						precomp_additive);

	precomp_foreground = HSM_Linearize(precomp_foreground, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_foreground.a = pow(precomp_foreground.a, DEFAULT_SRGB_GAMMA);

	precomp_middleground = HSM_Linearize(precomp_middleground, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_middleground.a = pow(precomp_middleground.a, DEFAULT_SRGB_GAMMA);
	
	precomp_background = HSM_Linearize(precomp_background, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_background.a = pow(precomp_background.a, DEFAULT_SRGB_GAMMA);

	float static_shadows = precomp_background.a;
	precomp_background.a = 1.0;

	precomp_additive = HSM_Linearize(precomp_additive, DEFAULT_SRGB_GAMMA);
	// Gamma added to alpha to avoid clipping on AMD GPUs
	precomp_additive.a = pow(precomp_additive.a, DEFAULT_SRGB_GAMMA);
	float reflection_mask = precomp_additive.a;

	// Debug Material Display Modes
	if (HSM_LGHT_DEBUG_MATERIAL > 0.5 || HSM_LGHT_DEBUG_SHADING_MODE > 4.5)
	{
		vec3 material_color = vec3(0);

		// Gray Material
		if (HSM_LGHT_DEBUG_MATERIAL == 1)
			material_color = vec3(0.2);
			
		// White Material
		if (HSM_LGHT_DEBUG_MATERIAL == 2)
			material_color = vec3(1.0);

		// Black Material (To show only the specular addition)
		if (HSM_LGHT_DEBUG_SHADING_MODE == 5)
			material_color = vec3(0.0);
		
		precomp_foreground.rgb = material_color * precomp_foreground.a;
		precomp_middleground.rgb = material_color * precomp_middleground.a;
		precomp_background.rgb = material_color;
		precomp_additive.rgb = vec3(0.0);
	}

	// Adjust Albedo Brightness of all non-additive layers
	precomp_foreground.rgb *= HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS;
	precomp_middleground.rgb *= HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS;
	precomp_background.rgb *= HSM_LGHT_BOUNCE_ALBEDO_BRIGHTNESS;\

	vec4 out_color = vec4(0);


	vec4 image_layers_packed_shadow_stretch = texture(SceneLayersPass, (vTexCoord - 0.5) / 1.1 + 0.5 + vec2(SCREEN_SCALE.x * 0.02, 0));
	vec4 precomp_background_shadow_stretch = vec4(0);
	vec4 precomp_middleground_shadow_stretch = vec4(0);
	vec4 precomp_foreground_shadow_stretch = vec4(0);
	vec4 precomp_additive_shadow_stretch = vec4(0);

	HSM_Unpack_1_Rgba_To_4_Rgba(image_layers_packed_shadow_stretch, 
								precomp_foreground_shadow_stretch, 
								precomp_middleground_shadow_stretch, 
								precomp_background_shadow_stretch, 
								precomp_additive_shadow_stretch);

	// Needs to divide by 2
	float mix_horz = HSM_LGHT_BOUNCE_MIX_HORZ / 2;
	float mix_vert = HSM_LGHT_BOUNCE_MIX_VERT / 2;

	vec2 bounce_coord = vTexCoord;
	// vec2 bounce_coord = HSM_Anamorphic_Distort(bounce_coord, HSM_LGHT_BOUNCE_ANAMORPH);
	vec2 bounce_coord_ctr = (bounce_coord - 0.5) * SCREEN_SCALE / HSM_LGHT_BOUNCE_SCALE;

	vec4 bounce_light = texture(BounceLightPass, bounce_coord_ctr + 0.5);
	vec4 bounce_light_center = texture(ReflectPrepFullscreenGlowPass, bounce_coord_ctr * 0.5 + 0.5);
	vec4 bounce_light_mirrored_x = texture(BounceLightPass, vec2(-1 * bounce_coord_ctr.x, bounce_coord_ctr.y) + 0.5);
	vec4 bounce_light_mirrored_y = texture(BounceLightPass, vec2(bounce_coord_ctr.x, -1 * bounce_coord_ctr.y) + 0.5);
	bounce_light = (1 - ( mix_horz + mix_vert)) * bounce_light + 
					mix_horz * bounce_light_mirrored_x + 
					mix_vert * bounce_light_mirrored_y;

	float bounce_center_Vignette = clamp(HSM_GetStrengthPowerVignetteFactor(bounce_coord_ctr * 0.5 + 0.5, 20, 2), 0, 1);
	bounce_light = mix(bounce_light, bounce_light_center, HSM_LGHT_BOUNCE_MIX_FULLSCREEN_GLOW * bounce_center_Vignette);

	float noise_sample = HSM_TextureAbsolute(DitherNoise, vTexCoord * 1).r; // * 1.9;
	float shadow_bias = 0.001;

	// Middle shadow should probably be soft
	float middle_umbra = 0.0;
	float middle_penumbra = 0.3;
	float middle_directional = 0.0;

	// Only look at the height in the masked area when doing the shadow calculation
	float valid_height_area_mask = 1 - precomp_middleground.a;

	float bounce_shadow_from_middle = clamp(GetHeightShadow(NormalsAndHeightPass, 
															valid_height_area_mask,
															vTexCoord, 
															vec3(0.0, 0.0, 5.0),
															middle_directional,
															HSM_HSHADOW_SAMPLES, 
															shadow_bias, 
															noise_sample, 
															HSM_HSHADOW_NOISE, 
															middle_umbra,
															middle_penumbra,
															HSM_HSHADOW_SPREAD_NOISE,
															HSM_HSHADOW_GAMMA), 0, 1).r;
	vec3 shadow_light_point = HSM_LGHT_LIGHT_POINT;
	shadow_light_point.z *= HSM_HSHADOW_DEPTH_MULT;

	// float bounce_shadow_from_top_right = 1 - precomp_middleground_shadow_stretch.a;
	float bounce_shadow_from_top_right = clamp(GetHeightShadow(NormalsAndHeightPass, 
												valid_height_area_mask,
												vTexCoord, 
												shadow_light_point,
												HSM_LGHT_OMNI,
												HSM_HSHADOW_SAMPLES, 
												shadow_bias, 
												noise_sample, 
												HSM_HSHADOW_NOISE, 
												HSM_HSHADOW_DIST_UMBRA,
												HSM_HSHADOW_DIST_PENUMBRA,
												HSM_HSHADOW_SPREAD_NOISE,
												HSM_HSHADOW_GAMMA), 0, 1).r;

	float left_umbra = 0.0;
	float left_penumbra = 0.5;
	float bounce_shadow_from_top_left = clamp(GetHeightShadow(NormalsAndHeightPass, 
															valid_height_area_mask,
															vTexCoord, 
															vec3(shadow_light_point.x * -0.75, shadow_light_point.y * 0.85, shadow_light_point.z),
															HSM_LGHT_OMNI,
															HSM_HSHADOW_SAMPLES, 
															shadow_bias, 
															noise_sample, 
															HSM_HSHADOW_NOISE, 
															left_umbra,
															left_penumbra,
															HSM_HSHADOW_SPREAD_NOISE,
															HSM_HSHADOW_GAMMA), 0, 1).r;

	vec2 bshadow_offset = vec2(-0.07, 0.07) / (global.OutputSize.x / global.OutputSize.y);
	float bshadow_opacity = 0.8;
	float bshadow_noise = 4;
	float bshadow_blur_scale_mult = 8;
	float middleground_bshadow = clamp(GetBlurredMiddleShadow(SceneLayersPass, vTexCoord - bshadow_offset, bshadow_blur_scale_mult, bshadow_noise) + precomp_middleground.a, 0, 1);
	middleground_bshadow = mix(1, middleground_bshadow, HSM_LGHT_SHADOW_PROJECTED_AMOUNT);

	// FragColor = vec4(middleground_bshadow);
	// return;

	// Do HSV alterations on the bounce_light
	if (HSM_LGHT_BOUNCE_SATURATION != 1 || HSM_LGHT_BOUNCE_BRIGHTNESS != 1)
	{
		vec3 bounce_light_hsv = HSM_RGBtoHSV(bounce_light.rgb);
		bounce_light_hsv.y *= HSM_LGHT_BOUNCE_SATURATION;
		bounce_light_hsv.z *= HSM_LGHT_BOUNCE_BRIGHTNESS;
		bounce_light.rgb = HSM_HSVtoRGB(bounce_light_hsv);
	}

	// Add some noise to avoid banding
	bounce_light.rgb += HSM_DitheredNoise(vTexCoord, global.OutputSize.xy, 1);

	// if (HSM_LGHT_DEBUG_SHADING_MODE > 0.5)
	// {
	// 	FragColor = pow(bounce_light, vec4(1 / DEFAULT_SRGB_GAMMA));
	// 	return;
	// }

	// Prep the Nightlighting images
	vec3 nightlight1_rgb = vec3(1);
	vec3 nightlight2_rgb = vec3(1);
	HSM_Fill_Nightlight_s(VIEWPORT_COORD, HSM_GetViewportCoordWithFlip(vTexCoord), TUBE_DIFFUSE_COORD, TUBE_DIFFUSE_SCALE, HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE, NightLighting, NightLighting2, nightlight1_rgb, nightlight2_rgb);

	vec4 composite_base_color = vec4(0);
	if (HSM_COMPOSITE_BASE_COLOR > 0.5)
	{
		// Red BG
		if (HSM_COMPOSITE_BASE_COLOR == 1)
			composite_base_color = vec4(1.0, 0.0, 0.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Pink (Magenta) BG
		if (HSM_COMPOSITE_BASE_COLOR == 2)
			composite_base_color = vec4(1.0, 0.0, 1.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Yellow BG
		if (HSM_COMPOSITE_BASE_COLOR == 3)
			composite_base_color = vec4(1.0, 1.0, 0.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
		// Aqua BG
		if (HSM_COMPOSITE_BASE_COLOR == 4)
			composite_base_color = vec4(0.0, 1.0, 1.0, 0.0) * HSM_COMPOSITE_BASE_COLOR_BRIGHTNESS;
	}

	out_color = composite_base_color;

	if (HSM_COMPOSITE_BACKGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_background_orig = precomp_background;

		// // Adjust brightness and add static shadows
		// precomp_background.rgb *= HSM_COMPOSITE_BACKGROUND_STATIC_AMBIENT_MULTIPLIER;
		// precomp_background.rgb *= (1 - clamp(HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH * (1 - static_shadows), 0, 1));
		// // precomp_background.rgb *= 1 - mix(vec3(0), vec3(static_shadows), HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH);

		// // Apply Night Lighting s
		// precomp_background.rgb = ApplyNightlights(	precomp_background.rgb,
		// 												nightlight1_rgb,
		// 												nightlight2_rgb,
		// 												HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT1_MULTIPLIER,
		// 												HSM_COMPOSITE_BACKGROUND_NIGHTLIGHT2_MULTIPLIER,
		// 												1, 
		// 												BLEND_MODE_NORMAL,
		// 												HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);
		
		// precomp_background.rgb *= 1 - clamp(HSM_COMPOSITE_BACKGROUND_STATIC_SHADOW_STRENGTH * (1 - static_shadows), 0, 1);

		// Add bounce light
		// height_shadows.r;
		// precomp_background.rgb += precomp_background_orig.rgb * HSM_COMPOSITE_BACKGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * mix(bounce_shadow_from_top_right, 1, 1 - HSM_LGHT_BOUNCE_SHADOW_STRENGTH);

		// Prep Color
		// precomp_background.rgb = PrepGraphicsForHDR(precomp_background.rgb);

		// The background is always 100% opaque so we can just replace the out_color
		out_color = precomp_background;
	}

	// Add additional shadow depth by darkening bouncelight shadow areas

	vec4 crt_layer = vec4(0);
	if (HSM_COMPOSITE_CRT_AND_REFLECTION_ON > 0.5)
	{
		crt_layer = texture(MiddlePass, vTexCoord);
	}

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		crt_layer.rgb = vec3(0);
	}

	float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
	if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
		bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

	// // TODO, need to see how to manage with parameters
	// #ifndef IS_NO_REFLECT_PRESET
	vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);

	TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);
	// // Apply Reflection  Mask
	crt_layer *= TUBE_MASK + (1 - TUBE_MASK) * reflection_mask;
	// #endif

	if (HSM_COMPOSITE_MIDDLEGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_middleground_orig = precomp_middleground;

		// // Adjust brightness and add static shadows
		// precomp_middleground.rgb *= HSM_COMPOSITE_MIDDLEGROUND_STATIC_AMBIENT_MULTIPLIER * static_shadows;
		
		// // Apply Night Lighting s
		// precomp_middleground.rgb = ApplyNightlights(	precomp_middleground.rgb,
		// 												nightlight1_rgb,
		// 												nightlight2_rgb,
		// 												HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT1_MULTIPLIER,
		// 												HSM_COMPOSITE_MIDDLEGROUND_NIGHTLIGHT2_MULTIPLIER,
		// 												1, 
		// 												BLEND_MODE_NORMAL,
		// 												HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);
		// // Add Bounce Light
		// // precomp_middleground.rgb += precomp_middleground_orig.rgb * HSM_COMPOSITE_MIDDLEGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * precomp_middleground_orig.a;

		// // Prep Color
		// precomp_middleground.rgb = PrepGraphicsForHDR(precomp_middleground.rgb);

		// Composite
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_middleground);
	}


	// // Need to consolidate per group lighting & surface lighting, or add it separately on each

	float ao = smoothstep(0, HSM_LGHT_AO_HEIGHT_CUTOFF, texture(NormalsAndHeightPass, vTexCoord).a);
	// FragColor = vec4(ao);
	// return;
	vec4 normal_and_height_color = texture(NormalsAndHeightPass, vTexCoord);
	
	float spot_specular_amount = HSM_LGHT_DEBUG_SHADING_MODE > 0.5 && HSM_LGHT_DEBUG_SHADING_MODE < 3.5 ? 0 : HSM_LGHT_SPECULAR_AMOUNT;
	float bounce_specular_amount = HSM_LGHT_DEBUG_SHADING_MODE > 0.5 && HSM_LGHT_DEBUG_SHADING_MODE < 3.5 ? 0 : HSM_LGHT_SPECULAR_AMOUNT * HSM_LGHT_BOUNCE_BRIGHTNESS;

	vec3 natural_ambient_color = HSM_HSVtoRGB(vec3(HSM_LGHT_NATURAL_AMBIENT_HUE, HSM_LGHT_NATURAL_AMBIENT_SATURATION, HSM_LGHT_NATURAL_AMBIENT_VALUE));
	natural_ambient_color = HSM_LGHT_DEBUG_SHADING_MODE == 2 ? vec3(0) : natural_ambient_color;

	// float luma_pos_right = luminance(texture(BounceLightPass, vec2(0.3, 0.5)).rgb);
	// float luma_pos_right_top = luminance(texture(BounceLightPass, vec2(0.85, 0.15)).rgb);
	// float luma_pos_right_bottom = luminance(texture(BounceLightPass, vec2(0.85, 0.85)).rgb);

	// float luma_pos_left_top = luminance(texture(BounceLightPass, vec2(0.15, 0.15)).rgb);
	// float luma_pos_left_bottom = luminance(texture(BounceLightPass, vec2(0.15, 0.85)).rgb);
	// float luma_pos_left = luminance(texture(BounceLightPass, vec2(0.7, 0.5)).rgb);

	// float luma_ratio_right = luma_pos_right / (luma_pos_right + luma_pos_left);
	// // Push values toward top and bottom of range
	// luma_ratio_right = HHLP_EasePowerInOut(luma_ratio_right, 1.0);
	// float luma_ratio_left = 1 - luma_ratio_right;

	// // Measure how close both items are to 0.5, when both are close to 0.5 this maps to 1
	// // as both shadows become close to 0.5 then we want more of the middle shadow and less of the side shadows
	// float middle_weight = smoothstep(0.5, 0, abs(luma_ratio_right - 0.5) + abs(luma_ratio_right - 0.5));

	bounce_shadow_from_middle = mix(1.0, pow(bounce_shadow_from_middle, 1 / HSM_HSHADOW_GAMMA), HSM_LGHT_SHADOW_FROM_MIDDLE_AMOUNT);// * middle_weight)
	bounce_shadow_from_top_right = mix(1.0, pow(bounce_shadow_from_top_right, 1 / HSM_HSHADOW_GAMMA), HSM_LGHT_SHADOW_FROM_RIGHT_AMOUNT);// * luma_ratio_left * (1 - middle_weight))
	bounce_shadow_from_top_left = mix(1.0, pow(bounce_shadow_from_top_left, 1 / HSM_HSHADOW_GAMMA), HSM_LGHT_SHADOW_FROM_LEFT_AMOUNT);// * luma_ratio_right * (1 - middle_weight));

	vec3 light_point_right = HSM_LGHT_LIGHT_POINT;
	vec3 light_point_left = vec3(HSM_LGHT_LIGHT_POINT.x * -1.0, HSM_LGHT_LIGHT_POINT.y * 0.85, HSM_LGHT_LIGHT_POINT.z);
	vec3 light_point_middle = vec3(0.0, 0, 5.0);

	vec3 ambient_color_black = vec3(0);

	vec3 prelit_color = out_color.rgb;
	bounce_light.rgb = HSM_LGHT_DEBUG_SHADING_MODE == 1 ? vec3(0) : bounce_light.rgb;

	out_color.rgb = vec3(0);

	// Add Natural Ambient Light often a bluish light
	out_color.rgb += ApplyAmbient(prelit_color, natural_ambient_color);

	vec3 spot_shared_color = pow(vec3(0.92, 0.84, 0.63), vec3(2.2)) * HSM_LGHT_SPOT_BRIGHTNESS;
	float omnidirectional = 1;

	// Add Spot Ambient Light spot light is usually incandescent
	out_color.rgb += ApplyAmbient(prelit_color, spot_shared_color * HSM_LGHT_SPOT_AMBIENT * HSM_LGHT_SPOT_BRIGHTNESS * middleground_bshadow * bounce_shadow_from_top_right);


	vec3 spot1_point = vec3(HSM_LGHT_SPOT1_POINTX, HSM_LGHT_SPOT1_POINTY, 1);

	out_color.rgb += ApplySpotlight(vTexCoord, 
									normal_and_height_color,
									spot1_point,
									omnidirectional,
									HSM_LGHT_SPOT_GLOBAL_SCALE,
									HSM_LGHT_SPOT_WIDTH,
									HSM_LGHT_SPOT_FALLOFF,
									spot_shared_color,
									ao,
									prelit_color,
									HSM_LGHT_ROUGHNESS,
									spot_specular_amount,
									HSM_LGHT_DIFFUSE_AO,
									middleground_bshadow * bounce_shadow_from_top_right);

	vec3 spot2_point = vec3(HSM_LGHT_SPOT2_POINTX, HSM_LGHT_SPOT2_POINTY, 1);

	out_color.rgb += ApplySpotlight(vTexCoord, 
									normal_and_height_color,
									spot2_point,
									omnidirectional,
									HSM_LGHT_SPOT_GLOBAL_SCALE,
									HSM_LGHT_SPOT_WIDTH,
									HSM_LGHT_SPOT_FALLOFF,
									spot_shared_color,
									ao,
									prelit_color,
									HSM_LGHT_ROUGHNESS,
									spot_specular_amount,
									HSM_LGHT_DIFFUSE_AO,
									middleground_bshadow * bounce_shadow_from_top_right);

	// Blend back to the prelit color depending on how much "nightlighting" 
	out_color.rgb = mix(prelit_color, out_color.rgb, HSM_LIGHT_NIGHTLIGHTING_OPACITY);

	// Bounce Lighting
	out_color.rgb += 0.5 * ApplyLighting(vTexCoord, 
								normal_and_height_color,
								light_point_middle,
								HSM_LGHT_OMNI,
								bounce_light.rgb,
								ao,
								prelit_color,
								HSM_LGHT_ROUGHNESS,
								bounce_specular_amount,
								BounceLightPass,
								HSM_LGHT_DIFFUSE_AO,
								bounce_shadow_from_middle * middleground_bshadow);

	out_color.rgb += 0.25 * ApplyLighting(vTexCoord, 
								normal_and_height_color,
								light_point_right,
								HSM_LGHT_OMNI,
								bounce_light.rgb,
								ao,
								prelit_color,
								HSM_LGHT_ROUGHNESS,
								bounce_specular_amount,
								BounceLightPass,
								HSM_LGHT_DIFFUSE_AO,
								bounce_shadow_from_top_right * middleground_bshadow);

	out_color.rgb += 0.25 * ApplyLighting(vTexCoord, 
								normal_and_height_color,
								light_point_left,
								HSM_LGHT_OMNI,
								bounce_light.rgb,
								ao,
								prelit_color,
								HSM_LGHT_ROUGHNESS,
								bounce_specular_amount,
								BounceLightPass,
								HSM_LGHT_DIFFUSE_AO,
								bounce_shadow_from_top_left * middleground_bshadow);



	if (HSM_COMPOSITE_CRT_AND_REFLECTION_ON > 0.5 && HSM_RENDER_SIMPLE_MODE < 0.5)
		out_color = HSM_PreMultAlphaBlend(out_color, crt_layer);

	if (HSM_COMPOSITE_FOREGROUND_ON > 0.5)
	{
		// Make a copy before shadows are added or brightness is adjusted
		vec4 precomp_foreground_orig = precomp_foreground;

		// // Adjust brightness and add static shadows
		// precomp_foreground.rgb *= HSM_COMPOSITE_FOREGROUND_STATIC_AMBIENT_MULTIPLIER * static_shadows;
		
		// // Apply Night Lighting s
		// precomp_foreground.rgb = ApplyNightlights(	precomp_foreground.rgb,
		// 												nightlight1_rgb,
		// 												nightlight2_rgb,
		// 												HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT1_MULTIPLIER,
		// 												HSM_COMPOSITE_FOREGROUND_NIGHTLIGHT2_MULTIPLIER,
		// 												1, 
		// 												BLEND_MODE_NORMAL,
		// 												HSM_LIGHT_NIGHTLIGHTING_SWAP_IMAGE_MODE);

		// // Add Bounce Light
		// // precomp_foreground.rgb += precomp_foreground_orig.rgb * HSM_COMPOSITE_FOREGROUND_BOUNCELIGHT_MULTIPLIER * bounce_light.rgb * precomp_foreground_orig.a;

		// // Prep Color
		// precomp_foreground.rgb = PrepGraphicsForHDR(precomp_foreground.rgb);

		// Composite
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_foreground);
	}

	if (HSM_COMPOSITE_ADDITIVE_ON > 0.5)
	{
		precomp_additive.rgb *= HSM_COMPOSITE_ADDITIVE_BRIGHTNESS;
		precomp_additive.rgb = PrepGraphicsForHDR(precomp_additive.rgb);
		out_color = HSM_PreMultAlphaBlend(out_color, precomp_additive * vec4(1, 1, 1, 0));
	}

	if (HSM_RENDER_SIMPLE_MODE > 0.5)
	{
		float bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * HSM_GLOBAL_CORNER_RADIUS;
		if(HSM_BZL_USE_INDEPENDENT_CURVATURE > 0)
			bezel_corner_radius = HSM_BZL_INNER_CORNER_RADIUS_SCALE * DEFAULT_SCREEN_CORNER_RADIUS;

		vec2 tube_curved_coord = HSM_GetTubeCurvedCoord(TUBE_DIFFUSE_COORD, 1, TUBE_DIFFUSE_SCALE, TUBE_SCALE, TUBE_DIFFUSE_ASPECT, 1);

		TUBE_DIFFUSE_CURVED_COORD = HSM_GetCurvedCoord(TUBE_DIFFUSE_COORD, HSM_TUBE_BLACK_EDGE_CURVATURE_SCALE, TUBE_DIFFUSE_ASPECT);
		TUBE_DIFFUSE_MASK = HSM_GetCornerMask(((TUBE_DIFFUSE_CURVED_COORD - 0.5) * 1.001) + 0.5, TUBE_DIFFUSE_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_TUBE_BLACK_EDGE_CORNER_RADIUS_SCALE, HSM_TUBE_BLACK_EDGE_SHARPNESS);

		TUBE_MASK =  HSM_GetCornerMask(tube_curved_coord, TUBE_DIFFUSE_ASPECT, bezel_corner_radius, 0.99);

		INSIDE_BEZEL_MASK = HSM_GetCornerMask(BEZEL_OUTSIDE_CURVED_COORD, TUBE_DIFFUSE_ASPECT, HSM_GLOBAL_CORNER_RADIUS * HSM_BZL_OUTER_CORNER_RADIUS_SCALE, 0.9);
		BEZEL_MASK = INSIDE_BEZEL_MASK * (1 - TUBE_MASK);

		OUTSIDE_BEZEL_MASK = 1 - INSIDE_BEZEL_MASK;
		OUTSIDE_FRAME_MASK = 1 - HSM_GetCornerMask(FRAME_OUTSIDE_CURVED_COORD, TUBE_DIFFUSE_ASPECT, HSM_FRM_OUTER_CORNER_RADIUS, 1);

		FRAME_MASK = OUTSIDE_BEZEL_MASK * (1 - OUTSIDE_FRAME_MASK);

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_SCREEN )
			out_color = HSM_PreMultAlphaBlend(out_color, vec4(0, 0, 0, 1) * (TUBE_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_SCREEN_AND_BG )
			out_color = HSM_PreMultAlphaBlend(out_color, vec4(0, 0, 0, 1) * (TUBE_MASK + OUTSIDE_FRAME_MASK));

		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_PINK_MASK )
		{
			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_SCREEN )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_DIFFUSE_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_TUBE )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (TUBE_MASK + OUTSIDE_FRAME_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (1 - BEZEL_MASK));

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_FRAME )
				out_color = HSM_PreMultAlphaBlend(out_color, vec4(1, 0, 1, 1) * (1 - FRAME_MASK));
		}
		if ( HSM_RENDER_SIMPLE_MODE == RENDER_SIMPLE_MODE_BLACK_WHITE_MASK )
		{
			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_SCREEN )
				out_color = vec4(1 - TUBE_DIFFUSE_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_TUBE )
				out_color = vec4(1 - TUBE_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL_AND_FRAME )
				out_color = vec4(1 - TUBE_MASK) - OUTSIDE_FRAME_MASK;

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_BEZEL )
				out_color = vec4(BEZEL_MASK);

			if ( HSM_RENDER_SIMPLE_MASK_TYPE == RENDER_SIMPLE_MASK_TYPE_FRAME )
				out_color = vec4(FRAME_MASK);
		}
	}

	if(HSM_COMPOSITE_SHOW_ALPHA > 0.5)
	{
		if (HSM_COMPOSITE_SHOW_ALPHA > 1.5)
			out_color.rgb = clamp(out_color.rgb * 0.5 + 0.5 * (1 - out_color.a) * vec3(0.5, 0.0, 0.0), 0.0, 1.0);
		else
			out_color = out_color.aaaa;
	}

	if (HSM_LGHT_DEBUG_MAPS == 1)
		out_color = vec4(bounce_shadow_from_top_right);

	if (HSM_LGHT_DEBUG_MAPS == 2)
		out_color = texture(NormalsAndHeightPass, vTexCoord).aaaa;

	if (HSM_LGHT_DEBUG_MAPS == 3)
		out_color = texture(NormalsAndHeightPass, vTexCoord);

#ifdef IS_HDR_PRESET	
	vec3 hdr_colour = vec3(0);
	LinearToOutputColor(out_color.rgb, hdr_colour);
	out_color = vec4(hdr_colour, 1.0f);
#else
	out_color = HSM_Delinearize(out_color, DEFAULT_SRGB_GAMMA);
#endif

	// // If we have calculated an image then set -1 as a flag to show that we have
	// if (vTexCoord.x < (2 / global.OutputSize.x) && vTexCoord.y < (2 / global.OutputSize.y))
	// 	FragColor.a = -1;

	FragColor = out_color;
}