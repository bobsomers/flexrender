-- Import relevant base types.
local types = require "base.types"
require "base.common"
require "base.vec4"

-- Import utility functions.
local geometric = require "base.geometric"

-- Module aliases.
local mat4 = types.mat4
local vec4 = types.vec4
local dot = geometric.dot

-- Constructor that provides the following forms:
--  mat4() -> [identity]
--  mat4(x) -> [<x..x>,..,<x..x>]           (x is a number)
--  mat4(m) -> [<m11..m14>,..,<m41..m44>]   (m is a mat4)
--  mat4(a, b, c, d) -> [a, b, c, d]        (a, b, c, d are column vec4s)
--  mat4(a,..,p) -> [<a..d>,..,<m..p>]      (a..p are numbers)
function mat4.new(...)
    local m = nil

    local nargs = select('#', ...)
    if nargs == 0 then
        m = {vec4.frnew(1, 0, 0, 0),
             vec4.frnew(0, 1, 0, 0),
             vec4.frnew(0, 0, 1, 0),
             vec4.frnew(0, 0, 0, 1)}
    elseif nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            m = {vec4.frnew(arg, arg, arg, arg),
                 vec4.frnew(arg, arg, arg, arg),
                 vec4.frnew(arg, arg, arg, arg),
                 vec4.frnew(arg, arg, arg, arg)}
        elseif argmt == mat4 then
            m = {vec4.frnew(arg[1][1], arg[1][2], arg[1][3], arg[1][4]),
                 vec4.frnew(arg[2][1], arg[2][2], arg[2][3], arg[2][4]),
                 vec4.frnew(arg[3][1], arg[3][2], arg[3][3], arg[3][4]),
                 vec4.frnew(arg[4][1], arg[4][2], arg[4][3], arg[4][4])}
        else
            error("invalid construction of mat4", 3)
        end
    elseif nargs == 4 then
        local args = {select(1, ...), select(2, ...), select(3, ...), select(4, ...)}
        if getmetatable(args[1]) ~= vec4 or getmetatable(args[2]) ~= vec4 or getmetatable(args[3]) ~= vec4 or getmetatable(args[4]) ~= vec4 then
            error("invalid construction of vec4", 3)
        end
        m = {vec4.frnew(args[1][1], args[1][2], args[1][3], args[1][4]),
             vec4.frnew(args[2][1], args[2][2], args[2][3], args[2][4]),
             vec4.frnew(args[3][1], args[3][2], args[3][3], args[3][4]),
             vec4.frnew(args[4][1], args[4][2], args[4][3], args[4][4])}
    elseif nargs == 16 then
        local args = {select(1, ...), select(2, ...), select(3, ...), select(4, ...),
                      select(5, ...), select(6, ...), select(7, ...), select(8, ...),
                      select(9, ...), select(10, ...), select(11, ...), select(12, ...),
                      select(13, ...), select(14, ...), select(15, ...), select(16, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" or type(args[3]) ~= "number" or type(args[4]) ~= "number" or
           type(args[5]) ~= "number" or type(args[6]) ~= "number" or type(args[7]) ~= "number" or type(args[8]) ~= "number" or
           type(args[9]) ~= "number" or type(args[10]) ~= "number" or type(args[11]) ~= "number" or type(args[12]) ~= "number" or
           type(args[13]) ~= "number" or type(args[14]) ~= "number" or type(args[15]) ~= "number" or type(args[16]) ~= "number" then
            error("invalid construction of mat4", 3)
        end
        m = {vec4.frnew(args[1], args[2], args[3], args[4]),
             vec4.frnew(args[5], args[6], args[7], args[8]),
             vec4.frnew(args[9], args[10], args[11], args[12]),
             vec4.frnew(args[13], args[14], args[15], args[16])}
    else
        error("invalid construction of mat4", 3)
    end

    setmetatable(m, mat4)
    return m
end

-- Fast constructor for internal use only.
function mat4.frnew(a, b, c, d)
    local m = {a, b, c, d}
    setmetatable(m, mat4)
    return m
end

-- Attempts to coerce val to a mat4 by construction.
function mat4.frcoerce(val)
    if getmetatable(val) == mat4 then
        return val
    end

    local result, m = pcall(mat4.new, val)
    if result then
        return m
    end

    error("no known conversion to mat4", 3)
end

-- Converts the matrix to a string representation.
function mat4.__tostring(m)
    return table.concat {
        "mat4<",
        tostring(m[1]), ", ",
        tostring(m[2]), ", ",
        tostring(m[3]), ", ",
        tostring(m[4]), ">"
    }
end

-- Adds two matrices component-wise, coercing the arguments as necessary.
-- Always produces a mat4.
function mat4.__add(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4])
end

-- Subtracts two matrices component-wise, coercing the arguments as necessary.
-- Always produces a mat4.
function mat4.__sub(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4])
end

-- If a and b are mat4s, performs matrix multiplcation and produces a mat4.
-- If a is a mat4 and b is a vec4, performs matrix-vector multiplication and
--  produces a vec4.
-- Otherwise multiples component-wise, coercing the arguments as necessary and
--  produces a mat4.
function mat4.__mul(a, b)
    local mta = getmetatable(a)
    local mtb = getmetatable(b)

    if mta == mat4 then
        local rows = {vec4.frnew(a[1][1], a[2][1], a[3][1], a[4][1]),
                      vec4.frnew(a[1][2], a[2][2], a[3][2], a[4][2]),
                      vec4.frnew(a[1][3], a[2][3], a[3][3], a[4][3]),
                      vec4.frnew(a[1][4], a[2][4], a[3][4], a[4][4])}
        local cols = b
        if mtb == mat4 then
            return mat4.frnew(vec4.frnew(dot(rows[1], cols[1]), dot(rows[2], cols[1]), dot(rows[3], cols[1]), dot(rows[4], cols[1])),
                              vec4.frnew(dot(rows[1], cols[2]), dot(rows[2], cols[2]), dot(rows[3], cols[2]), dot(rows[4], cols[2])),
                              vec4.frnew(dot(rows[1], cols[3]), dot(rows[2], cols[3]), dot(rows[3], cols[3]), dot(rows[4], cols[3])),
                              vec4.frnew(dot(rows[1], cols[4]), dot(rows[2], cols[4]), dot(rows[3], cols[4]), dot(rows[4], cols[4])))
        elseif mtb == vec4 then
            return vec4.frnew(dot(rows[1], cols),
                              dot(rows[2], cols),
                              dot(rows[3], cols),
                              dot(rows[4], cols))
        end
    end

    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4])
end

-- Divides two matrices component-wise, coercing the arguments as necessary.
-- Always produces a mat4.
function mat4.__div(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4])
end

-- Negates a matrix component-wise.
-- Always produces a mat4.
function mat4.__unm(m)
    return mat4.frnew(-m[1], -m[2], -m[3], -m[4])
end

-- Module exports.
return nil
