--add_rules("mode.debug", "mode.release")

add_requires("libcurl 8.11.0")

--includes("src/json")

set_languages("c++23")

target("llmapi")
    set_kind("moduleonly")
    add_files("src/*.cppm", { public = true, install = true })
    add_packages("libcurl", { public = true })
    --add_deps("__nlohmann_json")
    add_includedirs("src/json")
    add_headerfiles("src/json/json.hpp")
    add_files("src/json/json.cppm", { public = true })
    -- relocation R_X86_64_32 against `.rodata' can not be used when making a shared object; recompile with -fPIC
    -- ld: failed to set dynamic section sizes: bad value
    -- collect2: error: ld returned 1 exit status
    --add_cxxflags("-fPIC")

target("llmapi_c")
    --set_kind("shared")
    set_kind("static")
    add_files("src/c/llmapi.cpp")
    add_deps("llmapi")
    add_includedirs("include", { public = true })
    add_headerfiles("include/llmapi.h")

includes("examples")