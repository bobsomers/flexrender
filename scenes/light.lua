-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3

--[[
    V = normalized view vector (vec3)
    N = interpolated surface normal (vec3)
    T = interpolated texture coordinates (vec2)
    L = normalized light vector (vec3)
    I = illumination from the light (vec3)
]]

function indirect(V, N, T)
    accumulate3("R", "G", "B", vec3(1, 1, 1))
end

function emissive(T)
    return vec3(1, 1, 1)
end
