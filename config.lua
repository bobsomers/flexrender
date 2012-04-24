network {
    workers = {
        "10.11.12.25:19400"
    }
}

output {
    size = {1280, 720},
    name = "test",
    buffers = {
        "intersection"
    }
}

render {
    antialiasing = 1,
    min = {-100, -100, -100},
    max = {100, 100, 100},
}
