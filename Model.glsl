////////////////////////////////////////
// Model.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 ModelMtx=mat4(1);
uniform mat4 ModelViewProjMtx=mat4(1);

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main() {
	gl_Position=ModelViewProjMtx * vec4(Position,1);

	fragPosition=vec3(ModelMtx * vec4(Position,1));
	fragNormal=vec3(ModelMtx * vec4(Normal,0));
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 AmbientColor1=vec3(0.25);
uniform vec3 DiffuseColor1=vec3(0.5);
uniform vec3 specular1 =vec3(1); 
uniform vec3 LightDirection=normalize(vec3(0.6, 0, 0.1));
uniform vec3 LightColor=vec3(1, 0.25, 0);

uniform vec3 AmbientColor2=vec3(0.15);
uniform vec3 DiffuseColor2=vec3(1);
uniform vec3 specular2 =vec3(1); 
uniform vec3 LightDirection2=normalize(vec3(0 , -0.6, 0.1));
uniform vec3 LightColor2=vec3(0, 0.5, 1);

uniform float shininess = 100; 

out vec3 finalColor;

////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

vec3 ComputeLight (const in vec3 direction, const in vec3 lightcolor, 
				   const in vec3 normal, const in vec3 halfvec, 
				   const in vec3 mydiffuse, const in vec3 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec3 lambert = mydiffuse * lightcolor * max (nDotL, 0.0);  
		
        float nDotH = dot(normal, halfvec) ; 
        vec3 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        return lambert + phong;            
} 


void main() {

    vec3 finalcolor = AmbientColor1 + AmbientColor2; 

	const vec3 eyepos = vec3(0,0,0);
	vec3 eyedirn = normalize(eyepos - fragPosition); 
	vec3 normal = normalize(fragNormal); 

	vec3 direction0 = normalize(LightDirection);
	vec3 half0 = normalize(direction0 + eyedirn);
	finalcolor += ComputeLight(direction0, LightColor, normal, half0, DiffuseColor1, specular1, shininess);

	vec3 position = LightDirection2;
	vec3 direction1 = normalize(position - fragPosition);
	vec3 half1 = normalize(direction1 + eyedirn);
	finalcolor += ComputeLight(direction1, LightColor2, normal, half1, DiffuseColor2, specular2, shininess);

	finalColor = sqrt(finalcolor);
}

#endif

////////////////////////////////////////////////////////////////////////////////
