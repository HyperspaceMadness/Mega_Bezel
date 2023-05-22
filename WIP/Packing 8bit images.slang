Packing 8bit images


	vec4 a = layers_under_precomp;
	vec4 b = layers_over_precomp;
	// vec4 packed = a + b * 256.0;// + c / 65536.0 + d / 16777216.0;
	vec4 packed = round(a * 256) + round(b * 256) * 256;

	vec4 new_a = fract(packed / 256.0);
	vec4 new_b = fract(packed / 65536.0);