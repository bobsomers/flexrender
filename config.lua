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
    size = vec2(512, 512),
    name = "toystore",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 1,
    samples = 1,
    bounces = 3,
    threshold = 0.0001,
    --min = vec3(-75, -5, -75),
    --max = vec3(75, 25, 75),
    min = vec3(-1, -1, -1),
    max = vec3(31, 31, 31),
    --min = vec3(-5, -5, -5),
    --max = vec3(15, 15, 15)
}
