#version 450

// NTSC-Adaptive
// based on Themaister's NTSC shader

layout(std140, set = 0, binding = 0) uniform UBO
{
   mat4 MVP;
   vec4 OutputSize;
   vec4 OriginalSize;
   vec4 SourceSize;
   float ntsc_mode;
} global;

#pragma parameter ntsc_mode "          Mode - OFF | Svideo | Composite" 0 0 2 1

float linearize = 0;

#define fetch_offset(offset, one_x) \
   texture(Source, vTexCoord + vec2((offset) * (one_x), 0.0)).xyz

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
   gl_Position = global.MVP * Position;
   vTexCoord = TexCoord - vec2(0.5 / global.SourceSize.x, 0.0); // Compensate for decimate-by-2.
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

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

const int TAPS_2_phase = 32;
const float luma_filter_2_phase[33] = float[33](
   -0.000174844,
   -0.000205844,
   -0.000149453,
   -0.000051693,
   0.000000000,
   -0.000066171,
   -0.000245058,
   -0.000432928,
   -0.000472644,
   -0.000252236,
   0.000198929,
   0.000687058,
   0.000944112,
   0.000803467,
   0.000363199,
   0.000013422,
   0.000253402,
   0.001339461,
   0.002932972,
   0.003983485,
   0.003026683,
   -0.001102056,
   -0.008373026,
   -0.016897700,
   -0.022914480,
   -0.021642347,
   -0.008863273,
   0.017271957,
   0.054921920,
   0.098342579,
   0.139044281,
   0.168055832,
   0.178571429);

const float chroma_filter_2_phase[33] = float[33](
   0.001384762,
   0.001678312,
   0.002021715,
   0.002420562,
   0.002880460,
   0.003406879,
   0.004004985,
   0.004679445,
   0.005434218,
   0.006272332,
   0.007195654,
   0.008204665,
   0.009298238,
   0.010473450,
   0.011725413,
   0.013047155,
   0.014429548,
   0.015861306,
   0.017329037,
   0.018817382,
   0.020309220,
   0.021785952,
   0.023227857,
   0.024614500,
   0.025925203,
   0.027139546,
   0.028237893,
   0.029201910,
   0.030015081,
   0.030663170,
   0.031134640,
   0.031420995,
   0.031517031);

const int TAPS_3_phase = 24;
const float luma_filter_3_phase[25] = float[25](
   -0.000012020,
   -0.000022146,
   -0.000013155,
   -0.000012020,
   -0.000049979,
   -0.000113940,
   -0.000122150,
   -0.000005612,
   0.000170516,
   0.000237199,
   0.000169640,
   0.000285688,
   0.000984574,
   0.002018683,
   0.002002275,
   -0.000909882,
   -0.007049081,
   -0.013222860,
   -0.012606931,
   0.002460860,
   0.035868225,
   0.084016453,
   0.135563500,
   0.175261268,
   0.190176552);

const float chroma_filter_3_phase[25] = float[25](
   -0.000118847,
   -0.000271306,
   -0.000502642,
   -0.000930833,
   -0.001451013,
   -0.002064744,
   -0.002700432,
   -0.003241276,
   -0.003524948,
   -0.003350284,
   -0.002491729,
   -0.000721149,
   0.002164659,
   0.006313635,
   0.011789103,
   0.018545660,
   0.026414396,
   0.035100710,
   0.044196567,
   0.053207202,
   0.061590275,
   0.068803602,
   0.074356193,
   0.077856564,
   0.079052396);

void main()
{
   	// If NTSC is off return the color from the last pass
	if (global.ntsc_mode < 1)
	{
		FragColor = texture(Source, vTexCoord);
		return;
	}

   float phase = (global.OriginalSize.x > 300.0) ? 2.0 : 3.0;
   float one_x = global.SourceSize.z;
   vec3 signal = vec3(0.0);

   if(phase < 2.5)
   {
      for (int i = 0; i < TAPS_2_phase; i++)
      {
         float offset = float(i);

         vec3 sums = fetch_offset(offset - float(TAPS_2_phase), one_x) +
            fetch_offset(float(TAPS_2_phase) - offset, one_x);
         signal += sums * vec3(luma_filter_2_phase[i], chroma_filter_2_phase[i], chroma_filter_2_phase[i]);
      }
      signal += texture(Source, vTexCoord).xyz *
         vec3(luma_filter_2_phase[TAPS_2_phase], chroma_filter_2_phase[TAPS_2_phase], chroma_filter_2_phase[TAPS_2_phase]);
   }
   else if(phase > 2.5)
   {
      for (int i = 0; i < TAPS_3_phase; i++)
      {
         float offset = float(i);

         vec3 sums = fetch_offset(offset - float(TAPS_3_phase), one_x) +
            fetch_offset(float(TAPS_3_phase) - offset, one_x);
         signal += sums * vec3(luma_filter_3_phase[i], chroma_filter_3_phase[i], chroma_filter_3_phase[i]);
      }
      signal += texture(Source, vTexCoord).xyz *
         vec3(luma_filter_3_phase[TAPS_3_phase], chroma_filter_3_phase[TAPS_3_phase], chroma_filter_3_phase[TAPS_3_phase]);
   }

   vec3 rgb = yiq2rgb(signal);
   FragColor = vec4(rgb, 1.0);
   if(linearize < 0.5) return;
   else FragColor = pow(FragColor, vec4(2.2));
} 
