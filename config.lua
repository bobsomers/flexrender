-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "129.65.51.112",
        "129.65.51.113",
        "129.65.51.122",
        "129.65.51.115",
        "129.65.51.116",
        "129.65.51.117",
        "129.65.51.119",
        "129.65.51.120"
    },
    runaway = 5 -- percent
}

output {
    size = vec2(1024, 768),
    name = "toystore",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 2,
    samples = 10,
    bounces = 2,
    threshold = 0.0001,
    min = vec3(-1, -1, -1),
    max = vec3(34, 21, 34),
}
