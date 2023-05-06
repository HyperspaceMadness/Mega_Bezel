#version 450

layout(push_constant) uniform Push
{
    float g_signal_type;
    float g_crtgamut;
    float g_space_out;
    float g_hue_degrees;
    float g_U_SHIFT;
    float g_V_SHIFT;
    float g_U_MUL;
    float g_V_MUL;
    float g_CRT_b;
    float g_CRT_c;
    float g_CRT_l;
    float g_lum_fix;
    float g_vstr;
    float g_vpower;
    float g_sat;
    float g_vibr;
    float g_lum;
    float g_cntrst;
    float g_mid;
    float g_lift;
    float blr;
    float blg;
    float blb;
    float wlr;
    float wlg;
    float wlb;
    float rg;
    float rb;
    float gr;
    float gb;
    float br;
    float bg;
} params;

layout(std140, set = 0, binding = 0) uniform UBO
{
    mat4 MVP;
    vec4 SourceSize;
    vec4 OriginalSize;
    vec4 OutputSize;
    uint FrameCount;
    float g_vignette;
    float g_Dim_to_Dark;
    float wp_temperature;
    float g_satr;
    float g_satg;
    float g_satb;
    float LUT_Size1;
    float LUT1_toggle;
    float LUT_Size2;
    float LUT2_toggle;
} global;

/*
   Grade
   > Ubershader grouping some monolithic color related shaders:
    ::color-mangler (hunterk), ntsc color tuning knobs (Doriphor), white_point (hunterk, Dogway), RA Reshade LUT.
   > and the addition of:
    ::analogue color emulation, phosphor gamut, color space + TRC support, vibrance, HUE vs SAT, vignette (shared by Syh), black level, rolled gain and sigmoidal contrast.

   Author: Dogway
   License: Public domain

   **Thanks to those that helped me out keep motivated by continuous feedback and bug reports:
   **Syh, Nesguy, hunterk, and the libretro forum members.


    ######################################...PRESETS...#######################################
    ##########################################################################################
    ###                                                                                    ###
    ###    PAL                                                                             ###
    ###        Phosphor: 470BG (#3)                                                        ###
    ###        WP: D65 (6504K)               (in practice more like 7000K-7500K range)     ###
    ###        Saturation: -0.02                                                           ###
    ###                                                                                    ###
    ###    NTSC-U                                                                          ###
    ###        Phosphor: P22/SMPTE-C (#1 #-3)(or a SMPTE-C based CRT phosphor gamut)       ###
    ###        WP: D65 (6504K)               (in practice more like  7000K-7500K range)    ###
    ###                                                                                    ###
    ###    NTSC-J (Default)                                                                ###
    ###        Phosphor: NTSC-J (#2)         (or a NTSC-J based CRT phosphor gamut)        ###
    ###        WP: 9300K+27MPCD (8945K)      (CCT from x:0.281 y:0.311)(in practice ~8600K)###
    ###                                                                                    ###
    ###                                                                                    ###
    ##########################################################################################
    ##########################################################################################
*/


#pragma parameter g_signal_type  "Signal Type (0:RGB 1:Composite)"                           1.0  0.0 1.0 1.0
#pragma parameter g_crtgamut     "Phosphor (-2:CRT-95s -1:P22-80s 1:P22-90s 2:NTSC-J 3:PAL)" 2.0 -3.0 3.0 1.0
#pragma parameter g_space_out    "Diplay Color Space (-1:709 0:sRGB 1:DCI 2:2020 3:Adobe)"   0.0 -1.0 3.0 1.0
#pragma parameter g_Dim_to_Dark  "Dim to Dark adaptation"                                    1.0  0.0 1.0 1.0

// Analogue controls
#pragma parameter g_hue_degrees  "CRT Hue"              0.0 -360.0 360.0  1.0
#pragma parameter g_U_SHIFT      "CRT U Shift"          0.0   -0.2   0.2  0.01
#pragma parameter g_V_SHIFT      "CRT V Shift"          0.0   -0.2   0.2  0.01
#pragma parameter g_U_MUL        "CRT U Multiplier"     1.0    0.0   2.0  0.01
#pragma parameter g_V_MUL        "CRT V Multiplier"     1.0    0.0   2.0  0.01
#pragma parameter g_CRT_b        "CRT Brightness"       0.0    0.0 100.0  1.0
#pragma parameter g_CRT_c        "CRT Contrast"         100.0 50.0 150.0  1.0
#pragma parameter g_CRT_l        "CRT Black Level"      0.1    0.0   0.35 0.01
#pragma parameter g_vignette     "Vignette Toggle"      1.0    0.0   1.0  1.0
#pragma parameter g_vstr         "Vignette Strength"    40.0   0.0  50.0  1.0
#pragma parameter g_vpower       "Vignette Power"       0.20   0.0   0.5  0.01

// Digital controls
#pragma parameter g_lum_fix      "Sega Luma Fix"        0.0  0.0 1.0 1.0
#pragma parameter g_lum          "Brightness"           0.0 -0.5 1.0 0.01
#pragma parameter g_cntrst       "Contrast"             0.0 -1.0 1.0 0.05
#pragma parameter g_mid          "Contrast Pivot"       0.5  0.0 1.0 0.01
#pragma parameter wp_temperature "White Point"          8604.0 5004.0 12004.0 100.0
#pragma parameter g_sat          "Saturation"           0.0 -1.0 2.0 0.01
#pragma parameter g_vibr         "Dullness/Vibrance"    0.0 -1.0 1.0 0.05
#pragma parameter g_satr         "Hue vs Sat Red"       0.0 -1.0 1.0 0.01
#pragma parameter g_satg         "Hue vs Sat Green"     0.0 -1.0 1.0 0.01
#pragma parameter g_satb         "Hue vs Sat Blue"      0.0 -1.0 1.0 0.01
#pragma parameter g_lift         "Black Level"          0.0 -0.5 0.5 0.01
#pragma parameter blr            "Black-Red Tint"       0.0  0.0 1.0 0.01
#pragma parameter blg            "Black-Green Tint"     0.0  0.0 1.0 0.01
#pragma parameter blb            "Black-Blue Tint"      0.0  0.0 1.0 0.01
#pragma parameter wlr            "White-Red Tint"       1.0  0.0 2.0 0.01
#pragma parameter wlg            "White-Green Tint"     1.0  0.0 2.0 0.01
#pragma parameter wlb            "White-Blue Tint"      1.0  0.0 2.0 0.01
#pragma parameter rg             "Red-Green Tint"       0.0 -1.0 1.0 0.005
#pragma parameter rb             "Red-Blue Tint"        0.0 -1.0 1.0 0.005
#pragma parameter gr             "Green-Red Tint"       0.0 -1.0 1.0 0.005
#pragma parameter gb             "Green-Blue Tint"      0.0 -1.0 1.0 0.005
#pragma parameter br             "Blue-Red Tint"        0.0 -1.0 1.0 0.005
#pragma parameter bg             "Blue-Green Tint"      0.0 -1.0 1.0 0.005
#pragma parameter LUT_Size1      "LUT Size 1"           16.0 8.0 64.0 16.0
#pragma parameter LUT1_toggle    "LUT 1 Toggle"         0.0  0.0 1.0 1.0
#pragma parameter LUT_Size2      "LUT Size 2"           64.0 0.0 64.0 16.0
#pragma parameter LUT2_toggle    "LUT 2 Toggle"         0.0  0.0 1.0 1.0

#define M_PI            3.1415926535897932384626433832795/180.0
#define signal          params.g_signal_type
#define crtgamut        params.g_crtgamut
#define SPC             params.g_space_out
#define hue_degrees     params.g_hue_degrees
#define U_SHIFT         params.g_U_SHIFT
#define V_SHIFT         params.g_V_SHIFT
#define U_MUL           params.g_U_MUL
#define V_MUL           params.g_V_MUL
#define lum_fix         params.g_lum_fix
#define vignette        global.g_vignette
#define vstr            params.g_vstr
#define vpower          params.g_vpower
#define g_sat           params.g_sat
#define vibr            params.g_vibr
#define satr            global.g_satr
#define satg            global.g_satg
#define satb            global.g_satb
#define lum             params.g_lum
#define cntrst          params.g_cntrst
#define mid             params.g_mid
#define lift            params.g_lift
#define blr             params.blr
#define blg             params.blg
#define blb             params.blb
#define wlr             params.wlr
#define wlg             params.wlg
#define wlb             params.wlb
#define rg              params.rg
#define rb              params.rb
#define gr              params.gr
#define gb              params.gb
#define br              params.br
#define bg              params.bg

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
    gl_Position = global.MVP * Position;
    vTexCoord = TexCoord;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D SamplerLUT1;
layout(set = 0, binding = 4) uniform sampler2D SamplerLUT2;



///////////////////////// Color Space Transformations //////////////////////////


mat3 RGB_to_XYZ_mat(mat3 primaries, int illuminant) {

    // 1 for "D93" else "D65" reference white
    vec3 RW = illuminant > 0 ? vec3(0.903536977491961, 1., 1.311897106109325) : \
                               vec3(0.950457397565471, 1., 1.089436035930324) ;

    vec3 T  = RW * transpose(inverse(primaries));

    mat3 TB = mat3(
                T.x, 0, 0,
                0, T.y, 0,
                0, 0, T.z);

   return TB * primaries;
}


vec3 RGB_to_XYZ(vec3 RGB, mat3 primaries, int illuminant) {

   return RGB *        RGB_to_XYZ_mat(primaries, illuminant);
}

vec3 XYZ_to_RGB(vec3 XYZ, mat3 primaries, int illuminant) {

   return XYZ * inverse(RGB_to_XYZ_mat(primaries, illuminant));
}



vec3 XYZtoYxy(vec3 XYZ) {

    float XYZrgb = XYZ.r+XYZ.g+XYZ.b;
    float Yxyg = (XYZrgb <= 0.0) ? 0.3805 : XYZ.r / XYZrgb;
    float Yxyb = (XYZrgb <= 0.0) ? 0.3769 : XYZ.g / XYZrgb;
    return vec3(XYZ.g, Yxyg, Yxyb);
}

vec3 YxytoXYZ(vec3 Yxy) {

    float Xs = Yxy.r * (Yxy.g/Yxy.b);
    float Xsz = (Yxy.r <= 0.0) ? 0.0 : 1.0;
    vec3 XYZ = vec3(Xsz,Xsz,Xsz) * vec3(Xs, Yxy.r, (Xs/Yxy.g)-Xs-Yxy.r);
    return XYZ;
}


///////////////////////// White Point Mapping /////////////////////////
//
//
// PAL: D65        NTSC-U: D65       NTSC-J: CCT NTSC-J
// PAL: 6503.512K  NTSC-U: 6503.512K NTSC-J: 8945.436K
// [x:0.31266142   y:0.3289589]      [x:0.281 y:0.311]

// For NTSC-J there's not a common agreed value, measured consumer units span from 8229.87K to 8945.623K with accounts for 8800K as well.
// Recently it's been standardized to 9300K which is closer to what master monitors were (x=0.2838 y=0.2984) (9177.98K)


// Does a RGB to XYZ -> Temperature joint matrix
vec3 wp_adjust(vec3 RGB, float temperature, mat3 primaries, int illuminant) {

    float temp3 = 1000.       / temperature;
    float temp6 = 1000000.    / pow(temperature, 2.);
    float temp9 = 1000000000. / pow(temperature, 3.);

    vec3 wp = vec3(1.);

    wp.x = (temperature < 5500.) ? 0.244058 + 0.0989971 * temp3 + 2.96545 * temp6 - 4.59673 * temp9 : \
           (temperature < 8000.) ? 0.200033 + 0.9545630 * temp3 - 2.53169 * temp6 + 7.08578 * temp9 : \
                                   0.237045 + 0.2437440 * temp3 + 1.94062 * temp6 - 2.11004 * temp9 ;

    wp.y = -0.275275 + 2.87396 * wp.x - 3.02034 * pow(wp.x,2) + 0.0297408 * pow(wp.x,3);
    wp.z = 1. - wp.x - wp.y;


    vec3 RW = illuminant > 0 ? vec3(0.903536977491961, 1., 1.311897106109325) : \
                               vec3(0.950457397565471, 1., 1.089436035930324) ;

    const mat3 CAT16 = mat3(
     0.401288, 0.650173, -0.051461,
    -0.250268, 1.204414,  0.045854,
    -0.002079, 0.048952,  0.953127);

    vec3 VKV = (vec3(wp.x/wp.y,1.,wp.z/wp.y) * CAT16) / (RW * CAT16);

    mat3 VK = mat3(
                VKV.x, 0.0, 0.0,
                0.0, VKV.y, 0.0,
                0.0, 0.0, VKV.z);

    mat3 CAM  = CAT16 * (VK * inverse(CAT16));

    mat3 mata = RGB_to_XYZ_mat(primaries, illuminant);

    // Originally   "(mata * CAM) * matb" but we want to output XYZ so omit
    // In "D93" we omit CAM as we are in compensation mode, not simulation nor adaptation
    return illuminant > 0 ? RGB.xyz * (mata * VK) : RGB.xyz * (mata * CAM);
}


////////////////////////////////////////////////////////////////////////////////


// CRT Curve Function
//----------------------------------------------------------------------

float EOTF_1886a(float color, float bl, float brightness, float contrast) {

    // Defaults:
    //  Black Level = 0.1
    //  Brightness  = 0
    //  Contrast    = 100

    float wl = 100.0;
    float b  = pow(bl, 1/2.4);
    float a  = pow(wl, 1/2.4)-b;
          b  = brightness>0  ? (brightness/286.+b/a) : b/a;
          a  = contrast!=100 ? contrast/100.         : 1;

    float Vc = 0.35;                           // Offset
    float Lw = wl/100. * a;                    // White level
    float Lb = clamp(b * a,0.01,Vc);           // Black level
    float a1 = 2.6;                            // Shoulder gamma
    float a2 = 3.0;                            // Knee gamma
    float k  = Lw /pow(1  + Lb,    a1);
    float sl = k * pow(Vc + Lb, a1-a2);        // Slope for knee gamma

    color = color >= Vc ? k * pow((color + Lb), a1 ) : sl * pow((color + Lb), a2 );
    return color;
 }

vec3 EOTF_1886a_f3( vec3 color, float BlackLevel, float brightness, float contrast) {

    color.r = EOTF_1886a( color.r, BlackLevel, brightness, contrast);
    color.g = EOTF_1886a( color.g, BlackLevel, brightness, contrast);
    color.b = EOTF_1886a( color.b, BlackLevel, brightness, contrast);
    return color.rgb;
 }



// Monitor Curve Functions: https://github.com/ampas/aces-dev
//----------------------------------------------------------------------


float moncurve_f( float color, float gamma, float offs)
{
    // Forward monitor curve
    color    = clamp(color, 0.0, 1.0);
    float fs = (( gamma - 1.0) / offs) * pow( offs * gamma / ( ( gamma - 1.0) * ( 1.0 + offs)), gamma);
    float xb = offs / ( gamma - 1.0);

    color = ( color > xb) ? pow( ( color + offs) / ( 1.0 + offs), gamma) : color * fs;
    return color;
}


vec3 moncurve_f_f3( vec3 color, float gamma, float offs)
{
    color.r = moncurve_f( color.r, gamma, offs);
    color.g = moncurve_f( color.g, gamma, offs);
    color.b = moncurve_f( color.b, gamma, offs);
    return color.rgb;
}


float moncurve_r( float color, float gamma, float offs)
{
    // Reverse monitor curve
    color = clamp(color, 0.0, 1.0);
    float yb = pow( offs * gamma / ( ( gamma - 1.0) * ( 1.0 + offs)), gamma);
    float rs = pow( ( gamma - 1.0) / offs, gamma - 1.0) * pow( ( 1.0 + offs) / gamma, gamma);

    color = ( color > yb) ? ( 1.0 + offs) * pow( color, 1.0 / gamma) - offs : color * rs;
    return color;
}


vec3 moncurve_r_f3( vec3 color, float gamma, float offs)
{
    color.r = moncurve_r( color.r, gamma, offs);
    color.g = moncurve_r( color.g, gamma, offs);
    color.b = moncurve_r( color.b, gamma, offs);
    return color.rgb;
}


//-------------------------- Luma Functions ----------------------------


//  Performs better in gamma encoded space
float contrast_sigmoid(float color, float cont, float pivot){

    cont = pow(cont + 1., 3.);

    float knee  = 1. / (1. + exp(cont *  pivot));
    float shldr = 1. / (1. + exp(cont * (pivot - 1.)));

    color       =(1. / (1. + exp(cont * (pivot - color))) - knee) / (shldr - knee);

    return color;
}


//  Performs better in gamma encoded space
float contrast_sigmoid_inv(float color, float cont, float pivot){

    cont = pow(cont - 1., 3.);

    float knee  = 1. / (1. + exp (cont *  pivot));
    float shldr = 1. / (1. + exp (cont * (pivot - 1.)));

    color = pivot - log(1. / (color * (shldr - knee) + knee) - 1.) / cont;

    return color;
}


float rolled_gain(float color, float gain){

    float gx   = abs(gain) + 0.001;
    float anch = (gain > 0.0) ? 0.5 / (gx / 2.0) : 0.5 / gx;
    color      = (gain > 0.0) ? color * ((color - anch) / (1 - anch)) : color * ((1 - anch) / (color - anch)) * (1 - gain);

    return color;
}


vec4 rolled_gain_v4(vec4 color, float gain){

    color.r = rolled_gain(color.r, gain);
    color.g = rolled_gain(color.g, gain);
    color.b = rolled_gain(color.b, gain);

    return vec4(color.rgb, 1.0);
}


float SatMask(float color_r, float color_g, float color_b)
{
    float max_rgb = max(color_r, max(color_g, color_b));
    float min_rgb = min(color_r, min(color_g, color_b));
    float msk = clamp((max_rgb - min_rgb) / (max_rgb + min_rgb), 0.0, 1.0);
    return msk;
}


//  This shouldn't be necessary but it seems some undefined values can
//  creep in and each GPU vendor handles that differently. This keeps
//  all values within a safe range
vec3 mixfix(vec3 a, vec3 b, float c)
{
    return (a.z < 1.0) ? mix(a, b, c) : a;
}


vec4 mixfix_v4(vec4 a, vec4 b, float c)
{
    return (a.z < 1.0) ? mix(a, b, c) : a;
}


//---------------------- Range Expansion/Compression -------------------

//  0-235 YUV PAL
//  0-235 YUV NTSC-J
// 16-235 YUV NTSC

//  to Studio Swing/Broadcast Safe/SMPTE legal/Limited Range
vec3 PCtoTV(vec3 col, float luma_swing, float Umax, float Vmax, float max_swing, bool rgb_in)
{
   col *= 255.;
   Umax = (max_swing == 1.0) ? Umax * 224. : Umax * 239.;
   Vmax = (max_swing == 1.0) ? Vmax * 224. : Vmax * 239.;

   col.x = (luma_swing == 1.0) ? ((col.x * 219.) / 255.) + 16. : col.x;
   col.y = (rgb_in == true) ? ((col.y * 219.) / 255.) + 16. : (((col.y - 128.) * (Umax * 2.)) / 255.) + Umax;
   col.z = (rgb_in == true) ? ((col.z * 219.) / 255.) + 16. : (((col.z - 128.) * (Vmax * 2.)) / 255.) + Vmax;
   return col.xyz / 255.;
}


//  to Full Swing/Full Range
vec3 TVtoPC(vec3 col, float luma_swing, float Umax, float Vmax, float max_swing, bool rgb_in)
{
   col *= 255.;
   Umax = (max_swing == 1.0) ? Umax * 224. : Umax * 239.;
   Vmax = (max_swing == 1.0) ? Vmax * 224. : Vmax * 239.;

   float colx = (luma_swing == 1.0) ? ((col.x - 16.) / 219.) * 255. : col.x;
   float coly = (rgb_in == true) ? ((col.y - 16.) / 219.) * 255. : (((col.y - Umax) / (Umax * 2.)) * 255.) + 128.;
   float colz = (rgb_in == true) ? ((col.z - 16.) / 219.) * 255. : (((col.z - Vmax) / (Vmax * 2.)) * 255.) + 128.;
   return vec3(colx,coly,colz) / 255.;
}


//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/



//----------------------- ITU-R BT.470/601 (B/G)  &  SMPTE RP 145-1994 -----------------------

// Bymax 0.885515
// Rymax 0.701088
// R'G'B' full range to Decorrelated Intermediate (Y,B-Y,R-Y)
// Rows should sum to 0, except first one which sums 1
const float[9] YByRy = {
    0.298912, 0.586603, 0.114485,
   -0.298912,-0.586603, 0.885515,
    0.701088,-0.586603,-0.114485};


// YUV is defined with headroom and footroom (TV range),
// we need to limit the excursion to 16-235.
// LimitU = sclU/(1-0.114485); // scale factor, yields 0.555786025922879
// LimitV = sclU/(1-0.298912); // scale factor, yields 0.701990139247995
// Umax 0.435812284313725
// Vmax 0.615857694117647
// This is still R'G'B' full to YUV full though
vec3 r601_YUV(vec3 RGB) {

    float sclU = ((0.5*(235-16)+16)/255.); // This yields Luma   grey  at around 0.49216 or 125.5 in 8-bit
    float sclV =       (240-16)    /255. ; // This yields Chroma range at around 0.87843 or 224   in 8-bit

    mat3 conv_mat = mat3(
    0.298912,      0.586603,      0.114485,
    sclU*YByRy[3], sclU*YByRy[4], sclU*YByRy[5],
    sclV*YByRy[6], sclV*YByRy[7], sclV*YByRy[8]);

// -0.147111592156863  -0.288700692156863   0.435812284313725
//  0.615857694117647  -0.515290478431373  -0.100567215686275
    return RGB.rgb * conv_mat;
 }


vec3 YUV_r601(vec3 YUV) {

    mat3 conv_mat = mat3(
    1.0000000, -0.000000029378826483,  1.1383928060531616,
    1.0000000, -0.396552562713623050, -0.5800843834877014,
    1.0000000,  2.031872510910034000,  0.0000000000000000);

    return YUV.xyz * conv_mat;
 }

//------------------------- LMS --------------------------


// Hunt-Pointer-Estevez D65 cone response
// modification for IPT model
const mat3 LMS =
mat3(
 0.4002, 0.7075, -0.0807,
-0.2280, 1.1500,  0.0612,
 0.0000, 0.0000,  0.9184);

const mat3 IPT =
mat3(
 0.4000,  0.4000, 0.2000,
 4.4550, -4.8510, 0.3960,
 0.8056, 0.3572, -1.1628);



//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/


////// STANDARDS ///////
// SMPTE RP 145-1994 (SMPTE-C), 170M-1999
// SMPTE-C - Standard Phosphor (Rec.601 NTSC)
// ILLUMINANT: D65->[0.31266142,0.3289589]
const mat3 SMPTE170M_ph =
    mat3(
     0.630, 0.340, 0.030,
     0.310, 0.595, 0.095,
     0.155, 0.070, 0.775);

// ITU-R BT.470/601 (B/G)
// EBU Tech.3213 PAL - Standard Phosphor for Studio Monitors
// ILLUMINANT: D65->[0.31266142,0.3289589]
const mat3 SMPTE470BG_ph =
    mat3(
     0.640, 0.330, 0.030,
     0.290, 0.600, 0.110,
     0.150, 0.060, 0.790);

// NTSC-J P22
// ILLUMINANT: [0.281000,0.311000] (CCT of 8945.436K)
// Mix between averaging KV-20M20, KDS VS19, Dell D93 and 4-TR-B09v1_0.pdf and Phosphor Handbook 'P22'
const mat3 P22_J_ph =
    mat3(
    0.625, 0.350, 0.025,
    0.280, 0.605, 0.115,
    0.152, 0.062, 0.786);


////// P22 ///////
// You can run any of these primaries either through D65 or D93 indistinctly but typically these were D65 based.
// ILLUMINANT: D65->[0.31266142,0.3289589]
const mat3 P22_80s_ph =
    mat3(
     0.6470, 0.3430, 0.0100,
     0.2820, 0.6200, 0.0980,
     0.1472, 0.0642, 0.7886);

// P22 meta measurement (Use this for NTSC-U 16-bits, and above 1979-1994 for 8-bits)
const mat3 P22_90s_ph =
    mat3(
     0.6661, 0.3329, 0.0010,
     0.3134, 0.6310, 0.0556,
     0.1472, 0.0642, 0.7886);

// CRT for Projection Tubes for NTSC-U late 90s, early 00s
const mat3 CRT_95s_ph =
    mat3(
     0.640, 0.335, 0.025,
     0.341, 0.586, 0.073,
     0.150, 0.070, 0.780);



//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/


// sRGB (IEC 61966-2-1) and ITU-R BT.709-6 (originally CCIR Rec.709)
const mat3 sRGB_prims =
    mat3(
    0.640, 0.330, 0.030,
    0.300, 0.600, 0.100,
    0.150, 0.060, 0.790);

// Adobe RGB (1998)
const mat3 Adobe_prims =
    mat3(
    0.640, 0.330, 0.030,
    0.210, 0.710, 0.080,
    0.150, 0.060, 0.790);

// BT-2020/BT-2100 (from 630nm, 532nm and 467nm)
const mat3 rec2020_prims =
    mat3(
    0.707917792, 0.292027109, 0.000055099,
    0.170237195, 0.796518542, 0.033244263,
    0.131370635, 0.045875976, 0.822753389);

// SMPTE RP 432-2 (DCI-P3)
const mat3 DCIP3_prims =
    mat3(
    0.680, 0.320, 0.000,
    0.265, 0.690, 0.045,
    0.150, 0.060, 0.790);


//*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/





void main()
{

// Retro Sega Systems: Genesis, 32x, CD and Saturn 2D had color palettes designed in TV levels to save on transformations.
    float lum_exp = (lum_fix ==  1.0) ? (255./239.) : 1.;

    vec3 src = texture(Source, vTexCoord.xy).rgb * lum_exp;

// Clipping Logic / Gamut Limiting
    vec2 UVmax = vec2(0.435812284313725, 0.615857694117647);

// Assumes framebuffer in Rec.601 full range with baked gamma
    vec3 col = clamp(r601_YUV(src), vec3(0.0, -UVmax.x, -UVmax.y), vec3(1.0, UVmax.x, UVmax.y));

    col      = crtgamut < 2.0 ? PCtoTV(col, 1.0, UVmax.x, UVmax.y, 1.0, false) : col;


// YUV Analogue Color Controls (HUE + Color Shift + Color Burst)
    float hue_radians = hue_degrees * M_PI;
    float hue = atan(col.z, col.y) + hue_radians;
    float chroma = sqrt(col.z * col.z + col.y * col.y);
    col   = vec3(col.x, chroma * cos(hue), chroma * sin(hue));

    col.y = (mod((col.y + 1.0) + U_SHIFT, 2.0) - 1.0) * U_MUL;
    col.z = (mod((col.z + 1.0) + V_SHIFT, 2.0) - 1.0) * V_MUL;

// Back to RGB
    col   = crtgamut < 2.0 ? TVtoPC(col, 1.0, UVmax.x, UVmax.y, 1.0, false) : col;
    col   = clamp(YUV_r601(col), 0., 1.);

// Look LUT - (in SPC space)
    float red =   (col.r * (global.LUT_Size1 - 1.0) + 0.4999) / (global.LUT_Size1 * global.LUT_Size1);
    float green = (col.g * (global.LUT_Size1 - 1.0) + 0.4999) /  global.LUT_Size1;
    float blue1 = (floor(col.b * (global.LUT_Size1 - 1.0)) / global.LUT_Size1) + red;
    float blue2 =  (ceil(col.b * (global.LUT_Size1 - 1.0)) / global.LUT_Size1) + red;
    float mixer = clamp(max((col.b - blue1) / (blue2 - blue1), 0.0), 0.0, 32.0);
    vec3 color1 = texture(SamplerLUT1, vec2(blue1, green)).rgb;
    vec3 color2 = texture(SamplerLUT1, vec2(blue2, green)).rgb;
    vec3 vcolor = (global.LUT1_toggle == 0.0) ? col : mixfix(color1, color2, mixer);


// To Linear. Undo developer baked CRT gamma (from 2.40 at default 0.1 CRT black level, to 2.61 at 0.0 CRT black level)
    col = EOTF_1886a_f3(vcolor, params.g_CRT_l, params.g_CRT_b, params.g_CRT_c);


// CRT Phosphor Gamut (0.0 is noop)
    mat3 m_in;

    if (crtgamut == -3.0) { m_in = SMPTE170M_ph;         } else
    if (crtgamut == -2.0) { m_in = CRT_95s_ph;           } else
    if (crtgamut == -1.0) { m_in = P22_80s_ph;           } else
    if (crtgamut ==  1.0) { m_in = P22_90s_ph;           } else
    if (crtgamut ==  2.0) { m_in = P22_J_ph;             } else
    if (crtgamut ==  3.0) { m_in = SMPTE470BG_ph;        }

// White Point Mapping
    col = clamp(wp_adjust(col, global.wp_temperature, m_in, crtgamut==2.0?1:0), 0., 1.);



//_   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \



// Display color space
    mat3 m_ou;

    if (SPC ==  1.0) { m_ou = DCIP3_prims;           } else
    if (SPC ==  2.0) { m_ou = rec2020_prims;         } else
    if (SPC ==  3.0) { m_ou = Adobe_prims;           } else
                     { m_ou = sRGB_prims;            }

// Sigmoidal Luma Contrast under 'Yxy' decorrelated model (in gamma space)
    vec3 Yxy = XYZtoYxy(col);
    float toGamma = clamp(moncurve_r(Yxy.r, 2.40, 0.055), 0., 1.);
    toGamma = (Yxy.r > 0.5) ? contrast_sigmoid_inv(toGamma, 2.3, 0.5) : toGamma;
    float sigmoid = (cntrst > 0.0) ? contrast_sigmoid(toGamma, cntrst, mid) : contrast_sigmoid_inv(toGamma, cntrst, mid);
    vec3 contrast = vec3(moncurve_f(sigmoid, 2.40, 0.055), Yxy.g, Yxy.b);
    vec3 XYZsrgb = clamp(XYZ_to_RGB(YxytoXYZ(contrast), m_ou, 0), 0., 1.);
    contrast = (cntrst == 0.0) ? XYZ_to_RGB(col, m_ou, 0) : XYZsrgb;


// Vignetting & Black Level
    vec2 vpos = vTexCoord*(global.OriginalSize.xy/global.SourceSize.xy);

    vpos *= 1.0 - vpos.xy;
    float vig = vpos.x * vpos.y * vstr;
    vig = min(pow(vig, vpower), 1.0);
    contrast *= (vignette == 1.0) ? vig : 1.0;

    contrast += (lift / 20.0) * (1.0 - contrast);


// RGB Related Transforms
    vec4 screen = vec4(max(contrast, 0.0), 1.0);
    float sat = g_sat + 1.0;

                   //  r    g    b  alpha ; alpha does nothing for our purposes
    mat4 color = mat4(wlr, rg,  rb,   0.0,              //red tint
                      gr,  wlg, gb,   0.0,              //green tint
                      br,  bg,  wlb,  0.0,              //blue tint
                      blr/20., blg/20., blb/20., 0.0);  //black tint


    vec3 coeff = (SPC == 3.0) ? vec3(0.29730707406997680, 0.62737262248992920, 0.07532038539648056) : \
                 (SPC == 2.0) ? vec3(0.26237168908119200, 0.67846411466598510, 0.05916426330804825) : \
                 (SPC == 1.0) ? vec3(0.22894050180912018, 0.69174379110336300, 0.07931561022996902) : \
                                vec3(0.21259990334510803, 0.71517896652221680, 0.07222118973731995) ;

    float msat  = 1.0 - sat;
    float msatx = msat * coeff.x;
    float msaty = msat * coeff.y;
    float msatz = msat * coeff.z;

    mat3 adjust = mat3(msatx + sat, msatx,       msatx,
                       msaty,       msaty + sat, msaty,
                       msatz,       msatz,       msatz + sat);


    screen = clamp(rolled_gain_v4(screen, clamp(lum, -0.49, 0.99)), 0., 1.);
    screen = color * screen;

//  HUE vs SAT (in IPT space)
    vec3 src_h = RGB_to_XYZ(screen.rgb, m_ou, 0) * LMS;
    src_h.x = src_h.x >= 0.0 ? pow(src_h.x, 0.43) : -pow(-src_h.x, 0.43);
    src_h.y = src_h.y >= 0.0 ? pow(src_h.y, 0.43) : -pow(-src_h.y, 0.43);
    src_h.z = src_h.z >= 0.0 ? pow(src_h.z, 0.43) : -pow(-src_h.z, 0.43);

    src_h.xyz *= IPT;

    float hue_at = atan(src_h.z, src_h.y);
    chroma = sqrt(src_h.z * src_h.z + src_h.y * src_h.y);

    float hue_radians_r = -20.0 * M_PI;
    float hue_r = chroma * cos(hue_at + hue_radians_r) * 2.;

    float hue_radians_g = 230.0 * M_PI;
    float hue_g = chroma * cos(hue_at + hue_radians_g) * 2.;

    float hue_radians_b = 100.0 * M_PI;
    float hue_b = chroma * cos(hue_at + hue_radians_b) * 2.;

    float msk = dot(clamp(vec3(hue_r, hue_g, hue_b), 0., 1.), vec3(-satr, -satg, -satb));
    src_h = mixfix(screen.rgb, vec3(dot(coeff, screen.rgb)), msk);

    float sat_msk = (vibr < 0.0) ? 1.0 - abs(SatMask(src_h.x, src_h.y, src_h.z) - 1.0) * abs(vibr) : \
                                   1.0 -    (SatMask(src_h.x, src_h.y, src_h.z) * vibr)            ;

    src_h = g_sat==0.0 && satr==0.0 && satg==0.0 && satb==0.0 && vibr==0.0 ? src_h : \
            mixfix(src_h, clamp(adjust * src_h, 0., 1.), clamp(sat_msk, 0., 1.));

// Dim to Dark adaptation OOTF; only for 709 and 2020
    vec3 src_D = global.g_Dim_to_Dark > 0.0 ? pow(src_h,vec3(1./0.9811)) : src_h;


// EOTF^-1 - Inverted Electro-Optical Transfer Function
    vec3 TRC = (SPC == 3.0) ?     clamp(pow(src_h,                  vec3(1./(563./256.))),    0., 1.) : \
               (SPC == 2.0) ? moncurve_r_f3(src_D,                           2.20 + 0.022222, 0.0993) : \
               (SPC == 1.0) ?     clamp(pow(src_h,                  vec3(1./(2.20 + 0.40))),  0., 1.) : \
               (SPC == 0.0) ? moncurve_r_f3(src_h,                           2.20 + 0.20,     0.0550) : \
                              clamp(pow(    src_D,                  vec3(1./(2.20 + 0.20))),  0., 1.) ;


// Technical LUT - (in SPC space)
    float red_2 =   (TRC.r * (global.LUT_Size2 - 1.0) + 0.4999) / (global.LUT_Size2 * global.LUT_Size2);
    float green_2 = (TRC.g * (global.LUT_Size2 - 1.0) + 0.4999) / global.LUT_Size2;
    float blue1_2 = (floor(TRC.b * (global.LUT_Size2 - 1.0)) / global.LUT_Size2) + red_2;
    float blue2_2 =  (ceil(TRC.b * (global.LUT_Size2 - 1.0)) / global.LUT_Size2) + red_2;
    float mixer_2 = clamp(max((TRC.b - blue1_2) / (blue2_2 - blue1_2), 0.0), 0.0, 32.0);
    vec3 color1_2 = texture(SamplerLUT2, vec2(blue1_2, green_2)).rgb;
    vec3 color2_2 = texture(SamplerLUT2, vec2(blue2_2, green_2)).rgb;
    vec3 LUT2_output = mixfix(color1_2, color2_2, mixer_2);

    LUT2_output = (global.LUT2_toggle == 0.0) ? TRC : LUT2_output;


    FragColor = vec4(LUT2_output, 1.0);
}