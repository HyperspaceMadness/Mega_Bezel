// /*
// Shader to test maximum use of resources, e.g. textures, parameters & passes
// */

#pragma parameter param_num_0	 	"param_num_0" 1 0 4 0.1
#pragma parameter param_num_1 	 	"param_num_1" 1 0 4 0.1
#pragma parameter param_num_2 	 	"param_num_2" 1 0 4 0.1
#pragma parameter param_num_3 	 	"param_num_3" 1 0 4 0.1
#pragma parameter param_num_4 	 	"param_num_4" 1 0 4 0.1
#pragma parameter param_num_5 	 	"param_num_5" 1 0 4 0.1
#pragma parameter param_num_6 	 	"param_num_6" 1 0 4 0.1
#pragma parameter param_num_7 	 	"param_num_7" 1 0 4 0.1
#pragma parameter param_num_8 	 	"param_num_8" 1 0 4 0.1
#pragma parameter param_num_9 	 	"param_num_9" 1 0 4 0.1

#pragma parameter param_num_10	 	"param_num_10" 1 0 4 0.1
#pragma parameter param_num_11 	 	"param_num_11" 1 0 4 0.1
#pragma parameter param_num_12 	 	"param_num_12" 1 0 4 0.1
#pragma parameter param_num_13 	 	"param_num_13" 1 0 4 0.1
#pragma parameter param_num_14 	 	"param_num_14" 1 0 4 0.1
#pragma parameter param_num_15 	 	"param_num_15" 1 0 4 0.1
#pragma parameter param_num_16 	 	"param_num_16" 1 0 4 0.1
#pragma parameter param_num_17 	 	"param_num_17" 1 0 4 0.1
#pragma parameter param_num_18 	 	"param_num_18" 1 0 4 0.1
#pragma parameter param_num_19 	 	"param_num_19" 1 0 4 0.1

#pragma parameter param_num_20	 	"param_num_20" 1 0 4 0.1
#pragma parameter param_num_21 	 	"param_num_21" 1 0 4 0.1
#pragma parameter param_num_22 	 	"param_num_22" 1 0 4 0.1
#pragma parameter param_num_23 	 	"param_num_23" 1 0 4 0.1
#pragma parameter param_num_24 	 	"param_num_24" 1 0 4 0.1
#pragma parameter param_num_25 	 	"param_num_25" 1 0 4 0.1
#pragma parameter param_num_26 	 	"param_num_26" 1 0 4 0.1
#pragma parameter param_num_27 	 	"param_num_27" 1 0 4 0.1
#pragma parameter param_num_28 	 	"param_num_28" 1 0 4 0.1
#pragma parameter param_num_29 	 	"param_num_29" 1 0 4 0.1

#pragma parameter param_num_30	 	"param_num_30" 1 0 4 0.1
#pragma parameter param_num_31 	 	"param_num_31" 1 0 4 0.1
#pragma parameter param_num_32 	 	"param_num_32" 1 0 4 0.1
#pragma parameter param_num_33 	 	"param_num_33" 1 0 4 0.1
#pragma parameter param_num_34 	 	"param_num_34" 1 0 4 0.1
#pragma parameter param_num_35 	 	"param_num_35" 1 0 4 0.1
#pragma parameter param_num_36 	 	"param_num_36" 1 0 4 0.1
#pragma parameter param_num_37 	 	"param_num_37" 1 0 4 0.1
#pragma parameter param_num_38 	 	"param_num_38" 1 0 4 0.1
#pragma parameter param_num_39 	 	"param_num_39" 1 0 4 0.1

#pragma parameter param_num_40	 	"param_num_40" 1 0 4 0.1
#pragma parameter param_num_41 	 	"param_num_41" 1 0 4 0.1
#pragma parameter param_num_42 	 	"param_num_42" 1 0 4 0.1
#pragma parameter param_num_43 	 	"param_num_43" 1 0 4 0.1
#pragma parameter param_num_44 	 	"param_num_44" 1 0 4 0.1
#pragma parameter param_num_45 	 	"param_num_45" 1 0 4 0.1
#pragma parameter param_num_46 	 	"param_num_46" 1 0 4 0.1
#pragma parameter param_num_47 	 	"param_num_47" 1 0 4 0.1
#pragma parameter param_num_48 	 	"param_num_48" 1 0 4 0.1
#pragma parameter param_num_49 	 	"param_num_49" 1 0 4 0.1

#pragma parameter param_num_50	 	"param_num_50" 1 0 4 0.1
#pragma parameter param_num_51 	 	"param_num_51" 1 0 4 0.1
#pragma parameter param_num_52 	 	"param_num_52" 1 0 4 0.1
#pragma parameter param_num_53 	 	"param_num_53" 1 0 4 0.1
#pragma parameter param_num_54 	 	"param_num_54" 1 0 4 0.1
#pragma parameter param_num_55 	 	"param_num_55" 1 0 4 0.1
#pragma parameter param_num_56 	 	"param_num_56" 1 0 4 0.1
#pragma parameter param_num_57 	 	"param_num_57" 1 0 4 0.1
#pragma parameter param_num_58 	 	"param_num_58" 1 0 4 0.1
#pragma parameter param_num_59 	 	"param_num_59" 1 0 4 0.1

#pragma parameter param_num_60	 	"param_num_60" 1 0 4 0.1
#pragma parameter param_num_61 	 	"param_num_61" 1 0 4 0.1
#pragma parameter param_num_62 	 	"param_num_62" 1 0 4 0.1
#pragma parameter param_num_63 	 	"param_num_63" 1 0 4 0.1
#pragma parameter param_num_64 	 	"param_num_64" 1 0 4 0.1
#pragma parameter param_num_65 	 	"param_num_65" 1 0 4 0.1
#pragma parameter param_num_66 	 	"param_num_66" 1 0 4 0.1
#pragma parameter param_num_67 	 	"param_num_67" 1 0 4 0.1
#pragma parameter param_num_68 	 	"param_num_68" 1 0 4 0.1
#pragma parameter param_num_69 	 	"param_num_69" 1 0 4 0.1

#pragma parameter param_num_70	 	"param_num_70" 1 0 4 0.1
#pragma parameter param_num_71 	 	"param_num_71" 1 0 4 0.1
#pragma parameter param_num_72 	 	"param_num_72" 1 0 4 0.1
#pragma parameter param_num_73 	 	"param_num_73" 1 0 4 0.1
#pragma parameter param_num_74 	 	"param_num_74" 1 0 4 0.1
#pragma parameter param_num_75 	 	"param_num_75" 1 0 4 0.1
#pragma parameter param_num_76 	 	"param_num_76" 1 0 4 0.1
#pragma parameter param_num_77 	 	"param_num_77" 1 0 4 0.1
#pragma parameter param_num_78 	 	"param_num_78" 1 0 4 0.1
#pragma parameter param_num_79 	 	"param_num_79" 1 0 4 0.1

#pragma parameter param_num_80	 	"param_num_80" 1 0 4 0.1
#pragma parameter param_num_81 	 	"param_num_81" 1 0 4 0.1
#pragma parameter param_num_82 	 	"param_num_82" 1 0 4 0.1
#pragma parameter param_num_83 	 	"param_num_83" 1 0 4 0.1
#pragma parameter param_num_84 	 	"param_num_84" 1 0 4 0.1
#pragma parameter param_num_85 	 	"param_num_85" 1 0 4 0.1
#pragma parameter param_num_86 	 	"param_num_86" 1 0 4 0.1
#pragma parameter param_num_87 	 	"param_num_87" 1 0 4 0.1
#pragma parameter param_num_88 	 	"param_num_88" 1 0 4 0.1
#pragma parameter param_num_89 	 	"param_num_89" 1 0 4 0.1

#pragma parameter param_num_90	 	"param_num_90" 1 0 4 0.1
#pragma parameter param_num_91 	 	"param_num_91" 1 0 4 0.1
#pragma parameter param_num_92 	 	"param_num_92" 1 0 4 0.1
#pragma parameter param_num_93 	 	"param_num_93" 1 0 4 0.1
#pragma parameter param_num_94 	 	"param_num_94" 1 0 4 0.1
#pragma parameter param_num_95 	 	"param_num_95" 1 0 4 0.1
#pragma parameter param_num_96 	 	"param_num_96" 1 0 4 0.1
#pragma parameter param_num_97 	 	"param_num_97" 1 0 4 0.1
#pragma parameter param_num_98 	 	"param_num_98" 1 0 4 0.1
#pragma parameter param_num_99 	 	"param_num_99" 1 0 4 0.1

#pragma parameter param_num_100	 	"param_num_100" 1 0 4 0.1
#pragma parameter param_num_101 	 	"param_num_101" 1 0 4 0.1
#pragma parameter param_num_102 	 	"param_num_102" 1 0 4 0.1
#pragma parameter param_num_103 	 	"param_num_103" 1 0 4 0.1
#pragma parameter param_num_104 	 	"param_num_104" 1 0 4 0.1
#pragma parameter param_num_105 	 	"param_num_105" 1 0 4 0.1
#pragma parameter param_num_106 	 	"param_num_106" 1 0 4 0.1
#pragma parameter param_num_107 	 	"param_num_107" 1 0 4 0.1
#pragma parameter param_num_108 	 	"param_num_108" 1 0 4 0.1
#pragma parameter param_num_109 	 	"param_num_109" 1 0 4 0.1

#pragma parameter param_num_110	 	"param_num_110" 1 0 4 0.1
#pragma parameter param_num_111 	 	"param_num_111" 1 0 4 0.1
#pragma parameter param_num_112 	 	"param_num_112" 1 0 4 0.1
#pragma parameter param_num_113 	 	"param_num_113" 1 0 4 0.1
#pragma parameter param_num_114 	 	"param_num_114" 1 0 4 0.1
#pragma parameter param_num_115 	 	"param_num_115" 1 0 4 0.1
#pragma parameter param_num_116 	 	"param_num_116" 1 0 4 0.1
#pragma parameter param_num_117 	 	"param_num_117" 1 0 4 0.1
#pragma parameter param_num_118 	 	"param_num_118" 1 0 4 0.1
#pragma parameter param_num_119 	 	"param_num_119" 1 0 4 0.1

#pragma parameter param_num_120	 	"param_num_120" 1 0 4 0.1
#pragma parameter param_num_121 	 	"param_num_121" 1 0 4 0.1
#pragma parameter param_num_122 	 	"param_num_122" 1 0 4 0.1
#pragma parameter param_num_123 	 	"param_num_123" 1 0 4 0.1
#pragma parameter param_num_124 	 	"param_num_124" 1 0 4 0.1
#pragma parameter param_num_125 	 	"param_num_125" 1 0 4 0.1
#pragma parameter param_num_126 	 	"param_num_126" 1 0 4 0.1
#pragma parameter param_num_127 	 	"param_num_127" 1 0 4 0.1
#pragma parameter param_num_128 	 	"param_num_128" 1 0 4 0.1
#pragma parameter param_num_129 	 	"param_num_129" 1 0 4 0.1

#pragma parameter param_num_130	 	"param_num_130" 1 0 4 0.1
#pragma parameter param_num_131 	 	"param_num_131" 1 0 4 0.1
#pragma parameter param_num_132 	 	"param_num_132" 1 0 4 0.1
#pragma parameter param_num_133 	 	"param_num_133" 1 0 4 0.1
#pragma parameter param_num_134 	 	"param_num_134" 1 0 4 0.1
#pragma parameter param_num_135 	 	"param_num_135" 1 0 4 0.1
#pragma parameter param_num_136 	 	"param_num_136" 1 0 4 0.1
#pragma parameter param_num_137 	 	"param_num_137" 1 0 4 0.1
#pragma parameter param_num_138 	 	"param_num_138" 1 0 4 0.1
#pragma parameter param_num_139 	 	"param_num_139" 1 0 4 0.1

#pragma parameter param_num_140	 	"param_num_140" 1 0 4 0.1
#pragma parameter param_num_141 	 	"param_num_141" 1 0 4 0.1
#pragma parameter param_num_142 	 	"param_num_142" 1 0 4 0.1
#pragma parameter param_num_143 	 	"param_num_143" 1 0 4 0.1
#pragma parameter param_num_144 	 	"param_num_144" 1 0 4 0.1
#pragma parameter param_num_145 	 	"param_num_145" 1 0 4 0.1
#pragma parameter param_num_146 	 	"param_num_146" 1 0 4 0.1
#pragma parameter param_num_147 	 	"param_num_147" 1 0 4 0.1
#pragma parameter param_num_148 	 	"param_num_148" 1 0 4 0.1
#pragma parameter param_num_149 	 	"param_num_149" 1 0 4 0.1

#pragma parameter param_num_150	 	"param_num_150" 1 0 4 0.1
#pragma parameter param_num_151 	 	"param_num_151" 1 0 4 0.1
#pragma parameter param_num_152 	 	"param_num_152" 1 0 4 0.1
#pragma parameter param_num_153 	 	"param_num_153" 1 0 4 0.1
#pragma parameter param_num_154 	 	"param_num_154" 1 0 4 0.1
#pragma parameter param_num_155 	 	"param_num_155" 1 0 4 0.1
#pragma parameter param_num_156 	 	"param_num_156" 1 0 4 0.1
#pragma parameter param_num_157 	 	"param_num_157" 1 0 4 0.1
#pragma parameter param_num_158 	 	"param_num_158" 1 0 4 0.1
#pragma parameter param_num_159 	 	"param_num_159" 1 0 4 0.1

#pragma parameter param_num_160	 	"param_num_160" 1 0 4 0.1
#pragma parameter param_num_161 	 	"param_num_161" 1 0 4 0.1
#pragma parameter param_num_162 	 	"param_num_162" 1 0 4 0.1
#pragma parameter param_num_163 	 	"param_num_163" 1 0 4 0.1
#pragma parameter param_num_164 	 	"param_num_164" 1 0 4 0.1
#pragma parameter param_num_165 	 	"param_num_165" 1 0 4 0.1
#pragma parameter param_num_166 	 	"param_num_166" 1 0 4 0.1
#pragma parameter param_num_167 	 	"param_num_167" 1 0 4 0.1
#pragma parameter param_num_168 	 	"param_num_168" 1 0 4 0.1
#pragma parameter param_num_169 	 	"param_num_169" 1 0 4 0.1

#pragma parameter param_num_170	 	"param_num_170" 1 0 4 0.1
#pragma parameter param_num_171 	 	"param_num_171" 1 0 4 0.1
#pragma parameter param_num_172 	 	"param_num_172" 1 0 4 0.1
#pragma parameter param_num_173 	 	"param_num_173" 1 0 4 0.1
#pragma parameter param_num_174 	 	"param_num_174" 1 0 4 0.1
#pragma parameter param_num_175 	 	"param_num_175" 1 0 4 0.1
#pragma parameter param_num_176 	 	"param_num_176" 1 0 4 0.1
#pragma parameter param_num_177 	 	"param_num_177" 1 0 4 0.1
#pragma parameter param_num_178 	 	"param_num_178" 1 0 4 0.1
#pragma parameter param_num_179 	 	"param_num_179" 1 0 4 0.1

#pragma parameter param_num_180	 	"param_num_180" 1 0 4 0.1
#pragma parameter param_num_181 	 	"param_num_181" 1 0 4 0.1
#pragma parameter param_num_182 	 	"param_num_182" 1 0 4 0.1
#pragma parameter param_num_183 	 	"param_num_183" 1 0 4 0.1
#pragma parameter param_num_184 	 	"param_num_184" 1 0 4 0.1
#pragma parameter param_num_185 	 	"param_num_185" 1 0 4 0.1
#pragma parameter param_num_186 	 	"param_num_186" 1 0 4 0.1
#pragma parameter param_num_187 	 	"param_num_187" 1 0 4 0.1
#pragma parameter param_num_188 	 	"param_num_188" 1 0 4 0.1
#pragma parameter param_num_189 	 	"param_num_189" 1 0 4 0.1

#pragma parameter param_num_190	 	"param_num_190" 1 0 4 0.1
#pragma parameter param_num_191 	 	"param_num_191" 1 0 4 0.1
#pragma parameter param_num_192 	 	"param_num_192" 1 0 4 0.1
#pragma parameter param_num_193 	 	"param_num_193" 1 0 4 0.1
#pragma parameter param_num_194 	 	"param_num_194" 1 0 4 0.1
#pragma parameter param_num_195 	 	"param_num_195" 1 0 4 0.1
#pragma parameter param_num_196 	 	"param_num_196" 1 0 4 0.1
#pragma parameter param_num_197 	 	"param_num_197" 1 0 4 0.1
#pragma parameter param_num_198 	 	"param_num_198" 1 0 4 0.1
#pragma parameter param_num_199 	 	"param_num_199" 1 0 4 0.1

#pragma parameter param_num_200	 	"param_num_200" 1 0 4 0.1
#pragma parameter param_num_201 	 	"param_num_201" 1 0 4 0.1
#pragma parameter param_num_202 	 	"param_num_202" 1 0 4 0.1
#pragma parameter param_num_203 	 	"param_num_203" 1 0 4 0.1
#pragma parameter param_num_204 	 	"param_num_204" 1 0 4 0.1
#pragma parameter param_num_205 	 	"param_num_205" 1 0 4 0.1
#pragma parameter param_num_206 	 	"param_num_206" 1 0 4 0.1
#pragma parameter param_num_207 	 	"param_num_207" 1 0 4 0.1
#pragma parameter param_num_208 	 	"param_num_208" 1 0 4 0.1
#pragma parameter param_num_209 	 	"param_num_209" 1 0 4 0.1

#pragma parameter param_num_210	 	"param_num_210" 1 0 4 0.1
#pragma parameter param_num_211 	 	"param_num_211" 1 0 4 0.1
#pragma parameter param_num_212 	 	"param_num_212" 1 0 4 0.1
#pragma parameter param_num_213 	 	"param_num_213" 1 0 4 0.1
#pragma parameter param_num_214 	 	"param_num_214" 1 0 4 0.1
#pragma parameter param_num_215 	 	"param_num_215" 1 0 4 0.1
#pragma parameter param_num_216 	 	"param_num_216" 1 0 4 0.1
#pragma parameter param_num_217 	 	"param_num_217" 1 0 4 0.1
#pragma parameter param_num_218 	 	"param_num_218" 1 0 4 0.1
#pragma parameter param_num_219 	 	"param_num_219" 1 0 4 0.1

#pragma parameter param_num_220	 	"param_num_220" 1 0 4 0.1
#pragma parameter param_num_221 	 	"param_num_221" 1 0 4 0.1
#pragma parameter param_num_222 	 	"param_num_222" 1 0 4 0.1
#pragma parameter param_num_223 	 	"param_num_223" 1 0 4 0.1
#pragma parameter param_num_224 	 	"param_num_224" 1 0 4 0.1
#pragma parameter param_num_225 	 	"param_num_225" 1 0 4 0.1
#pragma parameter param_num_226 	 	"param_num_226" 1 0 4 0.1
#pragma parameter param_num_227 	 	"param_num_227" 1 0 4 0.1
#pragma parameter param_num_228 	 	"param_num_228" 1 0 4 0.1
#pragma parameter param_num_229 	 	"param_num_229" 1 0 4 0.1

#pragma parameter param_num_230	 	"param_num_230" 1 0 4 0.1
#pragma parameter param_num_231 	 	"param_num_231" 1 0 4 0.1
#pragma parameter param_num_232 	 	"param_num_232" 1 0 4 0.1
#pragma parameter param_num_233 	 	"param_num_233" 1 0 4 0.1
#pragma parameter param_num_234 	 	"param_num_234" 1 0 4 0.1
#pragma parameter param_num_235 	 	"param_num_235" 1 0 4 0.1
#pragma parameter param_num_236 	 	"param_num_236" 1 0 4 0.1
#pragma parameter param_num_237 	 	"param_num_237" 1 0 4 0.1
#pragma parameter param_num_238 	 	"param_num_238" 1 0 4 0.1
#pragma parameter param_num_239 	 	"param_num_239" 1 0 4 0.1

#pragma parameter param_num_240	 	"param_num_240" 1 0 4 0.1
#pragma parameter param_num_241 	 	"param_num_241" 1 0 4 0.1
#pragma parameter param_num_242 	 	"param_num_242" 1 0 4 0.1
#pragma parameter param_num_243 	 	"param_num_243" 1 0 4 0.1
#pragma parameter param_num_244 	 	"param_num_244" 1 0 4 0.1
#pragma parameter param_num_245 	 	"param_num_245" 1 0 4 0.1
#pragma parameter param_num_246 	 	"param_num_246" 1 0 4 0.1
#pragma parameter param_num_247 	 	"param_num_247" 1 0 4 0.1
#pragma parameter param_num_248 	 	"param_num_248" 1 0 4 0.1
#pragma parameter param_num_249 	 	"param_num_249" 1 0 4 0.1

#pragma parameter param_num_250	 	"param_num_250" 1 0 4 0.1
#pragma parameter param_num_251 	 	"param_num_251" 1 0 4 0.1
#pragma parameter param_num_252 	 	"param_num_252" 1 0 4 0.1
#pragma parameter param_num_253 	 	"param_num_253" 1 0 4 0.1
#pragma parameter param_num_254 	 	"param_num_254" 1 0 4 0.1
#pragma parameter param_num_255 	 	"param_num_255" 1 0 4 0.1






// Mirror image shader
// by hunterk
// license: too simple to license but let's call it
// public domain if that makes you feel better
 
// #pragma parameter MIRROR "Flip Horizontal" 1.0 0.0 1.0 1.0
 
#if defined(VERTEX)
 
#if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying
#define COMPAT_ATTRIBUTE attribute
#define COMPAT_TEXTURE texture2D
#endif
 
#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
 
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 COLOR;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 COL0;
COMPAT_VARYING vec4 TEX0;
 
vec4 _oPosition1;
uniform mat4 MVPMatrix;
uniform COMPAT_PRECISION int FrameDirection;
uniform COMPAT_PRECISION int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
 
// compatibility #defines
#define vTexCoord TEX0.xy
#define SourceSize vec4(TextureSize, 1.0 / TextureSize) //either TextureSize or InputSize
#define OutSize vec4(OutputSize, 1.0 / OutputSize)
 
#ifdef PARAMETER_UNIFORM
uniform COMPAT_PRECISION float param_num_0;
uniform COMPAT_PRECISION float param_num_1;
uniform COMPAT_PRECISION float param_num_2;
uniform COMPAT_PRECISION float param_num_3;
uniform COMPAT_PRECISION float param_num_4;
uniform COMPAT_PRECISION float param_num_5;
uniform COMPAT_PRECISION float param_num_6;
uniform COMPAT_PRECISION float param_num_7;
uniform COMPAT_PRECISION float param_num_8;
uniform COMPAT_PRECISION float param_num_9;

uniform COMPAT_PRECISION float param_num_10;
uniform COMPAT_PRECISION float param_num_11;
uniform COMPAT_PRECISION float param_num_12;
uniform COMPAT_PRECISION float param_num_13;
uniform COMPAT_PRECISION float param_num_14;
uniform COMPAT_PRECISION float param_num_15;
uniform COMPAT_PRECISION float param_num_16;
uniform COMPAT_PRECISION float param_num_17;
uniform COMPAT_PRECISION float param_num_18;
uniform COMPAT_PRECISION float param_num_19;

uniform COMPAT_PRECISION float param_num_20;
uniform COMPAT_PRECISION float param_num_21;
uniform COMPAT_PRECISION float param_num_22;
uniform COMPAT_PRECISION float param_num_23;
uniform COMPAT_PRECISION float param_num_24;
uniform COMPAT_PRECISION float param_num_25;
uniform COMPAT_PRECISION float param_num_26;
uniform COMPAT_PRECISION float param_num_27;
uniform COMPAT_PRECISION float param_num_28;
uniform COMPAT_PRECISION float param_num_29;

uniform COMPAT_PRECISION float param_num_30;
uniform COMPAT_PRECISION float param_num_31;
uniform COMPAT_PRECISION float param_num_32;
uniform COMPAT_PRECISION float param_num_33;
uniform COMPAT_PRECISION float param_num_34;
uniform COMPAT_PRECISION float param_num_35;
uniform COMPAT_PRECISION float param_num_36;
uniform COMPAT_PRECISION float param_num_37;
uniform COMPAT_PRECISION float param_num_38;
uniform COMPAT_PRECISION float param_num_39;

uniform COMPAT_PRECISION float param_num_40;
uniform COMPAT_PRECISION float param_num_41;
uniform COMPAT_PRECISION float param_num_42;
uniform COMPAT_PRECISION float param_num_43;
uniform COMPAT_PRECISION float param_num_44;
uniform COMPAT_PRECISION float param_num_45;
uniform COMPAT_PRECISION float param_num_46;
uniform COMPAT_PRECISION float param_num_47;
uniform COMPAT_PRECISION float param_num_48;
uniform COMPAT_PRECISION float param_num_49;

uniform COMPAT_PRECISION float param_num_50;
uniform COMPAT_PRECISION float param_num_51;
uniform COMPAT_PRECISION float param_num_52;
uniform COMPAT_PRECISION float param_num_53;
uniform COMPAT_PRECISION float param_num_54;
uniform COMPAT_PRECISION float param_num_55;
uniform COMPAT_PRECISION float param_num_56;
uniform COMPAT_PRECISION float param_num_57;
uniform COMPAT_PRECISION float param_num_58;
uniform COMPAT_PRECISION float param_num_59;

uniform COMPAT_PRECISION float param_num_60;
uniform COMPAT_PRECISION float param_num_61;
uniform COMPAT_PRECISION float param_num_62;
uniform COMPAT_PRECISION float param_num_63;
uniform COMPAT_PRECISION float param_num_64;
uniform COMPAT_PRECISION float param_num_65;
uniform COMPAT_PRECISION float param_num_66;
uniform COMPAT_PRECISION float param_num_67;
uniform COMPAT_PRECISION float param_num_68;
uniform COMPAT_PRECISION float param_num_69;

uniform COMPAT_PRECISION float param_num_70;
uniform COMPAT_PRECISION float param_num_71;
uniform COMPAT_PRECISION float param_num_72;
uniform COMPAT_PRECISION float param_num_73;
uniform COMPAT_PRECISION float param_num_74;
uniform COMPAT_PRECISION float param_num_75;
uniform COMPAT_PRECISION float param_num_76;
uniform COMPAT_PRECISION float param_num_77;
uniform COMPAT_PRECISION float param_num_78;
uniform COMPAT_PRECISION float param_num_79;

uniform COMPAT_PRECISION float param_num_80;
uniform COMPAT_PRECISION float param_num_81;
uniform COMPAT_PRECISION float param_num_82;
uniform COMPAT_PRECISION float param_num_83;
uniform COMPAT_PRECISION float param_num_84;
uniform COMPAT_PRECISION float param_num_85;
uniform COMPAT_PRECISION float param_num_86;
uniform COMPAT_PRECISION float param_num_87;
uniform COMPAT_PRECISION float param_num_88;
uniform COMPAT_PRECISION float param_num_89;

uniform COMPAT_PRECISION float param_num_90;
uniform COMPAT_PRECISION float param_num_91;
uniform COMPAT_PRECISION float param_num_92;
uniform COMPAT_PRECISION float param_num_93;
uniform COMPAT_PRECISION float param_num_94;
uniform COMPAT_PRECISION float param_num_95;
uniform COMPAT_PRECISION float param_num_96;
uniform COMPAT_PRECISION float param_num_97;
uniform COMPAT_PRECISION float param_num_98;
uniform COMPAT_PRECISION float param_num_99;

uniform COMPAT_PRECISION float param_num_100;
uniform COMPAT_PRECISION float param_num_101;
uniform COMPAT_PRECISION float param_num_102;
uniform COMPAT_PRECISION float param_num_103;
uniform COMPAT_PRECISION float param_num_104;
uniform COMPAT_PRECISION float param_num_105;
uniform COMPAT_PRECISION float param_num_106;
uniform COMPAT_PRECISION float param_num_107;
uniform COMPAT_PRECISION float param_num_108;
uniform COMPAT_PRECISION float param_num_109;

uniform COMPAT_PRECISION float param_num_110;
uniform COMPAT_PRECISION float param_num_111;
uniform COMPAT_PRECISION float param_num_112;
uniform COMPAT_PRECISION float param_num_113;
uniform COMPAT_PRECISION float param_num_114;
uniform COMPAT_PRECISION float param_num_115;
uniform COMPAT_PRECISION float param_num_116;
uniform COMPAT_PRECISION float param_num_117;
uniform COMPAT_PRECISION float param_num_118;
uniform COMPAT_PRECISION float param_num_119;

uniform COMPAT_PRECISION float param_num_120;
uniform COMPAT_PRECISION float param_num_121;
uniform COMPAT_PRECISION float param_num_122;
uniform COMPAT_PRECISION float param_num_123;
uniform COMPAT_PRECISION float param_num_124;
uniform COMPAT_PRECISION float param_num_125;
uniform COMPAT_PRECISION float param_num_126;
uniform COMPAT_PRECISION float param_num_127;
uniform COMPAT_PRECISION float param_num_128;
uniform COMPAT_PRECISION float param_num_129;

uniform COMPAT_PRECISION float param_num_130;
uniform COMPAT_PRECISION float param_num_131;
uniform COMPAT_PRECISION float param_num_132;
uniform COMPAT_PRECISION float param_num_133;
uniform COMPAT_PRECISION float param_num_134;
uniform COMPAT_PRECISION float param_num_135;
uniform COMPAT_PRECISION float param_num_136;
uniform COMPAT_PRECISION float param_num_137;
uniform COMPAT_PRECISION float param_num_138;
uniform COMPAT_PRECISION float param_num_139;

uniform COMPAT_PRECISION float param_num_140;
uniform COMPAT_PRECISION float param_num_141;
uniform COMPAT_PRECISION float param_num_142;
uniform COMPAT_PRECISION float param_num_143;
uniform COMPAT_PRECISION float param_num_144;
uniform COMPAT_PRECISION float param_num_145;
uniform COMPAT_PRECISION float param_num_146;
uniform COMPAT_PRECISION float param_num_147;
uniform COMPAT_PRECISION float param_num_148;
uniform COMPAT_PRECISION float param_num_149;

uniform COMPAT_PRECISION float param_num_150;
uniform COMPAT_PRECISION float param_num_151;
uniform COMPAT_PRECISION float param_num_152;
uniform COMPAT_PRECISION float param_num_153;
uniform COMPAT_PRECISION float param_num_154;
uniform COMPAT_PRECISION float param_num_155;
uniform COMPAT_PRECISION float param_num_156;
uniform COMPAT_PRECISION float param_num_157;
uniform COMPAT_PRECISION float param_num_158;
uniform COMPAT_PRECISION float param_num_159;

uniform COMPAT_PRECISION float param_num_160;
uniform COMPAT_PRECISION float param_num_161;
uniform COMPAT_PRECISION float param_num_162;
uniform COMPAT_PRECISION float param_num_163;
uniform COMPAT_PRECISION float param_num_164;
uniform COMPAT_PRECISION float param_num_165;
uniform COMPAT_PRECISION float param_num_166;
uniform COMPAT_PRECISION float param_num_167;
uniform COMPAT_PRECISION float param_num_168;
uniform COMPAT_PRECISION float param_num_169;

uniform COMPAT_PRECISION float param_num_170;
uniform COMPAT_PRECISION float param_num_171;
uniform COMPAT_PRECISION float param_num_172;
uniform COMPAT_PRECISION float param_num_173;
uniform COMPAT_PRECISION float param_num_174;
uniform COMPAT_PRECISION float param_num_175;
uniform COMPAT_PRECISION float param_num_176;
uniform COMPAT_PRECISION float param_num_177;
uniform COMPAT_PRECISION float param_num_178;
uniform COMPAT_PRECISION float param_num_179;

uniform COMPAT_PRECISION float param_num_180;
uniform COMPAT_PRECISION float param_num_181;
uniform COMPAT_PRECISION float param_num_182;
uniform COMPAT_PRECISION float param_num_183;
uniform COMPAT_PRECISION float param_num_184;
uniform COMPAT_PRECISION float param_num_185;
uniform COMPAT_PRECISION float param_num_186;
uniform COMPAT_PRECISION float param_num_187;
uniform COMPAT_PRECISION float param_num_188;
uniform COMPAT_PRECISION float param_num_189;

uniform COMPAT_PRECISION float param_num_190;
uniform COMPAT_PRECISION float param_num_191;
uniform COMPAT_PRECISION float param_num_192;
uniform COMPAT_PRECISION float param_num_193;
uniform COMPAT_PRECISION float param_num_194;
uniform COMPAT_PRECISION float param_num_195;
uniform COMPAT_PRECISION float param_num_196;
uniform COMPAT_PRECISION float param_num_197;
uniform COMPAT_PRECISION float param_num_198;
uniform COMPAT_PRECISION float param_num_199;

uniform COMPAT_PRECISION float param_num_200;
uniform COMPAT_PRECISION float param_num_201;
uniform COMPAT_PRECISION float param_num_202;
uniform COMPAT_PRECISION float param_num_203;
uniform COMPAT_PRECISION float param_num_204;
uniform COMPAT_PRECISION float param_num_205;
uniform COMPAT_PRECISION float param_num_206;
uniform COMPAT_PRECISION float param_num_207;
uniform COMPAT_PRECISION float param_num_208;
uniform COMPAT_PRECISION float param_num_209;

uniform COMPAT_PRECISION float param_num_210;
uniform COMPAT_PRECISION float param_num_211;
uniform COMPAT_PRECISION float param_num_212;
uniform COMPAT_PRECISION float param_num_213;
uniform COMPAT_PRECISION float param_num_214;
uniform COMPAT_PRECISION float param_num_215;
uniform COMPAT_PRECISION float param_num_216;
uniform COMPAT_PRECISION float param_num_217;
uniform COMPAT_PRECISION float param_num_218;
uniform COMPAT_PRECISION float param_num_219;

uniform COMPAT_PRECISION float param_num_220;
uniform COMPAT_PRECISION float param_num_221;
uniform COMPAT_PRECISION float param_num_222;
uniform COMPAT_PRECISION float param_num_223;
uniform COMPAT_PRECISION float param_num_224;
uniform COMPAT_PRECISION float param_num_225;
uniform COMPAT_PRECISION float param_num_226;
uniform COMPAT_PRECISION float param_num_227;
uniform COMPAT_PRECISION float param_num_228;
uniform COMPAT_PRECISION float param_num_229;

uniform COMPAT_PRECISION float param_num_230;
uniform COMPAT_PRECISION float param_num_231;
uniform COMPAT_PRECISION float param_num_232;
uniform COMPAT_PRECISION float param_num_233;
uniform COMPAT_PRECISION float param_num_234;
uniform COMPAT_PRECISION float param_num_235;
uniform COMPAT_PRECISION float param_num_236;
uniform COMPAT_PRECISION float param_num_237;
uniform COMPAT_PRECISION float param_num_238;
uniform COMPAT_PRECISION float param_num_239;

uniform COMPAT_PRECISION float param_num_240;
uniform COMPAT_PRECISION float param_num_241;
uniform COMPAT_PRECISION float param_num_242;
uniform COMPAT_PRECISION float param_num_243;
uniform COMPAT_PRECISION float param_num_244;
uniform COMPAT_PRECISION float param_num_245;
uniform COMPAT_PRECISION float param_num_246;
uniform COMPAT_PRECISION float param_num_247;
uniform COMPAT_PRECISION float param_num_248;
uniform COMPAT_PRECISION float param_num_249;

uniform COMPAT_PRECISION float param_num_250;
uniform COMPAT_PRECISION float param_num_251;
uniform COMPAT_PRECISION float param_num_252;
uniform COMPAT_PRECISION float param_num_253;
uniform COMPAT_PRECISION float param_num_254;
uniform COMPAT_PRECISION float param_num_255;

#else
#define param_num_0 1.0
#endif
 
void main()
{
    gl_Position = MVPMatrix * VertexCoord;
    // float offset = 0.5 * InputSize.x / TextureSize.x;
    TEX0.xy = TexCoord.xy;
    // TEX0.x = (param_num_0 > 0.5) ? (((TEX0.x - offset) * -1.0) + offset) : TEX0.x;
}
 
#elif defined(FRAGMENT)
 
#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
 
#if __VERSION__ >= 130
#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture
out COMPAT_PRECISION vec4 FragColor;
#else
#define COMPAT_VARYING varying
#define FragColor gl_FragColor
#define COMPAT_TEXTURE texture2D
#endif
 
uniform COMPAT_PRECISION int FrameDirection;
uniform COMPAT_PRECISION int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;

uniform sampler2D Texture;
uniform sampler2D PresetTex_1;
uniform sampler2D PresetTex_2;
uniform sampler2D PresetTex_3;
uniform sampler2D PresetTex_4;
uniform sampler2D PresetTex_5;
uniform sampler2D PresetTex_6;
uniform sampler2D PresetTex_7;
uniform sampler2D PresetTex_8;
uniform sampler2D PresetTex_9;
uniform sampler2D PresetTex_10;
uniform sampler2D PresetTex_11;
uniform sampler2D PresetTex_12;
uniform sampler2D PresetTex_13;
uniform sampler2D PresetTex_14;
uniform sampler2D PresetTex_15;
uniform sampler2D PresetTex_16;

COMPAT_VARYING vec4 TEX0;
 
// compatibility #defines
#define Source Texture
#define vTexCoord TEX0.xy
 
#define SourceSize vec4(TextureSize, 1.0 / TextureSize) //either TextureSize or InputSize
#define OutSize vec4(OutputSize, 1.0 / OutputSize)
 
void main()
{
    // FragColor = COMPAT_TEXTURE(Source, vTexCoord);

    float prev_pass_used_area = 0.9;

    // Squeeze the texture from the last pass into the top left quadrant
    if ((vTexCoord.x < prev_pass_used_area) && (vTexCoord.y < prev_pass_used_area))
    {
        FragColor = texture(Source, vTexCoord * (1 / prev_pass_used_area));
    }
    // Put the color from the preset textures in all other quadrants
    else 
    {
        vec2 check_coord = vec2(1);

        float num_textures = 16 + 5;

        FragColor = vec4(3);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        check_coord -= (1 - prev_pass_used_area) / num_textures;
        check_coord -= (1 - prev_pass_used_area) / num_textures;
        check_coord -= (1 - prev_pass_used_area) / num_textures;
        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_16, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_15, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_14, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_13, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_12, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_11, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_10, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_9, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_8, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_7, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_6, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_5, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_4, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_3, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_2, vTexCoord);

        check_coord -= (1 - prev_pass_used_area) / num_textures;
        if ((vTexCoord.x < check_coord.x) && (vTexCoord.y < check_coord.y)) FragColor = texture(PresetTex_1, vTexCoord);
    }
    FragColor = mix(FragColor, vec4(0), 0.07);
}
#endif
