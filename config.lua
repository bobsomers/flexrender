network {
    workers = {
        "127.0.0.1",
        "127.0.0.1:19401"
    }
}

output {
    size = {640, 480},
    name = "test",
    buffers = {
        "intersection"
    }
}

render {
    antialiasing = 3,
    min = {-100, -100, -100},
    max = {100, 100, 100},
}
