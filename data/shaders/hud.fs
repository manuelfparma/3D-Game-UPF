varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform bool u_discard;
uniform vec3 u_discard_color;
uniform bool u_grayscale;

void main()
{
    vec2 uv = v_uv;
    vec4 textureColor = texture2D(u_texture, uv);

    if (u_discard && textureColor.rgb == u_discard_color) {
        discard;
    }

    if (u_grayscale) {
        float grayscale = dot(textureColor.rgb, vec3(0.299, 0.587, 0.114));
        textureColor.rgb = vec3(grayscale);
    }

    gl_FragColor = u_color * textureColor;
}
