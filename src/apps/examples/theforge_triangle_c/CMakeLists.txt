set(AppName theforge_triangle_c)

set(Src
        main.c
        )

set(Deps
        level0/core
        level0/guishell
        level0/spirvtools
        level2/theforge
        )

ADD_GUI_APP(${AppName} "${Src}" "${Deps}")

