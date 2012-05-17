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
    eye = vec3(12.5, 35, 35),
    look = vec3(12.5, 0, 0)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(12.5, 35, 12.5)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(2)
}

for i = 1, 25 do
    for j = 1, 25 do
        for k = 1, 25 do
            local matname = table.concat {
                "mat-", tostring(i),
                "-", tostring(j),
                "-", tostring(k)
            }
            local color = vec3(i / 10, j / 10, j / 10)

            material {
                name = matname,
                emissive = false,
                shader = fre.phong(0.6, color, 0.2, color, 0.2, 8)
            }

            mesh {
                material = matname,
                transform = translate(vec3(i, j, k)),
                data = fre.cube(0.5)
            }
        end
    end
end
