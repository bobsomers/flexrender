local function fakeimg(filename)
    return function()
        flexrender_texture {
            name = filename,
            kind = "image",
            size = {3, 4},
            1.1, 2.2, 3.3,
            4.4, 5.5, 6.6,
            7.7, 8.8, 9.9,
            10.10, 11.11, 12.12
        }
        return filename
    end
end

return fakeimg
