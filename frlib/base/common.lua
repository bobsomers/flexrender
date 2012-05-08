-- Import relevant base types.
local types = require "base.types"
local common = types.common

-- Forwarding constructor based on type name.
function common.__call(parent, ...)
    local obj = parent.new(...)
    setmetatable(obj, parent)
    return obj
end

-- Getter lookup.
function common.__index(table, key)
    local mt = getmetatable(table)
    local func = mt.getters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    return func(table)
end

-- Setter lookup.
function common.__newindex(table, key, value)
    local mt = getmetatable(table)
    local func = mt.setters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    func(table, value)
end

-- Module exports.
return nil
