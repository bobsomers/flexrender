-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

local vec2 = fr.vec2
local vec3 = fr.vec3

--[[
    V = normalized view vector (vec3)
    N = interpolated surface normal (vec3)
    T = interpolated texture coordinates (vec2)
    L = normalized light vector (vec3)
    I = illumination from the light (vec3)
]]

function direct(V, N, T, L, I)
    setmetatable(V, vec3)
    setmetatable(N, vec3)
    setmetatable(T, vec2)
    setmetatable(L, vec3)
    setmetatable(I, vec3)

    print("V = " .. tostring(V))
    print("N = " .. tostring(N))
    print("T = " .. tostring(T))
    print("L = " .. tostring(L))
    print("I = " .. tostring(I))
end

function indirect(V, N, T)
    -- TODO
end

function emissive(V, N, T)
    -- TODO
end
