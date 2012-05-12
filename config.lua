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
    }
}

output {
    size = vec2(512, 512),
    name = "test",
    buffers = {
        "intersection"
    }
}

render {
    antialiasing = 1,
    samples = 2,
    min = vec3(-100, -100, -100),
    max = vec3(100, 100, 100),
}
