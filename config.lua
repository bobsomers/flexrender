-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "127.0.0.1",
        "127.0.0.1:19401"

    },
    runaway = 5 -- percent
}

output {
    size = vec2(640, 360),
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
    min = vec3(-25, -25, -25),
    max = vec3(25, 25, 25),
    --min = vec3(-10, -10, -10),
    --max = vec3(10, 10, 10),
}
