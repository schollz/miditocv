function random_number()
    return math.random(100, 150)
end

function new_env(code)
    local env = {}
    setmetatable(env, {
        __index = function(_, key)
            return _G[key] or 0
        end,
        __newindex = function(_, key, value)
            rawset(env, key, value)
        end
    })
    local chunk = load(code, "chunk", "t", env)
    chunk()
    return env
end

envs = {}
for i = 1, 8 do
    table.insert(envs, new_env(""))
end

function update_env(i, code)
    envs[i] = new_env(code)
end

function env_main(i)
    return envs[i].main()
end

math.randomseed(os.time())
