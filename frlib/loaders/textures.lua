-- Loads a procedural texture source file into a FlexRender texture.
-- Returns the resource ID of the texture.
local function procedural(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return texture {
        kind = "procedural",
        code = procedure
    }
end

-- Fake image texture loader for testing only.
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
    procedural = procedural,
    fakeimg = fakeimg
}
