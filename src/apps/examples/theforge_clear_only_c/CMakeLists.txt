set(AppName theforge_clearonly_c)

set(Src
        main.c
        )

set(Deps
        level0/core
        level0/guishell
        level3/theforge
        level4/theforge_display
        )

ADD_GUI_APP(${AppName} "${Src}" "${Deps}")

