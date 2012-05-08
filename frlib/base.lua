--[[
    FlexRender scripting library base functionality.

    Includes vector (vec2, vec3, vec4) types and a 4x4 matrix type (mat4) which
    is a set of 4 column vectors.

    Each vector type has component-wise arithmetic defined, including type
    coersion where possible to include scalar/vector operations and stepping
    up and down in vector dimensionality.

    Also includes utility functions for common vector/matrix ops such as length,
    distance, dot products, cross products, and normalization.

    Lastly, there are helpers for constructing common 4D transformation
    matrices, such as scaling (uniform and non-uniform), rotation about the
    cardinal axes, and translation.
]]

local common = {}
local vec2 = {}
local vec3 = {}
local vec4 = {}
local mat4 = {}

-- Common forwarding methods for all base types.
function common.__call(parent, ...)
    local obj = parent.new(...)
    setmetatable(obj, parent)
    return obj
end

function common.__index(table, key)
    local mt = getmetatable(table)
    local func = mt.getters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    return func(table)
end

function common.__newindex(table, key, value)
    local mt = getmetatable(table)
    local func = mt.setters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    func(table, value)
end

-- General utility functions.
local function radians(d)
    return math.pi / 180 * d
end

local function degrees(r)
    return 180 / math.pi * r
end

-- 2D Vector
vec2.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    u = function(v) return v[1] end,
    v = function(v) return v[2] end,
}

vec2.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    u = function(v, val) v[1] = val end,
    v = function(v, val) v[2] = val end,
}

vec2.__index = common.__index
vec2.__newindex = common.__newindex

function vec2.new(...)
    local v = nil

    local nargs = select("#", ...)
    if nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            v = {arg, arg}
        elseif argmt == vec2 or argmt == vec3 or argmt == vec4 then
            v = {arg[1], arg[2]}
        else
            error("invalid construction of vec2", 3)
        end
    elseif nargs == 2 then
        local args = {select(1, ...), select(2, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" then
            error("invalid construction of vec2", 3)
        end
        v = {args[1], args[2]}
    else
        error("invalid construction of vec2", 3)
    end

    setmetatable(v, vec2)
    return v
end

function vec2.frnew(x, y)
    local v = {x, y}
    setmetatable(v, vec2)
    return v
end

function vec2.frcoerce(val)
    if getmetatable(val) == vec2 then
        return val
    end

    local result, v = pcall(vec2.new, val)
    if result then
        return v
    end

    error("no known conversion to vec2", 3)
end

function vec2.__tostring(v)
    return table.concat {"vec2<", v[1], ", ", v[2], ">"}
end

function vec2.__add(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] + b[1], a[2] + b[2])
end

function vec2.__sub(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] - b[1], a[2] - b[2])
end

function vec2.__mul(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] * b[1], a[2] * b[2])
end

function vec2.__div(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] / b[1], a[2] / b[2])
end

function vec2.__unm(v)
    return vec2.frnew(-v[1], -v[2])
end

-- 3D Vector
vec3.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    z = function(v) return v[3] end,
    r = function(v) return v[1] end,
    g = function(v) return v[2] end,
    b = function(v) return v[3] end
}

vec3.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    z = function(v, val) v[3] = val end,
    r = function(v, val) v[1] = val end,
    g = function(v, val) v[2] = val end,
    b = function(v, val) v[3] = val end
}

vec3.__index = common.__index
vec3.__newindex = common.__newindex

function vec3.new(...)
    local v = nil

    local nargs = select("#", ...)
    if nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            v = {arg, arg, arg}
        elseif argmt == vec3 or argmt == vec4 then
            v = {arg[1], arg[2], arg[3]}
        else
            error("invalid construction of vec3", 3)
        end
    elseif nargs == 2 then
        local args = {select(1, ...), select(2, ...)}
        local arg1mt = getmetatable(args[1])
        if arg1mt ~= vec2 or type(args[2]) ~= "number" then
            error("invalid construction of vec3", 3)
        end
        local v2 = args[1]
        v = {v2[1], v2[2], args[2]}
    elseif nargs == 3 then
        local args = {select(1, ...), select(2, ...), select(3, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" or type(args[3]) ~= "number" then
            error("invalid construction of vec3", 3)
        end
        v = {args[1], args[2], args[3]}
    else
        error("invalid construction of vec3", 3)
    end

    setmetatable(v, vec3)
    return v
end

function vec3.frnew(x, y, z)
    local v = {x, y, z}
    setmetatable(v, vec3)
    return v
end

function vec3.frcoerce(val)
    if getmetatable(val) == vec3 then
        return val
    end

    local result, v = pcall(vec3.new, val)
    if result then
        return v
    end

    error("no known conversion to vec3", 3)
end

function vec3.__tostring(v)
    return table.concat {"vec3<", v[1], ", ", v[2], ", ", v[3], ">"}
end

function vec3.__add(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3])
end

function vec3.__sub(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3])
end

function vec3.__mul(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3])
end

function vec3.__div(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3])
end

function vec3.__unm(v)
    return vec3.frnew(-v[1], -v[2], -v[3])
end

-- 4D Vector
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

vec4.__index = common.__index
vec4.__newindex = common.__newindex

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

function vec4.frnew(x, y, z, w)
    local v = {x, y, z, w}
    setmetatable(v, vec4)
    return v
end

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

function vec4.__tostring(v)
    return table.concat {"vec4<", v[1], ", ", v[2], ", ", v[3], ", ", v[4], ">"}
end

function vec4.__add(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4])
end

function vec4.__sub(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4])
end

function vec4.__mul(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4])
end

function vec4.__div(a, b)
    a = vec4.frcoerce(a)
    b = vec4.frcoerce(b)
    return vec4.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4])
end

function vec4.__unm(v)
    return vec4.frnew(-v[1], -v[2], -v[3], -v[4])
end

-- Vector utility functions.
local function length(v)
    local mt = getmetatable(v)

    if mt == vec2 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2])
    elseif mt == vec3 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3])
    elseif mt == vec4 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3] + v[4] * v[4])
    end

    error("length is only defined for vec2/vec3/vec4", 2)
end

local function distance(p1, p2)
    local mt = getmetatable(p1)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("distance is only defined for vec2/vec3/vec4", 2)
    end
    if mt ~= getmetatable(p2) then
        error("arguments to distance must be of the same type", 2)
    end

    return length(p1 - p2)
end

local function dot(a, b)
    local mt = getmetatable(a)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("dot is only defined for vec2/vec3/vec4", 2)
    end
    if mt ~= getmetatable(b) then
        error("arguments to dot must be of the same type", 2)
    end

    if mt == vec2 then
        return a[1] * b[1] + a[2] * b[2]
    elseif mt == vec3 then
        return a[1] * b[1] + a[2] * b[2] + a[3] * b[3]
    elseif mt == vec4 then
        return a[1] * b[1] + a[2] * b[2] + a[3] * b[3] + a[4] * b[4]
    end
end

local function cross(a, b)
    if getmetatable(a) ~= vec3 or getmetatable(b) ~= vec3 then
        error("cross is only defined for vec3", 2)
    end

    return vec3.frnew(a[2] * b[3] - b[2] * a[3],
                      a[3] * b[1] - b[3] * a[1],
                      a[1] * b[2] - b[1] * a[2])
end

local function normalize(v)
    local mt = getmetatable(v)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("normalize is only defined for vec2/vec3/vec4", 2)
    end

    return v / length(v)
end

-- 4x4 Matrix
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

function mat4.frnew(a, b, c, d)
    local m = {a, b, c, d}
    setmetatable(m, mat4)
    return m
end

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

function mat4.__tostring(m)
    return table.concat {
        "mat4<",
        tostring(m[1]), ", ",
        tostring(m[2]), ", ",
        tostring(m[3]), ", ",
        tostring(m[4]), ">"
    }
end

function mat4.__add(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4])
end

function mat4.__sub(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4])
end

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

function mat4.__div(a, b)
    a = mat4.frcoerce(a)
    b = mat4.frcoerce(b)
    return mat4.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4])
end

function mat4.__unm(m)
    return mat4.frnew(-m[1], -m[2], -m[3], -m[4])
end

-- Matrix utility functions.
local function scale(v)
    v = vec3.frcoerce(v)
    return mat4.frnew(vec4.frnew(v[1], 0, 0, 0),
                      vec4.frnew(0, v[2], 0, 0),
                      vec4.frnew(0, 0, v[3], 0),
                      vec4.frnew(0, 0, 0, 1))
end

local function rotate(a, v)
    if type(a) ~= "number" then
        error("rotate expects the angle to be a number (in radians)", 2)
    end
    v = vec3.frcoerce(v)

    local c = math.cos(a)
    local s = math.sin(a)
    local axis = normalize(v)

    -- Credit: GLM, in matrix_transform.inl
    local d = vec4.frnew(c + (1 - c) * axis[1] * axis[1],
                         (1 - c) * axis[1] * axis[2] + s * axis[3],
                         (1 - c) * axis[1] * axis[3] - s * axis[2],
                         0)
    local e = vec4.frnew((1 - c) * axis[2] * axis[1] - s * axis[3],
                         c + (1 - c) * axis[2] * axis[2],
                         (1 - c) * axis[2] * axis[3] + s * axis[1],
                         0)
    local f = vec4.frnew((1 - c) * axis[3] * axis[1] + s * axis[2],
                         (1 - c) * axis[3] * axis[2] - s * axis[1],
                         c + (1 - c) * axis[3] * axis[3],
                         0)

    return mat4.frnew(d, e, f, vec4.frnew(0, 0, 0, 1))
end

local function translate(v)
    v = vec3.frcoerce(v)

    return mat4.frnew(vec4.frnew(1, 0, 0, 0),
                      vec4.frnew(0, 1, 0, 0),
                      vec4.frnew(0, 0, 1, 0),
                      vec4(v, 1))
end

-- Parent base types to common type.
setmetatable(vec2, common)
setmetatable(vec3, common)
setmetatable(vec4, common)
setmetatable(mat4, common)

-- Module exports.
return {
    radians = radians,
    degrees = degrees,
    vec2 = vec2,
    vec3 = vec3,
    vec4 = vec4,
    mat4 = mat4,
    length = length,
    distance = distance,
    dot = dot,
    cross = cross,
    normalize = normalize,
    scale = scale,
    rotate = rotate,
    translate = translate
}
