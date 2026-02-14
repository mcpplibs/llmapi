-- TODO: fix build on macOS (12.3) libc++/println issue
if not is_host("macosx") then
    -- xmake f --toolchain=llvm --sdk=/opt/homebrew/opt/llvm@20
    --add_ldflags("-L/opt/homebrew/opt/llvm@20/lib/c++ -L/opt/homebrew/opt/llvm@20/lib/unwind -lunwind")

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
end
