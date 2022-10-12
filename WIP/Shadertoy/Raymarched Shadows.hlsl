// @author ciphered <https://ciphrd.com>
// @license MIT
//
// -----------
//
// This shader was made to be working with a vertical plane, parallel to the view screen,
// passing by (0;0;0). This constraint allows for great optimizations, because this shader 
// is intended to be working along texture manipulations to play with the surface.
//
// 2 parallel planes are defined in the space, in front of the camera, with a gap between
// them corresponding to the DEPTH parameter. For each ray casted from the camera, we first
// find the intersection between the ray and the first plane. From there, we start marching
// along the ray and sample the height map at each step to find if whether we're in the
// volume or not. Once we reach a point within the volume, we can find the point that belongs
// to the surface and light it accordingly by computing the height map gradient.
//
// This is basically a fancy 3d texture renderer
//
// If you change the input texture, adjust the DEPTH accordingly
//
// Keep in mind that the textures do not represent to height of the elements within, only
// the default one works correctly for this example
//
//
// LIMITATIONS
//
// I coudln't find a way to get soft shadows, if you do feel free to make me know in the comments
// This version does not provide a COLOR MAP, however it's pretty easy to color the surface using a
// color map, just copy the getHeightMapValue function to sample from a color map instead of a depthmap
//

#define PI 3.14159265359
#define MAX_ITERATIONS 200

// needs to be adujested to match the input texture
#define DEPTH 0.6

// the step size for the ray marching
#define PRECISION 0.006

// the lower this is, the more precise the normals get (too much precision results in the
// apparition of normal artifacts
#define EPSILON 0.002



// returns the intersection point between the ray and the hightest point on the plane
vec3 boundsSDF (vec3 ro, vec3 rd, float h) {
    // the z length of the target vector
    float dz = ro.z - h;
    float t = dz / rd.z;
    return vec3(
    	ro.x - rd.x * t,
        ro.y - rd.y * t,
        ro.z - rd.z * t
    );
}

// returns the height map value of any point in space
// this is done by projecting the point on the vertical plane
float getHeightMapValue (vec3 p) {
    // yes this is a projection
    vec2 tuv = p.xy;
    tuv.x+= iTime * .1;
    tuv = fract(tuv - .5);
    
    vec3 hm = texture(iChannel0, tuv).rgb;
    
    return .9 * (hm.r+hm.g+hm.b)/3. + 0.1;
}

vec3 calcNormal (vec3 p) {
	float hx1 = getHeightMapValue(vec3(p.x - EPSILON, p.y, 0.0)) * DEPTH;   
    float hx2 = getHeightMapValue(vec3(p.x + EPSILON, p.y, 0.0)) * DEPTH;
    vec3 pu = normalize(vec3(2.0 * EPSILON, 0.0, hx2 - hx1));
    
    float hy1 = getHeightMapValue(vec3(p.x, p.y, 0.0)) * DEPTH;
    float hy2 = getHeightMapValue(vec3(p.x, p.y + EPSILON, 0.0)) * DEPTH;
    vec3 pv = normalize(vec3(0.0, EPSILON, hy2 - hy1));
    
    return cross(pu, pv);
}

vec3 calcLight( in vec3 pos , in vec3 lightp, in vec3 lightc, in vec3 camdir, float spec)
{    
	vec3 normal = calcNormal(pos);
    vec3 lightdir = normalize(pos - lightp);
    float cosa = pow(0.5+0.5*dot(normal, -lightdir), 3.0);
    float cosr = max(dot(-camdir, reflect(lightdir, normal)), 0.0);
    
    vec3 ambiant = vec3(0.1);
    vec3 diffuse = vec3(0.5 * cosa);
    vec3 phong = vec3(0.1 * pow(cosr, spec)) * .4;
    
    return lightc * (ambiant + diffuse + phong);
}

vec4 raymarching (vec3 p, vec3 dir) {
    vec3 prev = p;
    vec3 curr = p;
    float lastD = p.z;
    float hmap = 0.;
    float df = 0.;
    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
    	prev = curr;
        curr = prev + dir * PRECISION;
        
        if (curr.z < 0.0) {
        	return vec4(0, 0, 1, 0);    
        }
        
        hmap = getHeightMapValue(curr);
        // distance to the displaced surface
        float df = curr.z - hmap * DEPTH;
        
        // if we have an intersection
        if (df < 0.0) {
        	// linear interpolation to find more precise df
            float t = lastD / (abs(df)+lastD);
            vec3 pi = prev + t * (curr - prev);
            float h = getHeightMapValue(pi);
            return vec4(pi, h);
        } else {
         	lastD = df;
        }
    }
    
    return vec4(0, 0, 1, 0);
}

// computes a sharp shadow for a given point p of the surface
float shadow (vec3 p, vec3 lpos) {
    vec3 dir = normalize(lpos - p);
    vec3 curr = p;
    float hmap;
    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
    	curr+= dir * PRECISION;
        
        if (curr.z < 0.0) {
            return 0.0;
        }
        
        hmap = getHeightMapValue(curr);
        
        // distance to the displaced surface
        float df = curr.z - hmap * DEPTH;
        
        if (df < 0.0) {
            return 1.0;
        }
    }
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy - .5;
    uv.x*= iResolution.x/iResolution.y;
    
    float camZ = 1.2;// + cos(iTime*.1) * .1;
    
    vec3 ro = vec3(0.0, 0.0, camZ);
    vec3 cw = normalize(vec3(0.0, 0.0, 0.0) - ro);
    vec3 cu = normalize(cross(cw, vec3(0.0, 1.0, 0.0)));
    vec3 cv = normalize(cross(cu, cw));
    vec3 rd = mat3(cu, cv, cw) * normalize(vec3(uv, 1.0));
    
    vec4 col = vec4(0, 0, 0, 1);
    
    // the intersection to the highest plane
    vec3 inter = boundsSDF(ro, rd, DEPTH);
    
    // the raymarching
    vec4 rm = raymarching(inter.xyz, rd);
    
    if (rm.w > 999.0) {
        fragColor = vec4(0);
        return;
    }
    
    vec3 normal = calcNormal(rm.xyz);
    
    vec3 lpos = vec3(cos(iTime)*20., sin(iTime)*20., 7.0);
    //vec3 lpos = vec3(4.0, 4.0, 2.0);
    
    col.rgb = calcLight(rm.xyz , lpos, vec3(1.0, 1.0, 1.0), rd, 0.5);
    
    // we get the shadow
    float shad = shadow(rm.xyz, lpos);
    col.rgb*= 0.5 + .5 * (1. - shad);
    
    //col.rgb = normal;
   
    fragColor = vec4(col.rgb, 1.0);
}