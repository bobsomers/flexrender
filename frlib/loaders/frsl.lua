local function frsl(filename)
    return function()
        local f = assert(io.open(filename, "r"))
        local procedure = f:read("*a")
        f:close()

        flexrender_shader {
            name = filename,
            code = procedure
        }

        return filename
    end
end

return frsl
