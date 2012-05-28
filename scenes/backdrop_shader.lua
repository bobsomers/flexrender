-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3

function direct(V, N, T, L, I)
    -- Nothing.
end

function indirect(V, N, T)
    local color = texture3("bg_r", "bg_g", "bg_b", T)
    accumulate3("R", "G", "B", color)
end
