-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "127.0.0.1:19400",
        "127.0.0.1:19401"
    },
    runaway = 2 -- percent
}

output {
    size = vec2(1024, 768),
    name = "cornell",
    buffers = {
        -- no auxilliary image buffers
    }
}

render {
    antialiasing = 1,
    samples = 10,
    bounces = 3,
    threshold = 0.0001,
    min = vec3(-10, -10, -10),
    max = vec3(10, 10, 10),
}
