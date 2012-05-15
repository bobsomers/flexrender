-- Builds a standard light shader with the given emissive color.
local function light(color)
    local code = table.concat{[[

-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

vec2 = fr.vec2
vec3 = fr.vec3

    ]],

    "local color = vec3(", tostring(color.r), ", ", tostring(color.g), ", ", tostring(color.b), ")\n",

    [[

function indirect(V, N, T)
    local ambient = emissive(T)

    accumulate3("R", "G", "B", ambient)
end

function emissive(T)
    return color
end

    ]]}

    return shader {
        code = code
    }
end

-- Module exports.
return {
    light = light
}
