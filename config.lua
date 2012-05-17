-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "10.11.12.1",
        "10.11.12.25",
        "10.11.12.21",
        "10.11.12.27"
    },
    runaway = 2.5 -- percent
}

output {
    size = vec2(640, 480),
    name = "test",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 1,
    samples = 1,
    bounces = 2,
    threshold = 0.0001,
    min = vec3(-5, -5, -5),
    max = vec3(15, 15, 15),
}
