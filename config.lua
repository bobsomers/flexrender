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
    name = "cornell-models",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 2,
    samples = 32,
    bounces = 3,
    threshold = 0.0001,
    --min = vec3(-50, -5, -50),
    --max = vec3(50, 25, 50),
    min = vec3(-10, -10, -10),
    max = vec3(10, 10, 10),
    --min = vec3(-5, -5, -5),
    --max = vec3(15, 15, 15)
}
