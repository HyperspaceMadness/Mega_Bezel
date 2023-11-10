#version 450

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

// Copyright (c) 2015, bacondither
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer
//    in this position and unchanged.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

// Adaptive sharpen - version 2015-05-15 - (requires ps >= 3.0)
// Tuned for use post resize, EXPECTS FULL RANGE GAMMA LIGHT



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

void main()
{
	vec2	tex	=	vTexCoord;
	
	float	px	=	params.SourceSize.z;
	float	py	=	params.SourceSize.w;

// Get points and saturate out of range values (BTB & WTW)
// [                                  ]
// [                                  ]
// [          c1,  c2,  c3,           ]
// [          c4,  c0,  c5,           ]
// [          c6,  c7,  c8,           ]
// [                                  ]
// [                                  ]
	vec3	 c4 	=	clamp( texture(Source, vTexCoord + vec2(  -px,   0)).rgb, 0.0, 1.0);
	vec3	 c2 	=	clamp( texture(Source, vTexCoord + vec2(   0,   -py)).rgb, 0.0, 1.0);
	vec3	 c0 	=	clamp( texture(Source, vTexCoord).rgb, 0.0, 1.0);
	vec3	 c7 	=	clamp( texture(Source, vTexCoord + vec2(   0,    py)).rgb, 0.0, 1.0);
	vec3	 c5 	=	clamp( texture(Source, vTexCoord + vec2(   px,   0)).rgb, 0.0, 1.0);
	
//  conv: 0 4 0, 1 4 1, 0 4 0
	vec3	blur	=	(4*c2 + c4 + c5 + 4*c7 + 4.*c0)/12.;

   FragColor = vec4(blur, 1.0);
}