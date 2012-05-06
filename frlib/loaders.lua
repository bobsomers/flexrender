-- Loads the FlexRender Shading Language source code into a FlexRender shader.
local function frsl(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return shader {
        code = procedure
    }
end

-- Loads the procedural texture source code into a FlexRender texture.
local function procedural(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return texture {
        kind = "procedural",
        code = procedure
    }
end

-- Creates fake image data for testing FlexRender image textures.
local function fakeimg(filename)
    return texture {
        kind = "image",
        size = {3, 4},
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9,
        10.10, 11.11, 12.12
    }
end

-- Module exports.
return {
    frsl = frsl,
    procedural = procedural,
    fakeimg = fakeimg
}
