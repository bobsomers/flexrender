-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot

function direct(V, N, T, L, I)
    if dot(V, N) < 0.4 then
        return
    end

    local LdotN = dot(L, N)
    if LdotN > 0.95 then
        accumulate3("R", "G", "B", vec3(1, 1, 1))
    elseif LdotN > 0.66 then
        accumulate3("R", "G", "B", vec3(0.5, 0.2, 0.2))
    elseif LdotN > 0.33 then
        accumulate3("R", "G", "B", vec3(0.15, 0.05, 0.05))
    else
        accumulate3("R", "G", "B", vec3(0.05, 0.01, 0.01))
    end
end

function indirect(V, N, T)
    if dot(V, N) < 0.4 then
        return
    end

    accumulate3("R", "G", "B", vec3(0.1, 0.05, 0.05))
end
