#version 450

// CRT 240p Scale Shader
// by Tim C. Schroeder (visit www.blitzcode.net to learn more)

// published under MIT License
// adapted to slang format by hunterk as retrieved from github.com/blitzcode/crt-240p-scale-shader
// on 5/26/2022

layout(push_constant) uniform Push
{
	vec4 SourceSize;
	vec4 OriginalSize;
	vec4 OutputSize;
	uint FrameCount;
} params;

layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
} global;

#define InputSize params.SourceSize.xy
#define TextureSize params.SourceSize.xy
#define OutputSize params.OutputSize.xy

// Screen rotation in RA is implemented by rotating the output geometry, detect this here
bool is_vertical() { return global.MVP[0].y != 0.0; }

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 filter_offs;

void main()
{
    // Output position in clip space [-1, 1]
    vec4 clip_space_pos = global.MVP * vec4(Position.x, Position.y, 0.0, 1.0);

    bool is_vertical = is_vertical();

    // Scale factor for the vertical axis. Do nothing if input / output resolution
    // matches, center if input is lower than output, shrink to output if input is higher
    // than output. Keep in mind that our geometry might be rotated
    float vert_center = min(1.0, (is_vertical ? InputSize.x : InputSize.y) / OutputSize.y);
    clip_space_pos.y *= vert_center;

    // Handheld consoles get centered on the screen and have their correct aspect ratio
    bool is_handheld = true;
    float handheld_ar = 1.0;
         if (InputSize.x == 160.0 && InputSize.y == 144.0) handheld_ar = 1.11; // GB(C) / GG *
    else if (InputSize.x == 160.0 && InputSize.y == 152.0) handheld_ar = 1.05; // NGP(C)
    else if (InputSize.x == 224.0 && InputSize.y == 144.0) handheld_ar = 1.55; // WonderSwan
    else if (InputSize.x == 160.0 && InputSize.y == 102.0) handheld_ar = 1.57; // Atari Lynx
    else if (InputSize.x == 102.0 && InputSize.y == 160.0) handheld_ar = 0.64; // Atari Lynx Vertical **
    else if (InputSize.x == 240.0 && InputSize.y == 160.0) handheld_ar = 1.50; // GBA
    else
        is_handheld = false;
    // *  We unfortunately can't distinguish between the Game Gear and the Nintendo
    //    handhelds, causing the former to have the wrong aspect ratio as it uses
    //    non-square pixels. Feel free to change the aspect to 1.33 to reverse the
    //    situation in favor of Sega's system
    // ** This is a weird special case. Lynx seems to be the only system where vertical
    //    mode does not rotate the image in post, but the emulator actually outputs a
    //    different resolution. So it's not treated as a vertical system, is_vertical ==
    //    false and we simply treat it as a horizontal system having a tall aspect ratio

    // Fix for 2 & 3 screen wide Darius games. This is not going to look terribly good but
    // at least they're playable (sort of)
    if (InputSize.x == 640.0 && InputSize.y == 224.0)
        clip_space_pos.y /= 2.0;
    else if (InputSize.x == 864.0 && InputSize.y == 224.0)
        clip_space_pos.y /= 3.0;

    if (is_handheld)
    {
        clip_space_pos.x = clip_space_pos.x
            // This gets us to a square display
            * vert_center * (3.0 / 4.0)
            // Now it has the same AR as the physical screen of the device
            * handheld_ar
            // Aspect ratio correction when in vertical orientation
            * (is_vertical ? (1.0 / handheld_ar) * (1.0 / handheld_ar) : 1.0);
    }
    else if (is_vertical)
    {
#if 0
        // Overscan adjustment for TATE games. Most of these games don't seem to have any
        // consideration for the typical overscan present on consumer CRTs and place
        // critical elements like score and bomb counters right at the margin. Here we
        // shrink the image a little bit so it's not cut off on CRTs calibrated for
        // typical home consoles
        float overscan_adj = 1.045;
        clip_space_pos.x /= overscan_adj;
        clip_space_pos.y /= overscan_adj;
#endif

        // Correct the aspect ratio for 3:4 image in 4:3 frame
        clip_space_pos.x *= (3.0 / 4.0) * (3.0 / 4.0);
    }

    // Setup texture filtering offsets for downscaling. We do this here so we don't have
    // to repeat these calculations per-pixel
    {
        // If we want to properly filter the input texture we need to know the radius
        // which one screen pixel (pixel radius / number of output pixels) represents in
        // the normalized texture coordinates of the input texture. It's important to keep
        // in mind that the input texture might not be fully used, so we have to adjust
        // with the input-res-to-texture-size ratio to prevent an oversized filter kernel
        //
        // Why do we not simply use the automatic derivative functions dFdx() / dFdy() to
        // figure out the proper offsets? Because RPi 3B's GPU doesn't support those
        float pixel_r = 0.7;
        float support = (pixel_r / OutputSize.y) *
                        (is_vertical ? InputSize.x / TextureSize.x : InputSize.y / TextureSize.y);

        // Make sure we super-sample along the correct axis of the input texture and use
        // the filter support we just computed
        filter_offs = is_vertical ? vec2(support, 0.0) : vec2(0.0, support);
    }

    // Output

   gl_Position = clip_space_pos;
   vTexCoord = TexCoord;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 filter_offs;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

void main()
{
    bool is_vertical = is_vertical();

    // Disable texture filtering on the horizontal axis by sampling at the texel center.
    // The super-resolution output in combination with the softness of the CRT already
    // takes care of all filtering, additional bilinear lookups just introduce blurring
    // and a loss of brightness in slim features. Filtering on the vertical is fine since
    // we either match input texture to screen lines perfectly and it has no effect or
    // we're downscaling and want filtering.
    //
    // In vertical mode, disable filtering on the vertical input texture axis as it runs along
    // the horizontal screen axis due to the rotated output geometry
    vec2 tex_coord_center = vTexCoord;
    if (is_vertical)
        tex_coord_center.y = (floor(tex_coord_center.y * TextureSize.y) + 0.5) / TextureSize.y;
    else
        tex_coord_center.x = (floor(tex_coord_center.x * TextureSize.x) + 0.5) / TextureSize.x;

    // Do we have to downscale on the vertical axis (high-res arcade games, TATE games)?
    //
    // Vertical games have the horizontal input texture axis run along the vertical screen axis,
    // swap as with the filtering adjustment code above
    if (is_vertical ? OutputSize.y < InputSize.x : OutputSize.y < InputSize.y)
    {
        // Super-sampling with a tent filter and a bit of sharpening. This can never look
        // perfect and our filter is rather simplistic, but the result already looks
        // significantly better than default RA scaling and strikes a good balance between
        // shimmering and blurriness
        float sharpen = 0.7;
        FragColor =
            ( texture(Source, tex_coord_center - filter_offs * 1.5) * -sharpen +
              texture(Source, tex_coord_center - filter_offs      ) * 0.5      +
              texture(Source, tex_coord_center - filter_offs * 0.5)            +
              texture(Source, tex_coord_center)                     * 1.5      +
              texture(Source, tex_coord_center + filter_offs * 0.5)            +
              texture(Source, tex_coord_center + filter_offs      ) * 0.5      +
              texture(Source, tex_coord_center + filter_offs * 1.5) * -sharpen
            ) * (1.0 / (4.5 - 2.0 * sharpen));
    }
    else
        FragColor = texture(Source, tex_coord_center);
}