set(AppName theforge_triangle_c)

set(Src
        main.c
        )

set(Deps
        level0/core
        level0/guishell
        level0/spirvtools
        level3/theforge
        level4/theforge_display
        level4/theforge_shaderreflection
        level4/theforge_resourceloader
        )

ADD_GUI_APP(${AppName} "${Src}" "${Deps}")

