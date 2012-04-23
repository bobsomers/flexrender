local function frsl(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return shader {
        code = procedure
    }
end

return frsl
