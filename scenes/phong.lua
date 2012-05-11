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

function direct(V, N, T, L, I)
    accumulate3("R", "G", "B", vec3(0.196, 0.486, 0.796))
    write("intersection", 1)
end

function indirect(V, N, T)
    -- TODO
end

function emissive(V, N, T)
    -- TODO
end
