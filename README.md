# FlexRender

This is the source code for the renderer implementation described in the paper
*FlexRender: A Distributed Rendering Architecture for Ray Tracing Huge Scenes
on Commodity Hardware*, presented at GRAPP 2013. FlexRender distributes
geometry among a cluster of commodity machines to keep the scene in-core within
the cluster, and passes rays around as messages using a feed-forward messaging
architecture that never requires replies. This keeps communication and
rendering completely asychronous.

For more information about FlexRender, including the paper and slides from the
presentation, see [flexrender.org](http://www.flexrender.org).

## Build Instructions

We rely on several third-party libraries, but anything that you won't find on a
standard Linux distro is downloaded, built, and statically linked by our
dependency build script. To build FlexRender itself, you'll need a C++ compiler
with reasonable C++11 support (GCC 4.7+ is known to work) and
[Premake](http://industriousone.com/premake), a build script generator.

First, check out the source code and run the dependency build script. This only
needs to be done once. (No packages are installed, everything is built and
linked within the `3p/` directory so no root privileges are required.)

    git clone https://github.com/bobsomers/flexrender.git
    cd flexrender/3p
    ./make.bash

Next, generate GNU Makefiles for building FlexRender by running Premake. Then,
run make (optionally with `config=release` for a release build).

    cd ..
    premake4 gmake
    make config=release

This should leave you with 3 binaries in the `bin/` directory. The `flexrender`
and `flexworker` executables are the renderer and worker respectively. The
`baseline` is the image plane decomposition.

## Directory Layout

* `3p/` All third-party libraries and build scripts.
* `frlib/` Lua libraries for scene files and FlexRender shaders.
* `scenes/` Some example scenes and shaders.
* `scripts/` Handy scripts for profiling.
* `src/[baseline|render|worker]` Code specific to the baseline, renderer, and worker executables.
* `src/shared` Shared code for the libfr static library.
* `config.lua` Example renderer configuration.

## Example Run

In two shells, we run two flexworkers. One on the default port (19400), the
other on port 19401.

    bin/flexworker
    bin/flexworker -p 19401

In a third shell, we run the renderer, giving it the path to its renderer
configuration (config.lua) and the scene file.

    bin/flexrender config.lua scenes/cornell.lua

When finished, this should write out cornell.exr. It will also write out the
image buffers of each worker, as well as a CSV file for each worker with
rendering statistics.

## License

The FlexRender source code is licensed under the MIT license. We encourage you
to use the code, ideas, and techniques to enhance your own research.

If anything from FlexRender does aid your research, please cite the paper
appropriately.
