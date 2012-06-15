-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "127.0.0.1",
        "10.11.12.1"
    },
    runaway = 2 -- percent
}

output {
    size = vec2(800, 600),
    name = "toystore",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 1,
    samples = 10,
    bounces = 3,
    threshold = 0.0001,
    min = vec3(-1, -1, -1),
    max = vec3(34, 21, 34),
}
