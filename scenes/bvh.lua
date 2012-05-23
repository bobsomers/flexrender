-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local vec2 = fr.vec2
local translate = fr.translate

camera {
    eye = vec3(0, 0, 8),
    look = vec3(0, 0, 0)
}

material {
    name = "white",
    emissive = false,
    shader = fre.phong(0.6, vec3(0.730, 0.739, 0.729),
                       0.2, vec3(0.730, 0.739, 0.729),
                       0.2, 8)
}

mesh {
    material = "white",
    transform = translate(vec3(5, 5, 5)),
    data = function()
        triangle {{
            v = vec3(0, 0, 1),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }, {
            v = vec3(0, 1, 1),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }, {
            v = vec3(1, 0, 1),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(0, 0, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }, {
            v = vec3(1, 0, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }, {
            v = vec3(0, -1, 0),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(0, 0, -1),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }, {
            v = vec3(0, -1, -1),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }, {
            v = vec3(-1, 0, -1),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(0, 0, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }, {
            v = vec3(-1, 0, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }, {
            v = vec3(0, 1, 0),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }}
    end
}
