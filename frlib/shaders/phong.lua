-- Builds a phong shader with diffuse contribution and color dscale and dcolor,
-- ambient contribution and color ascale and acolor, and specular contribution
-- and power sscale and spower.
local function phong(dscale, dcolor, ascale, acolor, sscale, spower)
    local code = table.concat{[[

-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot
reflect = fr.reflect

    ]],

    "local dscale = ", tostring(dscale), "\n",
    "local dcolor = vec3(", tostring(dcolor.r), ", ", tostring(dcolor.g), ", ", tostring(dcolor.b), ")\n",
    "local ascale = ", tostring(ascale), "\n",
    "local acolor = vec3(", tostring(acolor.r), ", ", tostring(acolor.g), ", ", tostring(acolor.b), ")\n",
    "local sscale = ", tostring(sscale), "\n",
    "local spower = ", tostring(spower), "\n",

    [[

function direct(V, N, T, L, I)
    local NdotL = dot(N, L)
    if NdotL < 0 then NdotL = 0 end

    local R = reflect(-V, N)
    local VdotR = dot(R, V)
    if VdotR < 0 then VdotR = 0 end

    local diffuse = dscale * dcolor * I * NdotL

    local specular = sscale * I * (VdotR ^ spower)

    accumulate3("R", "G", "B", diffuse + specular)
end

function indirect(V, N, T)
    local ambient = ascale * acolor

    accumulate3("R", "G", "B", ambient)
end

    ]]}

    return shader {
        code = code
    }
end

-- Builds a phong shader with diffuse contribution and color dscale and dcolor,
-- ambient contribution ascale, and specular contribution and power sscale and
-- spower. The ambient color is determined using samples Monte Carlo samples
-- in the hemisphere above the surface.
local function montecarlo(dscale, dcolor, ascale, samples, sscale, spower)
    local code = table.concat{[[

-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3
dot = fr.dot
reflect = fr.reflect
hemisample = fr.hemisample

    ]],

    "local dscale = ", tostring(dscale), "\n",
    "local dcolor = vec3(", tostring(dcolor.r), ", ", tostring(dcolor.g), ", ", tostring(dcolor.b), ")\n",
    "local ascale = ", tostring(ascale), "\n",
    "local samples = ", tostring(samples), "\n",
    "local sscale = ", tostring(sscale), "\n",
    "local spower = ", tostring(spower), "\n",

    [[

function direct(V, N, T, L, I)
    local NdotL = dot(N, L)
    if NdotL < 0 then NdotL = 0 end

    local R = reflect(-V, N)
    local VdotR = dot(R, V)
    if VdotR < 0 then VdotR = 0 end

    local diffuse = dscale * dcolor * I * NdotL

    local R = reflect(-V, N)
    local specular = sscale * I * (VdotR ^ spower)

    accumulate3("R", "G", "B", diffuse + specular)
end

function indirect(V, N, T)
    for i = 1, samples do
        trace(hemisample(N), 1 / samples)
    end
end

    ]]}

    return shader {
        code = code
    }
end

-- Module exports.
return {
    phong = phong,
    montecarlo = montecarlo
}
