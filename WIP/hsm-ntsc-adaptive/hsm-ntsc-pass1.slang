#version 450

// NTSC-Adaptive
// based on Themaister's NTSC shader

layout(std140, set = 0, binding = 0) uniform UBO
{
   mat4 MVP;
   vec4 OutputSize;
   vec4 OriginalSize;
   vec4 SourceSize;
   uint FrameCount;
   float ntsc_mode;
} global;

// #pragma parameter quality "Quality (Composite = 0, Svideo = 1)" 0.0 0.0 1.0 1.0
// #pragma parameter bw "Black and White" 0.0 0.0 1.0 1.0

#pragma parameter NTA_TITLE		"[ --- TV PROCESS NTSC ADAPTIVE --- ]:" 0 0 0.01 0.01

#pragma parameter ntsc_mode      "          Mode - OFF | Svideo | Composite" 0 0 2 1


float quality = 1;
float bw = 0;

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 pix_no;

void main()
{
   gl_Position = global.MVP * Position;
   vTexCoord = TexCoord;
   pix_no = TexCoord * global.SourceSize.xy * (global.OutputSize.xy / global.SourceSize.xy);
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 pix_no;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

#define PI 3.14159265
float phase = (global.OriginalSize.x > 300.0) ? 2.0 : 3.0;


#define mix_mat mat3(BRIGHTNESS, FRINGING, FRINGING, ARTIFACTING, 2.0 * SATURATION, 0.0, ARTIFACTING, 0.0, 2.0 * SATURATION)

const mat3 yiq2rgb_mat = mat3(
   1.0, 0.956, 0.6210,
   1.0, -0.2720, -0.6474,
   1.0, -1.1060, 1.7046);

vec3 yiq2rgb(vec3 yiq)
{
   return yiq * yiq2rgb_mat;
}

const mat3 yiq_mat = mat3(
      0.2989, 0.5870, 0.1140,
      0.5959, -0.2744, -0.3216,
      0.2115, -0.5229, 0.3114
);

vec3 rgb2yiq(vec3 col)
{
   return col * yiq_mat;
}

void main()
{
   vec3 col = texture(Source, vTexCoord).rgb;

	// If NTSC is off return the color from the last pass
	if (global.ntsc_mode < 1)
	{
		FragColor = vec4(col, 1);
		return;
	}
   
   //* HSM Removed
   float frame_count = global.FrameCount;
   //*/
   // HSM Added
   // Removed the time based behavior with chroma phase which causes flashing sometimes
   //float frame_count = 1;

   // ntsc_mode == 1, use svideo, ntsc_mode == 2, use composite, 
   quality = global.ntsc_mode > 1 ? 0 : 1;

   float CHROMA_MOD_FREQ = (phase < 2.5) ? (4.0 * PI / 15.0) : (PI / 3.0);
   float ARTIFACTING = 1.0 - quality;
   float FRINGING = 1.0 - quality;
   float SATURATION = 1.0 - bw;
   // prevent some very slight clipping that happens at 1.0
   const float BRIGHTNESS = 0.95;


   vec3 yiq = rgb2yiq(col);

   float chroma_phase = (phase < 2.5) ? PI * (mod(pix_no.y, 2.0) + mod(frame_count, 2.)) : 0.6667 * PI * (mod(pix_no.y, 3.0) + mod(frame_count, 2.));

   float mod_phase = chroma_phase + pix_no.x * CHROMA_MOD_FREQ;

   float i_mod = cos(mod_phase);
   float q_mod = sin(mod_phase);

   yiq.yz *= vec2(i_mod, q_mod); // Modulate.
   yiq *= mix_mat; // Cross-talk.
   yiq.yz *= vec2(i_mod, q_mod); // Demodulate.
   FragColor = vec4(yiq, 1.0);
} 
