-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3

function texture(t)
    return vec3(1 - t.u, 1, 1 - t.v)
end
