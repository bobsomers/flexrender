-- import functionality
local base = require "frlib.base"
local loaders = require "frlib.loaders"
local primitives = require "frlib.primitives"

-- local aliases
local vec3 = base.vec3

camera {
    eye = vec3(0, 0, 5),
    look = vec3(0, 0, 0)
}

material {
    name = "brushed metal",
    emissive = true,
    shader = loaders.frsl("config.lua"),
    textures = {
        diffuse = loaders.procedural("scenes/test.lua"),
        specular = loaders.fakeimg("image.fake")
    }
}

mesh {
    material = "brushed metal",
    transform = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {10, 20, 30, 1}
    },
    --[[
    transforms = {
        rotate(math.pi, {1, 2, 3}),
        scale({4, 5, 6}),
        translate({7, 8, 9})
    },
    ]]
    data = primitives.cube(1)
}
