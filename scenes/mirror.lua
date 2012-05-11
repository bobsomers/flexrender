-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot
reflect = fr.reflect
normalize = fr.normalize

--[[
    V = normalized view vector (vec3)
    N = interpolated surface normal (vec3)
    T = interpolated texture coordinates (vec2)
    L = normalized light vector (vec3)
    I = illumination from the light (vec3)
]]

local diffuse = 0.2
local ambient = 0.8

function direct(V, N, T, L, I)
    accumulate3("R", "G", "B", diffuse * vec3(1, 1, 1) * I * dot(N, L))
end

function indirect(V, N, T)
    trace(normalize(reflect(-V, N)), 1)
end
