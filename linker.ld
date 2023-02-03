SECTIONS
{
    .text : {
        *(.rodata)
        *(.text.unlikely .text.*_unlikely)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }
}
INSERT AFTER .rodata;