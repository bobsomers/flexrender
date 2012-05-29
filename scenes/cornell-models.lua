-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local radians = fr.radians
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

local tile_r, tile_g, tile_b = fre.targa("scenes/assets/tile1.tga")

function draw_tile(i, j)
    local tile_mat = table.concat {
        "mat-", tostring(i), "-", tostring(j)
    }

    material {
        name = tile_mat,
        emissive = false,
        shader = fre.frsl("scenes/shiny_tile_shader.lua"),
        textures = {
            tile_r = tile_r,
            tile_g = tile_g,
            tile_b = tile_b,
        }
    }

    mesh {
        material = tile_mat,
        transform = translate(vec3(i + 0.5, -2.75, j + 0.5)) * rotate(radians(-90), vec3(1, 0, 0)),
        data = fre.plane(1)
    }
end

camera {
    eye = vec3(0, 0.01, 8),
    look = vec3(0, 0, 0)
}

material {
    name = "white",
    emissive = false,
    shader = fre.montecarlo(0.6, vec3(0.730, 0.739, 0.729),
                            0.2, 32,
                            0.2, 8)
    --shader = fre.phong(0.6, vec3(0.730, 0.739, 0.729),
    --                   0.2, vec3(0.730, 0.739, 0.729),
    --                   0.2, 8)
}

material {
    name = "red",
    emissive = false,
    --shader = fre.montecarlo(0.6, vec3(0.610, 0.056, 0.062),
    --                        0.2, 32,
    --                        0.2, 8)
    shader = fre.phong(0.6, vec3(0.610, 0.056, 0.062),
                       0.2, vec3(0.610, 0.056, 0.062),
                       0.2, 8)
}

material {
    name = "green",
    emissive = false,
    --shader = fre.montecarlo(0.6, vec3(0.117, 0.435, 0.115),
    --                        0.2, 32,
    --                        0.2, 8)
    shader = fre.phong(0.6, vec3(0.117, 0.435, 0.115),
                       0.2, vec3(0.117, 0.435, 0.115),
                       0.2, 8)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 0.788, 0.553))
}

material {
    name = "fill",
    emissive = true,
    shader = fre.light(vec3(0.5, 0.394, 0.277))
}

material {
    name = "mirror",
    emissive = false,
    shader = fre.frsl("scenes/mirror_shader.lua")
}

material {
    name = "toon",
    emissive = false,
    shader = fre.frsl("scenes/toon_shader.lua")
}

-- Light.
mesh {
    material = "light",
    transform = translate(vec3(0, 2.74, 0)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(1.2)
}

-- Fill light.
mesh {
    material = "fill",
    transform = translate(vec3(0, 2.5, 5)) * rotate(radians(135), vec3(1, 0, 0)),
    data = fre.plane(1)
}

-- Floor.
for i = 1, 6 do
    for j = 1, 6 do
        draw_tile(i - 3.75, j - 3.75)
    end
end

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
    material = "green",
    transform = translate(vec3(2.75, 0, 0)) * rotate(radians(-90), vec3(0, 1, 0)),
    data = fre.plane(5.5)
}

-- Buddha.
mesh {
    material = "mirror",
    transform = translate(vec3(-0.9, -0.75, -0.9)) * rotate(radians(20), vec3(0, 1, 0)) * scale(2),
    data = fre.obj("scenes/assets/buddha-hi.obj", true)
}

-- Bunny.
mesh {
    material = "toon",
    transform = translate(vec3(1, -1.75, 1)) * rotate(radians(20), vec3(0, 1, 0)),
    data = fre.obj("scenes/assets/bunny-hi.obj", true)
}
