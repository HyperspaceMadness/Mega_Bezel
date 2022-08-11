layout(std140, set = 0, binding = 0) uniform UBO
{
	mat4 MVP;
   OutputSize;
} global;


#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main()
{
   gl_Position = global.MVP * Position;
   vTexCoord = TexCoord;
}

// --- hexamaze https://www.shadertoy.com/view/ll2cRR
float v; vec3 V;

#define f(U)  (                                                   \
    V =  ( ( (U) -9.*global.FrameCount / 60 ) * mat2(cos(.1 + vec4(0,11,33,0))) )  \
        * mat3x2( 12, -7, 0, 14,    12, 7 ) / 1e2 ,               \
    vec4( fract(  V[ 1 + int( 2.* sin( 1e5* length( ceil( V.xy )))) ] )  < .1 )\
)

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

void main()
{


// variant of https://shadertoy.com/view/WtjBzV
// adapted from https://shadertoy.com/view/3ljfRG

#define R  global.OutputSize.xy

#define N 5      // pixel oversampling NxN ( only where necessary )
#define eps 1e-2 // similarity threshold, is case of continuous mask

//#define T(U,l)          texelFetch(iChannel0, ivec2(U)>>l, l)
#define T(U,l)          textureLod(Source, (U)/R, float(l-1) )
// #define keyToggle(a)  ( texelFetch(iChannel3,ivec2(a,2),0).x > 0.)



   vec2 U = vTexCoord;
    O -= O;
    float n = float(N),
          m = T( U, 2 ).w,             // check neighborhood. Here: LOD #2 of mask
         //  s = iMouse.z>0. ? iMouse.x : R.x/2.;

    if ( U.x > 0.5
      // && m !=0. && m!= 1.) {        // neighborhood does not agree
         && abs(m-T(U,0).w) > eps ) {  // why ? otherwise, inside don't work in fullscreen
 // if ( true ) {                      // for bench: always oversample
        for (int k=0; k<N*N; k++) {    // --- oversampling
            vec2 D = ( vec2(k%N,k/N) - float(N-1)/2. ) / n; // subpixel
            O += f( U +  D ).w;
        }
        O /= n*n;
    }
    else                               // LOD#0 is ok
      //   O = keyToggle(32)              // SPACE: show red/blue where not oversampled
      //   ? T( U, 0 ).w == 1. ? vec4(1,0,0,1) : vec4(0,0,1,1)
      //   : T, 0 ( U).wwww;
        0 = ( U).wwww;
      
    if ( abs(U.x-0.5) < 1. ) O = vec4(1,0,0,1); // red separator
    O = pow( O, vec4(1./2.2) );        // to sRGB

   FragColor = vec4(O, 1.0);
}