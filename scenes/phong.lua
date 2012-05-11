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

local diffuse = 0.7
local ambient = 0.3
local color = vec3(0.196, 0.486, 0.796)

function direct(V, N, T, L, I)
    accumulate3("R", "G", "B", diffuse * color * I * dot(N, L))
end

function indirect(V, N, T)
    accumulate3("R", "G", "B", ambient * color)
end

function emissive(T)
    return vec3(1, 1, 1)
end
