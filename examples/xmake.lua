target("hello_mcpp")
    set_kind("binary")
    add_files("hello_mcpp.cpp")
    add_deps("llmapi")

target("basic")
    set_kind("binary")
    add_files("basic.cpp")
    add_deps("llmapi")

target("chat")
    set_kind("binary")
    add_files("chat.cpp")
    add_deps("llmapi")

includes("c")
includes("cxx17")