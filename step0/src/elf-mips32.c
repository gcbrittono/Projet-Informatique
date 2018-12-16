#include <pelf/pelf.h>

int elf_mips_reloc_num( char *rname ) {
    elf_catch_reloc( rname, R_MIPS_NONE );
    elf_catch_reloc( rname, R_MIPS_16 );
    elf_catch_reloc( rname, R_MIPS_32 );
    elf_catch_reloc( rname, R_MIPS_REL32 );
    elf_catch_reloc( rname, R_MIPS_26 );
    elf_catch_reloc( rname, R_MIPS_HI16 );
    elf_catch_reloc( rname, R_MIPS_LO16 );
    elf_catch_reloc( rname, R_MIPS_GPREL16 );
    elf_catch_reloc( rname, R_MIPS_LITERAL );
    elf_catch_reloc( rname, R_MIPS_GOT16 );
    elf_catch_reloc( rname, R_MIPS_PC16 );
    elf_catch_reloc( rname, R_MIPS_CALL16 );
    elf_catch_reloc( rname, R_MIPS_GPREL32 );
    elf_catch_reloc( rname, R_MIPS_GOTHI16 );
    elf_catch_reloc( rname, R_MIPS_GOTLO16 );
    elf_catch_reloc( rname, R_MIPS_CALLHI16 );
    elf_catch_reloc( rname, R_MIPS_CALLLO16 );

    PELF_ERROR( "Unknown MIPS relocation" );
    return -1;
}

char *elf_mips_print_reltype( unsigned int type ) {
    switch( type ) {
        elf_catch_reloc_name( R_MIPS_NONE );
        elf_catch_reloc_name( R_MIPS_16 );
        elf_catch_reloc_name( R_MIPS_32 );
        elf_catch_reloc_name( R_MIPS_REL32 );
        elf_catch_reloc_name( R_MIPS_26 );
        elf_catch_reloc_name( R_MIPS_HI16 );
        elf_catch_reloc_name( R_MIPS_LO16 );
        elf_catch_reloc_name( R_MIPS_GPREL16 );
        elf_catch_reloc_name( R_MIPS_LITERAL );
        elf_catch_reloc_name( R_MIPS_GOT16 );
        elf_catch_reloc_name( R_MIPS_PC16 );
        elf_catch_reloc_name( R_MIPS_CALL16 );
        elf_catch_reloc_name( R_MIPS_GPREL32 );
        elf_catch_reloc_name( R_MIPS_GOTHI16 );
        elf_catch_reloc_name( R_MIPS_GOTLO16 );
        elf_catch_reloc_name( R_MIPS_CALLHI16 );
        elf_catch_reloc_name( R_MIPS_CALLLO16 );
    default :
        return "Unknown MIPS relocation";
    }

}
