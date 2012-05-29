-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot
reflect = fr.reflect

function direct(V, N, T, L, I)
    local NdotL = dot(N, L)
    if NdotL < 0 then NdotL = 0 end

    local R = reflect(-V, N)
    local VdotR = dot(R, V)
    if VdotR < 0 then VdotR = 0 end

    local color = texture3("tile_r", "tile_g", "tile_b", T)
    local diffuse = 0.6 * color * I * NdotL
    local specular = 0.2 * I * (VdotR ^ 8)
    accumulate3("R", "G", "B", diffuse + specular)
end

function indirect(V, N, T)
    local R = reflect(-V, N)
    trace(R, 0.2)
end
