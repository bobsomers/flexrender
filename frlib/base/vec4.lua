-- Import relevant base types.
local types = require "base.types"
require "base.common"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4

-- Forward index getter to common type.
vec4.__index = types.common.__index

-- Forward index setter to common type.
vec4.__newindex = types.common.__newindex

-- Getters.
vec4.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    z = function(v) return v[3] end,
    w = function(v) return v[4] end,
    r = function(v) return v[1] end,
    g = function(v) return v[2] end,
    b = function(v) return v[3] end,
    a = function(v) return v[4] end
}

-- Setters.
vec4.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    z = function(v, val) v[3] = val end,
    w = function(v, val) v[4] = val end,
    r = function(v, val) v[1] = val end,
    g = function(v, val) v[2] = val end,
    b = function(v, val) v[3] = val end,
    a = function(v, val) v[3] = val end
}

-- Constructor that provides the following forms:
--  vec4(x) -> <x, x, x, x>             (x is a number)
--  vec4(v) -> <v.x, v.y, v.z, v.w>     (v is a vec4)
--  vec4(v, w) -> <v.x, v.y, v.z, w>    (v is a vec3, w is a number)
--  vec4(v, z, w) -> <v.x, v.y, w, z>   (v is a vec2, w, z are numbers)
--  vec4(x, y, z, w) -> <x, y, z, w>    (x, y, z, w are numbers)
function vec4.new(...)
    local v = nil

    local nargs = select("#", ...)
    if nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            v = {arg, arg, arg, arg}
        elseif argmt == vec4 then
            v = {arg[1], arg[2], arg[3], arg[4]}
        else
            error("invalid construction of vec4", 3)
        end
    elseif nargs == 2 then
        local args = {select(1, ...), select(2, ...)}
        local arg1mt = getmetatable(args[1])
        if arg1mt ~= vec3 or type(args[2]) ~= "number" then
            error("invalid construction of vec4", 3)
        end
        local v3 = args[1]
        v = {v3[1], v3[2], v3[3], args[2]}
    elseif nargs == 3 then
        local args = {select(1, ...), select(2, ...), select(3, ...)}
        local arg1mt = getmetatable(args[1])
        if arg1mt ~= vec2 or type(args[2]) ~= "number" or type(args[3]) ~= "number" then
            error("invalid construction of vec4", 3)
        end
        local v2 = args[1]
        v = {v2[1], v2[2], args[2], args[3]}
    elseif nargs == 4 then
        local args = {select(1, ...), select(2, ...), select(3, ...), select(4, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" or type(args[3]) ~= "number" or type(args[4]) ~= "number" then
            error("invalid construction of vec4", 3)
        end
        v = {args[1], args[2], args[3], args[4]}
    else
        error("invalid construction of vec4", 3)
    end

    setmetatable(v, vec4)
    return v
end

-- Fast constructor for internal use only.
function vec4.frnew(x, y, z, w)
    local v = {x, y, z, w}
    setmetatable(v, vec4)
    return v
end

-- Attempts to coerce val to a vec3 by construction.
function vec4.frcoerce(val)
    if getmetatable(val) == vec4 then
        return val
    end

    local result, v = pcall(vec4.new, val)
    if result then
        return v
    end

    error("no known conversion to vec4", 3)
end

-- Converts the vector to a string representation.
function vec4.__tostring(v)
    return table.concat {"vec4<", v[1], ", ", v[2], ", ", v[3], ", ", v[4], ">"}
end

-- Adds two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec4.
function vec4.__add(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4])
end

-- Subtracts two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec4.
function vec4.__sub(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4])
end

-- Multiplies two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec4.
function vec4.__mul(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4])
end

-- Divides two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec4.
function vec4.__div(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4])
end

-- Negates a vector component-wise.
-- Always produces a vec4.
function vec4.__unm(v)
    return vec4.frnew(-v[1], -v[2], -v[3], -v[4])
end

-- Module exports.
return nil
