-- Import frlib.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"

-- Handy aliases.
local vec2 = fr.vec2
local vec3 = fr.vec3

network {
    workers = {
        "127.0.0.1",
        "127.0.0.1:19401",
        "127.0.0.1:19402",
        "127.0.0.1:19403",
        "127.0.0.1:19404",
        "127.0.0.1:19405",
        "127.0.0.1:19406",
        "127.0.0.1:19407"
    },
    runaway = 5 -- percent
}

output {
    size = vec2(640, 480),
    name = "toystore",
    buffers = {
        -- none
    }
}

render {
    antialiasing = 1,
    samples = 10,
    bounces = 2,
    threshold = 0.0001,
    min = vec3(-1, -1, -1),
    max = vec3(34, 21, 34),
}
