-- Import frlib and extras.
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local radians = fr.radians
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

camera {
    eye = vec3(0, 0, 8),
    look = vec3(0, 0, 0)
}

material {
    name = "white",
    emissive = false,
    shader = fre.frsl("scenes/phong.lua"), -- TODO
    textures = {
        ramp = fre.procedural("scenes/ramp.lua")
    }
}

material {
    name = "red",
    emissive = false,
    shader = fre.frsl("scenes/phong.lua"), -- TODO
    textures = {
        ramp = fre.procedural("scenes/ramp.lua")
    }
}

material {
    name = "green",
    emissive = false,
    shader = fre.frsl("scenes/phong.lua"), -- TODO
    textures = {
        ramp = fre.procedural("scenes/ramp.lua")
    }
}

material {
    name = "light",
    emissive = true,
    shader = fre.frsl("scenes/phong.lua"), -- TODO
    textures = {
        ramp = fre.procedural("scenes/ramp.lua")
    }
}

-- Light.
mesh {
    material = "light",
    transform = translate(vec3(0, 2.7, 0)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(1.2)
}

-- Floor.
mesh {
    material = "white",
    transform = translate(vec3(0, -2.75, 0)) * rotate(radians(-90), vec3(1, 0, 0)),
    data = fre.plane(5.5)
}

-- Ceiling.
mesh {
    material = "white",
    transform = translate(vec3(0, 2.75, 0)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(5.5)
}

-- Back wall.
mesh {
    material = "white",
    transform = translate(vec3(0, 0, -2.75)),
    data = fre.plane(5.5)
}

-- Left wall.
mesh {
    material = "red",
    transform = translate(vec3(-2.75, 0, 0)) * rotate(radians(90), vec3(0, 1, 0)),
    data = fre.plane(5.5)
}

-- Right wall.
mesh {
    material = "red",
    transform = translate(vec3(2.75, 0, 0)) * rotate(radians(-90), vec3(0, 1, 0)),
    data = fre.plane(5.5)
}

-- Tall box.
mesh {
    material = "white",
    transform = translate(vec3(-0.9, -1.1, -0.9)) * rotate(radians(20), vec3(0, 1, 0)) * scale(vec3(1.7, 3.3, 1.7)),
    data = fre.cube(1)
}

-- Short box.
mesh {
    material = "white",
    transform = translate(vec3(1, -1.95, 1)) * rotate(radians(-20), vec3(0, 1, 0)) * scale(1.6),
    data = fre.cube(1)
}
