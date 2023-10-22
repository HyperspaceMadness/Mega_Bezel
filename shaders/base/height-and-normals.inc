/*
    Mega Bezel - Creates a graphic treatment for the game play area to give a retro feel
    Copyright (C) 2019-2023 HyperspaceMadness - HyperspaceMadness@outlook.com

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

/////////////// IMPORTS ///////////////
#include "common/globals-and-image-layers-params.inc"
#include "common/common-functions.inc"
#include "common/common-functions-bezel.inc"

#pragma name NormalsAndHeightPass

// Float format so we can avoid banding
#pragma format R32G32B32A32_SFLOAT

float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Generating height & normals from a shadertoy by granito
const vec3 GrayscaleWeights = vec3(0.2125,0.7154,0.0721);

// Sample map to generate Normal map and Height map
vec4 GenerateNormalHeight (sampler2D tex, vec2 uv, vec2 res, float width)
{
    vec2 texelSize = 1.0 / (res * width);
    vec4 h;
    h[0] = dot(GrayscaleWeights, HSM_Linearize(texture(tex, uv + vec2(texelSize * vec2( 0,-1)) ), DEFAULT_SRGB_GAMMA).rgb);
    h[1] = dot(GrayscaleWeights, HSM_Linearize(texture(tex, uv + vec2(texelSize * vec2(-1, 0)) ), DEFAULT_SRGB_GAMMA).rgb);
    h[2] = dot(GrayscaleWeights, HSM_Linearize(texture(tex, uv + vec2(texelSize * vec2( 1, 0)) ), DEFAULT_SRGB_GAMMA).rgb);
    h[3] = dot(GrayscaleWeights, HSM_Linearize(texture(tex, uv + vec2(texelSize * vec2( 0, 1)) ), DEFAULT_SRGB_GAMMA).rgb);
    vec3 n;
    n.y = h[3] - h[0];
    n.x = h[1] - h[2];
    n.z = .25;
    float height = dot(GrayscaleWeights, texture(tex, uv).rgb);
    n = normalize(n);
    return vec4(n, height);
}

// Multipass version of above
vec4 GenerateNormalHeightMultipass (sampler2D tex, vec2 uv, vec2 res)
{
    // uv += 1.0 / res * (random(uv) - 0.5) * 3;
    float dist = 0.5;
    vec4 multi = vec4(0.);
	multi += GenerateNormalHeight(tex, uv, res, 1. * dist);
	multi += GenerateNormalHeight(tex, uv, res, 2.5 * dist);
	multi += GenerateNormalHeight(tex, uv, res, 5. * dist);
	multi += GenerateNormalHeight(tex, uv, res, 10. * dist);    
    multi *= 0.25;

	multi = GenerateNormalHeight(tex, uv, res, 1. * dist);
    return multi;
}

// TODO see if there is a simpler way
// vec3 GetNormalFromHeight(in sampler2D height_map, vec2 uv, float intensity, float width, vec2 map_res)
// {
//     vec2 offset = 1 / map_res * width;
//     vec2 coord_plus_offset = uv + offset;
//     vec2 coord_minus_offset = uv - offset;
//     vec3 a = vec3(coord_minus_offset.x, 0.0, texture(height_map, vec2(coord_minus_offset.x, uv.y)).a * intensity);
//     vec3 b = vec3(coord_plus_offset.x, 0.0, texture(height_map, vec2(coord_plus_offset.x, uv.y)).a * intensity);
//     vec3 c = vec3(0.0, coord_plus_offset.y, texture(height_map, vec2(uv.x, coord_plus_offset.y)).a * intensity);
//     vec3 d = vec3(0.0, coord_minus_offset.y, texture(height_map, vec2(uv.x, coord_minus_offset.y)).a * intensity);

//     return normalize(cross(b-a, c-d));
// }

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

// Pass Framebuffer Textures
layout(set = 0, binding = 1)	uniform sampler2D CacheInfoPass;

//  Layers
layout(set = 0, binding = 2)	uniform sampler2D Background;
layout(set = 0, binding = 3)	uniform sampler2D BackgroundNormals;
layout(set = 0, binding = 4)	uniform sampler2D BackgroundHeight;

layout(set = 0, binding = 5)	uniform sampler2D Device;
layout(set = 0, binding = 6)	uniform sampler2D DeviceNormals;
layout(set = 0, binding = 7)	uniform sampler2D DeviceHeight;

layout(set = 0, binding = 8)	uniform sampler2D NormalsAndHeightPassFeedback;
#define PassFeedback NormalsAndHeightPassFeedback

// layout(set = 0, binding = 8)	uniform sampler2D ReflectionMask;


//////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    HSM_UpdateCacheInfoChanged(CacheInfoPass);

	// vec4 feedback_color_test = texture(PassFeedback, vec2(0,0));
	if (HSM_CACHE_GRAPHICS_ON > 0.5 && !CACHE_INFO_CHANGED || HSM_AB_COMPARE_FREEZE_GRAPHICS == 1 && HSM_GetIsInABCompareArea(vTexCoord))
	{
		FragColor = texture(PassFeedback, vTexCoord);
		return;
	}

	vec2 vTexCoord_precomp = vTexCoord;

	HSM_UpdateGlobalScreenValuesFromCache(CacheInfoPass, vTexCoord_precomp);
	VIEWPORT_FLIPPED_UNZOOMED_COORD = HSM_GetViewportCoordWithFlip(vTexCoord_precomp);
	vec2 VIEWPORT_COORD = HSM_GetViewportCoordWithZoomAndPan(vTexCoord_precomp);

	HSM_UpdateBezelAndTubeGlobalValuesAndMasks(TUBE_DIFFUSE_COORD, 
												TUBE_DIFFUSE_SCALE, 
												TUBE_SCALE, 
												TUBE_DIFFUSE_ASPECT, 
												BEZEL_OUTSIDE_SCALE,
												BEZEL_OUTSIDE_COORD, 
												BEZEL_OUTSIDE_CURVED_COORD, 
												FRAME_OUTSIDE_CURVED_COORD,
												OUTSIDE_BEZEL_MASK,
												OUTSIDE_FRAME_MASK,
												INSIDE_BEZEL_MASK,
												BEZEL_MASK,
												FRAME_MASK,
												OUTSIDE_TUBE_MASK
												);

	float output_aspect = global.OutputSize.x / global.OutputSize.y;

    vec3 neutral_normals = vec3(0.5, 0.5, 1);
    float neutral_height = 0.5;

	vec4 out_color = vec4(0, 0, 0, 1);
	float out_height = neutral_height;
	vec3 out_normals = neutral_normals;

    vec4 out_normal_color = vec4(neutral_normals, 1.0);
    vec4 out_height_color = vec4(vec3(neutral_height), 1.0);

	CUTOUT_MASK = 1;

    float normal_intensity = 1;
    float normal_width = 1;

    //-----------------------------------------------------------------------------------------
    // Background
    //-----------------------------------------------------------------------------------------
    vec2 BACKGROUND_SIZE = textureSize(Background, 0);
    if (HSM_BG_OPACITY > 0 && BACKGROUND_SIZE.y > 16)
    {
        BACKGROUND_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
                                                    VIEWPORT_FLIPPED_UNZOOMED_COORD,
                                                    TEXTURE_ASPECT_MODE_EXPLICIT,
                                                    BACKGROUND_SIZE.x / BACKGROUND_SIZE.y,
                                                    vec2(HSM_BG_POS_X, HSM_BG_POS_Y),
                                                    vec2(HSM_BG_SCALE * HSM_BG_SCALE_X, HSM_BG_SCALE),
                                                    HSM_BG_FOLLOW_LAYER,
                                                    HSM_BG_FOLLOW_MODE,
                                                    HSM_BG_FOLLOW_FULL_USES_ZOOM,
                                                    HSM_BG_FILL_MODE,
                                                    HSM_BG_SPLIT_PRESERVE_CENTER,
                                                    HSM_BG_SPLIT_REPEAT_WIDTH,
                                                    true,
                                                    BACKGROUND_COORD,
                                                    BACKGROUND_SCALE);

        // Tile Wrap the background
        if (HSM_BG_WRAP_MODE == 1)
            BACKGROUND_CURVED_COORD = mod(BACKGROUND_CURVED_COORD, 1.0);
        
        // Mirror Wrap the bBackground
        if (HSM_BG_WRAP_MODE == 2)
            BACKGROUND_CURVED_COORD = HSM_GetMirrorWrapCoord(BACKGROUND_CURVED_COORD);

        vec4 bg_layer = HSM_GetMipmappedTexSample(Background, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);

        vec4 bg_normals_layer = vec4(neutral_normals * bg_layer.a, bg_layer.a);
        if (HSM_LGHT_NORMAL_SOURCE == 1)
        {
            bg_normals_layer = HSM_GetMipmappedTexSample(BackgroundNormals, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);
            bg_normals_layer = HSM_Linearize(bg_normals_layer, DEFAULT_SRGB_GAMMA);
        }
        if (HSM_LGHT_NORMAL_SOURCE == 2)
        {
            bg_normals_layer = vec4(GenerateNormalHeightMultipass(BackgroundHeight, BACKGROUND_CURVED_COORD, textureSize(BackgroundHeight, 0)).rgb, 1);
            bg_normals_layer.rgb = (normalize(bg_normals_layer.rgb) + 1) / 2;
        }

        bg_normals_layer *= bg_layer.a;

        vec4 bg_height_layer = HSM_GetMipmappedTexSample(BackgroundHeight, BACKGROUND_CURVED_COORD, BACKGROUND_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);
        bg_height_layer = HSM_Linearize(bg_height_layer, DEFAULT_SRGB_GAMMA);
        bg_height_layer.rgb = (bg_height_layer.rgb - HSM_HSHADOW_BG_CONTRAST_CENTER) * HSM_HSHADOW_BG_CONTRAST + HSM_HSHADOW_BG_CONTRAST_CENTER;
        bg_height_layer.rgb += HSM_HSHADOW_BG_OFFSET_Z;
        bg_height_layer.rgb = clamp(bg_height_layer.rgb, 0, 1);
        bg_height_layer.rgb *= bg_layer.a;

        out_normal_color = HSM_PreMultAlphaBlend(out_normal_color, bg_normals_layer);
        out_height_color = HSM_PreMultAlphaBlend(out_height_color, bg_height_layer);

    }

    //----------------------------------------------------
    // Device 
    //----------------------------------------------------
    float device_center_height = HSM_Linearize(HSM_GetMipmappedTexSample(DeviceHeight, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS), DEFAULT_SRGB_GAMMA).r;
    device_center_height = (device_center_height - HSM_HSHADOW_DEVICE_CONTRAST_CENTER) * HSM_HSHADOW_DEVICE_CONTRAST + HSM_HSHADOW_DEVICE_CONTRAST_CENTER;
    device_center_height += HSM_HSHADOW_DEVICE_OFFSET_Z;

    vec2 DEVICE_SIZE = textureSize(Device, 0);
    if (HSM_DEVICE_OPACITY > 0 && DEVICE_SIZE.y > 16)
    {
        DEVICE_CURVED_COORD = HSM_GetScaledCoord(VIEWPORT_COORD,
                                                VIEWPORT_FLIPPED_UNZOOMED_COORD,
                                                TEXTURE_ASPECT_MODE_EXPLICIT,
                                                DEVICE_SIZE.x / DEVICE_SIZE.y,
                                                vec2(HSM_DEVICE_POS_X, HSM_DEVICE_POS_Y),
                                                vec2(HSM_DEVICE_SCALE * HSM_DEVICE_SCALE_X, HSM_DEVICE_SCALE),
                                                HSM_DEVICE_FOLLOW_LAYER,
                                                HSM_DEVICE_FOLLOW_MODE,
                                                HSM_DEVICE_FOLLOW_FULL_USES_ZOOM,
                                                HSM_DEVICE_FILL_MODE,
                                                HSM_DEVICE_SPLIT_PRESERVE_CENTER,
                                                HSM_DEVICE_SPLIT_REPEAT_WIDTH,
                                                true,
                                                DEVICE_COORD,
                                                DEVICE_SCALE);

            vec4 device_layer = HSM_GetMipmappedTexSample(Device, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);

            vec4 device_normals_layer = vec4(neutral_normals, 1);
            if (HSM_LGHT_NORMAL_SOURCE == 1)
            {
                // device_normals_layer = HSM_GetMipmappedTexSample(DeviceNormals, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS) * 2.0 - 1.0;
                device_normals_layer = HSM_GetMipmappedTexSample(DeviceNormals, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);
                device_normals_layer = HSM_Linearize(device_normals_layer, DEFAULT_SRGB_GAMMA);
            }
            if (HSM_LGHT_NORMAL_SOURCE == 2)
            {
                device_normals_layer = vec4(GenerateNormalHeightMultipass(DeviceHeight, DEVICE_CURVED_COORD, textureSize(DeviceHeight, 0)).rgb, 1);
                device_normals_layer.rgb = (normalize(device_normals_layer.rgb) + 1) / 2;
            }

            device_normals_layer *= device_layer.a;

            vec4 device_height_layer = HSM_GetMipmappedTexSample(DeviceHeight, DEVICE_CURVED_COORD, DEVICE_SCALE, HSM_BG_MIPMAPPING_BLEND_BIAS);
            device_height_layer = HSM_Linearize(device_height_layer, DEFAULT_SRGB_GAMMA);

            device_height_layer.rgb = (device_height_layer.rgb - HSM_HSHADOW_DEVICE_CONTRAST_CENTER) * HSM_HSHADOW_DEVICE_CONTRAST + HSM_HSHADOW_DEVICE_CONTRAST_CENTER;
            device_height_layer.rgb += HSM_HSHADOW_DEVICE_OFFSET_Z;
            device_height_layer.rgb = clamp(device_height_layer.rgb, 0, 1);
            device_height_layer *= device_layer.a;

            out_normal_color = HSM_PreMultAlphaBlend(out_normal_color, device_normals_layer);
            out_height_color = HSM_PreMultAlphaBlend(out_height_color, device_height_layer);

    }
// (normalize(device_normals_layer.rgb) + 1) / 2;
    
    out_normal_color.rgb = out_normal_color.rgb * 2 - 1;
    out_normal_color.rg *= HSM_LGHT_NORMAL_STRENGTH;
    out_normal_color.rgb = (normalize(out_normal_color.rgb) + 1) / 2;
    out_normal_color = normalize(out_normal_color);

    //----------------------------------------------------
    // Generated Bezel
    //----------------------------------------------------
    if (HSM_BZL_OPACITY > 0 || HSM_FRM_OPACITY > 0)
    {

        bool generate_normals_and_height = true;
        vec4 frame_highlight_color = vec4(0);
        float static_shadow_multiply = 1;
        vec4 tube_bg_layer = vec4(0);

        vec4 bezel_color_layer = vec4(0);
        vec4 bezel_normals_layer = vec4(0);
        vec4 bezel_height_layer = vec4(0);

        // TODO - This is Temp
        device_center_height = 1;
        float bezel_outer_edge_height = device_center_height + HSM_HSHADOW_BEZEL_OFFSET_Z;

        // TODO need to pass in more channels, e.g. color rgba, normals rgba, height float
        HSM_GenerateBezelLayer(generate_normals_and_height,
                                bezel_outer_edge_height, 
                                frame_highlight_color, 
                                static_shadow_multiply, 
                                bezel_color_layer, 
                                bezel_normals_layer, 
                                bezel_height_layer, 
                                tube_bg_layer);

        // TODO need to offset by frame height as well
        // bezel_height_layer.rgb = bezel_height_layer.rgb - bezel_height_layer.a * ((1 - device_center_height) + HSM_HSHADOW_BEZEL_OFFSET_Z);
        // bezel_height = clamp(bezel_height, 0, 1);

        out_normal_color = HSM_PreMultAlphaBlend(out_normal_color, bezel_normals_layer);
        out_height_color = HSM_PreMultAlphaBlend(out_height_color, bezel_height_layer);
    }

    // out_height_color *= 0.33;
    out_normal_color.rgb = normalize(out_normal_color.rgb);
    out_normal_color = HSM_Delinearize(out_normal_color, DEFAULT_SRGB_GAMMA);
    FragColor = vec4(out_normal_color.rgb, out_height_color.r);
}