-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot

--[[
    V = normalized view vector (vec3)
    N = interpolated surface normal (vec3)
    T = interpolated texture coordinates (vec2)
    L = normalized light vector (vec3)
    I = illumination from the light (vec3)
]]

local diffuse = 0.9
local ambient = 0.1

function direct(V, N, T, L, I)
    local ramp = texture("ramp", T)
    accumulate3("R", "G", "B", diffuse * ramp * I * dot(N, L))
end

function indirect(V, N, T)
    accumulate3("R", "G", "B", ambient * vec3(1, 1, 1))
end

function emissive(T)
    return vec3(1, 1, 1)
end
