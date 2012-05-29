-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
reflect = fr.reflect

function direct(V, N, T, L, I)
    -- Nothing.
end

function indirect(V, N, T)
    local R = reflect(-V, N)
    trace(R, 1)
end
