-- Loads a FlexRender Shading Language source file into a FlexRender shader.
-- Returns the resource ID of the shader.
local function frsl(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return shader {
        code = procedure
    }
end

-- Module exports.
return {
    frsl = frsl
}
