-- library loaders for shaders, textures, and mesh data
local frsl = require "frlib.loaders.frsl"
local procedural = require "frlib.loaders.procedural"
local fakeimg = require "frlib.loaders.fakeimg"
local unitbox = require "frlib.loaders.unitbox"

-- library functions for transforms
local rotate = require "frlib.transforms.rotate"
local scale = require "frlib.transforms.scale"
local translate = require "frlib.transforms.translate"

camera {
    eye = {0, 0, 5},
    look = {0, 0, 0}
}

material {
    name = "brushed metal",
    emissive = true,
    shader = frsl("config.lua"),
    textures = {
        diffuse = procedural("example.lua"),
        specular = fakeimg("image.fake")
    }
}

mesh {
    name = "awesome",
    material = "brushed metal",
    transforms = {
        rotate(math.pi, {1, 2, 3}),
        scale({4, 5, 6}),
        translate({7, 8, 9})
    },
    data = unitbox()
}
