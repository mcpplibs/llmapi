set_languages("c++23")
set_version("0.2.0")
set_policy("build.c++.modules", true)

add_repositories("mcpplibs-index https://github.com/mcpplibs/mcpplibs-index.git")
add_requires("mcpplibs-tinyhttps 0.1.0")

target("llmapi")
    set_kind("static")
    add_files("src/*.cppm", { public = true, install = true })
    add_files("src/providers/*.cppm", { public = true, install = true })
    add_packages("mcpplibs-tinyhttps", { public = true })
    add_includedirs("src/json")
    add_headerfiles("src/json/json.hpp")
    add_files("src/json/json.cppm", { public = true })

-- Only include examples/tests when building llmapi standalone
if os.scriptdir() == os.projectdir() then
    includes("examples")
    includes("tests")
end
