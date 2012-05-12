-- Import loaders.
local sloaders = require "loaders.shaders"
local tloaders = require "loaders.textures"

-- Import primitives.
local simple = require "primitives.simple"

-- Import shader builders.
local phong = require "shaders.phong"
local lights = require "shaders.lights"

-- Module exports.
return {
    frsl = sloaders.frsl,
    procedural = tloaders.procedural,
    targa = tloaders.targa,
    plane = simple.plane,
    cube = simple.cube,
    phong = phong.phong,
    light = lights.light
}
