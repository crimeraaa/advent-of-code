local function variadic(x, y, ...)
    -- '"#"' in `select` returns the total number of arguments received.
    if select("#", ...) > 0 then
        print("Have varargs!")
    else
        print("No varargs.")
    end
end

variadic(1, 2)
variadic(1)
variadic(1, 2, 3)
