-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
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
    eye = vec3(6, 6, 9),
    look = vec3(3, 0, 0)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(3, 3, 10)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(1)
}

for i = 1, 4 do
    for j = 1, 4 do
        local matname = table.concat {
            "mat-", tostring(i),
            "-", tostring(j),
            "-", tostring(k)
        }
        local color = vec3(1, 0, 0)

        material {
            name = matname,
            emissive = false,
            shader = fre.phong(0.6, color, 0.2, color, 0.2, 8)
        }

        mesh {
            material = matname,
            transform = translate(vec3(i, 0, j)) * scale(5),
            data = fre.obj("scenes/assets/bunny-hi.obj")
        }
    end
end
