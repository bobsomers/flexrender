solution "flexrender"
    configurations {
        "debug",
        "release"
    }

    configuration "debug"
        defines {
            "DEBUG"
        }
        flags {
            "Symbols",
            "ExtraWarnings",
            "EnableSSE",
            "EnableSSE2"
        }

    configuration "release"
        defines {
            "NDEBUG"
        }
        flags {
            "Symbols",
            "OptimizeSpeed",
            "ExtraWarnings",
            "EnableSSE",
            "EnableSSE2"
        }

    configuration {"linux", "gmake"}
        buildoptions {
            "-std=c++11",
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --cflags luajit`",
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --cflags IlmBase`",
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --cflags OpenEXR`"
        }
        linkoptions {
            "-Lbin -lfr", -- This is annoying, but necessary.
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --libs --static luajit`",
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --libs --static IlmBase`",
            "`PKG_CONFIG_PATH=3p/build/lib/pkgconfig pkg-config --libs --static OpenEXR`"
        }

    project "libfr"
        kind "StaticLib"
        language "C++"
        targetdir "bin"
        targetname "fr"
        files {
            "src/shared/**.cpp"
        }
        includedirs {
            "src/shared",
            "3p/build/include"
        }

    project "flexrender"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin"
        targetname "flexrender"
        files {
            "src/render/**.cpp"
        }
        includedirs {
            "src/render",
            "src/shared",
            "3p/build/include",
        }
        libdirs {
            "bin",
            "3p/build/lib"
        }
        links {
            "libfr",
            "rt",
            "uv",
            "msgpack"
        }

    project "flexworker"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin"
        targetname "flexworker"
        files {
            "src/worker/**.cpp"
        }
        includedirs {
            "src/worker",
            "src/shared",
            "3p/build/include",
        }
        libdirs {
            "bin",
            "3p/build/lib"
        }
        links {
            "libfr",
            "rt",
            "uv",
            "msgpack"
        }
