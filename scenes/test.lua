-- import functionality
local base = require "frlib.base"
local loaders = require "frlib.loaders"
local primitives = require "frlib.primitives"

-- local aliases
local vec3 = base.vec3

camera {
    eye = vec3(4, 2, 6),
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
        {2, 0, 2, 1}
    },
    data = primitives.cube(1)
}

mesh {
    material = "brushed metal",
    transform = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {-2, 0, 2, 1}
    },
    data = primitives.cube(1)
}

mesh {
    material = "brushed metal",
    transform = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {2, 0, -2, 1}
    },
    data = primitives.cube(1)
}

mesh {
    material = "brushed metal",
    transform = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {-2, 0, -2, 1}
    },
    data = primitives.cube(1)
}
