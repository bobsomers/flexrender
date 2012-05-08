-- Import frlib and extras.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local radians = fr.radians
local normalize = fr.normalize
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

camera {
    eye = vec3(4, 2, 6),
    look = vec3(0, 0, 0)
}

material {
    name = "brushed metal",
    emissive = true,
    shader = fre.frsl("config.lua"),
    textures = {
        diffuse = fre.procedural("scenes/test.lua"),
        specular = fre.fakeimg("image.fake")
    }
}

mesh {
    material = "brushed metal",
    transform = translate(vec3(2, 0, 2)),
    data = fre.cube(1)
}

mesh {
    material = "brushed metal",
    transform = translate(vec3(-2, 0, 2)) * scale(vec3(1, 3, 2)),
    data = fre.cube(1)
}

mesh {
    material = "brushed metal",
    transform = translate(vec3(2, 0, -2)) * rotate(radians(45), vec3(0, 1, 0)),
    data = fre.cube(1)
}

mesh {
    material = "brushed metal",
    transform = translate(vec3(-2, 0, -2)) * rotate(radians(-30), normalize(vec3(-1, 1, -1))) * scale(2),
    data = fre.cube(1)
}
