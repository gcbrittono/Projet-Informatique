
/**
 * @file print.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Sep 22 19:02:40 2017
 * @brief Print ELF structures.
 *
 * Print ELF structures.
 */

#include <pelf/pelf.h>

static char *elf_print_reltype( unsigned int machine, unsigned int type ) {
    switch( machine ) {
    case EM_MIPS :
        return elf_mips_print_reltype( type );
    default :
        fprintf( PSTDOUT, "Unknown machine" );
        break;
    }

    return "";
}


void elf_print_string_table( unsigned char *table, size_t length ) {
    char *str = (char*) table;

    do {
        str = elf_next_string( table, length, &str );
        if ( !str ) return;
        if ( str ) {
            fprintf( PSTDOUT, "%ld:%s\n", str-(char*)table, str );
        }
    } while( str );
}

#define arch_print( arch, descr )			\
  case EM_##arch :					\
  fprintf( PSTDOUT, "EM_%s (%s)", #arch, descr );	\
  break;



/*
  Going 32/64-bit agnostic.
 */

void elf_print_ehdr( Elf_Ehdr ehdr ) {

    int      width = 0;
    int endianness = 0;

    if ( ehdr._32.e_ident[EI_MAG0] != ELF_MAG0 ||
            ehdr._32.e_ident[EI_MAG1] != ELF_MAG1 ||
            ehdr._32.e_ident[EI_MAG2] != ELF_MAG2 ||
            ehdr._32.e_ident[EI_MAG3] != ELF_MAG3    ) {
        fprintf( PSTDOUT, "Wrong ELF magic bytes.\n" );
        return;
    }

    width      = elf_width( ehdr );
    endianness = elf_endianness( ehdr );

    fprintf( PSTDOUT, "\nELF header: " );

    switch( PELF( width, Ehdr, &ehdr, e_type ) ) {
    case ET_NONE :
        fprintf( PSTDOUT, "ET_NONE (no file type)" );
        break;
    case ET_REL :
        fprintf( PSTDOUT, "ET_REL (Relocatable)" );
        break;
    case ET_EXEC :
        fprintf( PSTDOUT, "ET_EXEC (Executable)" );
        break;
    case ET_DYN :
        fprintf( PSTDOUT, "ET_DYN (Shared objet)" );
        break;
    case ET_CORE :
        fprintf( PSTDOUT, "ET_CORE (Core file)" );
        break;
    case ET_LOPROC :
        fprintf( PSTDOUT, "ET_LOPROC (Processor specific)" );
        break;
    case ET_HIPROC :
        fprintf( PSTDOUT, "ET_HIPROC (Processor specific)" );
        break;
    default :
        fprintf( PSTDOUT, "Unknown type %u.", PELF( width, Ehdr, &ehdr, e_type ) );
        break;
    }

    fprintf( PSTDOUT, "\n" );

    fprintf( PSTDOUT, "%s, ", ELFCLASSNONE == width ? "ELFCLASSNONE" : (ELFCLASS32 == width ? "ELFCLASS32" : "ELFCLASS64") );

    fprintf( PSTDOUT, "%s\n", ELFDATANONE == endianness ? "ELFDATANONE" : (ELFDATA2MSB == endianness ? "ELFDATA2MSB" : "ELFDATA2LSB") );

    switch( PELF( width, Ehdr, &ehdr, e_machine ) ) {
        arch_print( NONE, "No machine" );
        arch_print( M32, "AT&T WE 32100" );
        arch_print( SPARC, "SPARC" );
        arch_print( 386, "Intel i386" );
        arch_print( 68K, "Motorola 68000" );
        arch_print( 88K, "Motorola 88000" );
        arch_print( IAMCU, "Intel MCU" );
        arch_print( 860, "Intel 80860" );
        arch_print( MIPS, "MIPS I" );
        arch_print( S370, "IBM S/370" );
        arch_print( MIPS_RS3_LE, "MIPS RS3000 Little-Endian" );
        arch_print( PPC, "PowerPC" );
        arch_print( PPC64, "64-bit PowerPC" );
        arch_print( ARM, "32-bit ARM" );
        arch_print( ALPHA, "Digital Alpha" );
        arch_print( SPARCV9, "SPARC Version 9" );
        arch_print( IA_64, "Intel IA-64" );
        arch_print( MIPS_X, "Stanford MIPS-X" );
        arch_print( COLDFIRE, "Motorola COldFire" );
        arch_print( 68HC12, "Motorola M68HC12" );
        arch_print( X86_64, "AMD x86-64" );
        arch_print( PDP10, "Digital Equipment Corp. PDP-10" );
        arch_print( PDP11, "Digital Equipment Corp. PDP-11" );
        arch_print( 68HC11, "Motorola M68HC11 Microcontroller" );
        arch_print( VAX, "Digital VAX" );
        arch_print( MMIX, "Donald Knuth's educational 64-bit processor" );
        arch_print( AARCH64, "64-bit ARM" );
        arch_print( MICROBLAZE, "Xilinx MicroBlaze 32-bit RISC soft processor core" );
        arch_print( CUDA, "NVidia CUDA" );
        arch_print( Z80, "Zilog Z80" );
        arch_print( AMDGPU, "AMD GPU" );
    default :
        fprintf( PSTDOUT, "Unknown machine (%d)", PELF( width, Ehdr, &ehdr, e_machine ) );
        break;
    }


    fprintf( PSTDOUT, "\n" );

}

void elf_print_shdr( Elf_Shdr shdr, Elf_Ehdr ehdr, unsigned char *shstrtab ) {

    int width = elf_width( ehdr );

    if ( SHT_NULL != PELF( width, Shdr, &shdr, sh_type ) ) {
        size_t sz = PELF( width, Shdr, &shdr, sh_size );

        if ( PELF( width, Shdr, &shdr, sh_size ) ) {
            if ( ELFCLASS64 == width ) {
                fprintf( PSTDOUT, "%s\t@ 0x%16lx+0x%lx\n",
                         (char*)&(shstrtab[PELF( width, Shdr, &shdr, sh_name )]),
                         PELF( width, Shdr, &shdr, sh_offset ),
                         sz );
            }
            else {
                fprintf( PSTDOUT, "%s\t@ 0x%08lx+0x%lx\n",
                         (char*)&(shstrtab[PELF( width, Shdr, &shdr, sh_name )]),
                         PELF( width, Shdr, &shdr, sh_offset ),
                         sz );
            }

            switch( PELF( width, Shdr, &shdr, sh_type ) ) {
            case SHT_STRTAB :
                fprintf( PSTDOUT, "    \tcontains string table.\n" );
                break;
            case SHT_SYMTAB :
            case SHT_DYNSYM :
                fprintf( PSTDOUT, "    \tcontains %lu symbols.\n", sz/PELF_SIZEOF( width, Sym ) );
                break;
            case SHT_REL :
                fprintf( PSTDOUT, "    \tcontains %lu relocation entries.\n", sz/PELF_SIZEOF( width, Rel ) );
                break;
            case SHT_RELA :
                fprintf( PSTDOUT, "    \tcontains %lu relocation entries (with addend).\n", sz/PELF_SIZEOF( width, Rela ) );
                break;
            default :
                break;
            }
        }
        else {
            fprintf( PSTDOUT, "empty %s.\n", (char*)&(shstrtab[PELF( width, Shdr, &shdr, sh_name )]) );
        }
    }
    else {
        fprintf( PSTDOUT, "SHN_UNDEF\n" );
    }
}

void elf_print_shtab( unsigned char *shtab, Elf_Ehdr ehdr, size_t offset, unsigned char *shstrtab ) {
    size_t i;

    int      width = elf_width( ehdr );
    size_t   shnum = PELF( width, Ehdr, &ehdr, e_shnum );
    PELF_DECLARE2( Shdr, shdr, shtab );

    if ( shnum ) {
        fprintf( PSTDOUT, "\n%lu sections:\n", shnum );

        for ( i = 0; i< shnum; PELF_NEXT2(shdr), i++ ) {
            fprintf( PSTDOUT, "[%2lu] ", i );

            if ( ELFCLASS64 == width ) {
                elf_print_shdr( (Elf_Shdr)((Elf64_Shdr*)shtab)[i], ehdr, shstrtab );
            }
            else {
                elf_print_shdr( (Elf_Shdr)((Elf32_Shdr*)shtab)[i], ehdr, shstrtab );
            }
        }
    }
}


void elf_print_rel( Elf_Rel rel, Elf_Ehdr ehdr, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab, unsigned char *symtab ) {
    int            width = elf_width( ehdr );
    unsigned int machine = PELF( width, Ehdr, &ehdr, e_machine );


    fprintf( PSTDOUT, "offset: 0x%lx, symbol: ", PELF( width, Rel, &rel, r_offset ) );

    if ( ELFCLASS64 == width ) {
        if ( ELF64_R_SYM( PELF( width, Rel, &rel, r_info ) ) < PELF( width, Ehdr, &ehdr, e_shnum ) ) {
            elf_print_sym( (Elf_Sym)((Elf64_Sym*)symtab)[ELF64_R_SYM(PELF( width, Rel, &rel, r_info ))], ehdr, strtab, shstrtab, shtab );
            fprintf( PSTDOUT, ", type: %s", elf_print_reltype( machine, ELF64_R_TYPE(PELF( width, Rel, &rel, r_info )) ) );
        }
        else {
            fprintf( PSTDOUT, "Relocation #%lu/%u?",
                     ELF64_R_SYM( PELF( width, Rel, &rel, r_info ) ),
                     PELF( width, Ehdr, &ehdr, e_shnum ) );
        }
    }
    else {
        if ( ELF32_R_SYM( PELF( width, Rel, &rel, r_info ) ) < PELF( width, Ehdr, &ehdr, e_shnum ) ) {
            elf_print_sym( (Elf_Sym)((Elf64_Sym*)symtab)[ELF32_R_SYM(PELF( width, Rel, &rel, r_info ))], ehdr, strtab, shstrtab, shtab );
        }
        else {
            fprintf( PSTDOUT, "Relocation #%lu/%u?",
                     ELF32_R_SYM( PELF( width, Rel, &rel, r_info ) ),
                     PELF( width, Ehdr, &ehdr, e_shnum ) );
        }
    }
}

void elf_print_rel_table( char *name, Elf_Ehdr ehdr, unsigned char *table, size_t length, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab, unsigned char *symtab ) {
    int          width = elf_width( ehdr );
    size_t           i = 0;

    if ( !table ) return;

    fprintf( PSTDOUT, "%s [%lu entr%s]:\n",
             name,
             length/PELF_SIZEOF( width, Rel ),
             1 == length/PELF_SIZEOF( width, Rel ) ? "y" : "ies" );

    for ( i = 0; i< length/PELF_SIZEOF( width, Rel ); i++ ) {
        fprintf( PSTDOUT, "[%2lu] ", i );
        if ( ELFCLASS64 == width ) {
            elf_print_rel( (Elf_Rel)((Elf64_Rel*)table)[i], ehdr, strtab, shstrtab, shtab, symtab );
        }
        else {
            elf_print_rel( (Elf_Rel)((Elf32_Rel*)table)[i], ehdr, strtab, shstrtab, shtab, symtab );
        }
        fprintf( PSTDOUT, "\n" );
    }
}

void elf_print_sym( Elf_Sym sym, Elf_Ehdr ehdr, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab ) {

    int width = elf_width( ehdr );
    PELF_DECLARE2( Shdr, shdr, shtab );

    if ( 0 == PELF( width, Sym, &sym, st_name )  &&
            0 == PELF( width, Sym, &sym, st_value ) &&
            0 == PELF( width, Sym, &sym, st_size )  &&
            0 == PELF( width, Sym, &sym, st_info )  &&
            0 == PELF( width, Sym, &sym, st_other ) &&
            SHN_UNDEF == PELF( width, Sym, &sym, st_shndx ) ) {
        fprintf( PSTDOUT, "STN_UNDEF" );
        return;
    }

    if ( SHN_UNDEF == PELF( width, Sym, &sym, st_shndx ) ) {
        fprintf( PSTDOUT, "%s\tUndefined", strtab+PELF( width, Sym, &sym, st_name ) );
        return;
    }

    if ( shtab ) {
        PELF_DECLARE2( Sym, sym, &sym );

        fprintf( PSTDOUT, "%s\t%lu bytes at %s+0x%lx",
                 strtab+PELF( width, Sym, &sym, st_name ),
                 PELF( width, Sym, &sym, st_size ),
                 ( PELF( width, Sym, &sym, st_shndx ) <= PELF( width, Ehdr, &ehdr, e_shnum ) ) ?
                 ( (char*)shstrtab+ ( ( ELFCLASS64 == width ) ?
                                      ((Elf64_Shdr*)shtab)[sym64->st_shndx].sh_name :
                                      ((Elf32_Shdr*)shtab)[sym32->st_shndx].sh_name ) ) : "Noname",
                 PELF( width, Sym, &sym, st_value ) );
    }
    else {
        fprintf( PSTDOUT, "%s\t%lu bytes",
                 strtab+PELF( width, Sym, &sym, st_name ),
                 PELF( width, Sym, &sym, st_size ) );
    }

    if ( ELFCLASS64 == width ) {
        fprintf( PSTDOUT, ", " );
        switch( ELF64_ST_BIND( PELF( width, Sym, &sym, st_info ) ) ) {
        case STB_LOCAL :
            fprintf( PSTDOUT, "local" );
            break;
        case STB_GLOBAL :
            fprintf( PSTDOUT, "global" );
            break;
        case STB_WEAK :
            fprintf( PSTDOUT, "weak" );
            break;
        case STB_LOPROC :
        case STB_HIPROC :
        default :
            fprintf( PSTDOUT, "Processor specific" );
            break;
        }

        fprintf( PSTDOUT, " " );
        switch( ELF64_ST_TYPE( PELF( width, Sym, &sym, st_info ) ) ) {
        case STT_NOTYPE :
            fprintf( PSTDOUT, "No type" );
            break;
        case STT_OBJECT :
            fprintf( PSTDOUT, "object" );
            break;
        case STT_FUNC :
            fprintf( PSTDOUT, "function" );
            break;
        case STT_SECTION :
            fprintf( PSTDOUT, "associated with section" );
            break;
        case STT_FILE :
            fprintf( PSTDOUT, "file" );
            break;
        case STT_LOPROC :
        case STT_HIPROC :
        default :
            fprintf( PSTDOUT, "Processor specific" );
            break;
        }
    }
    else {
        fprintf( PSTDOUT, ", " );
        switch( ELF32_ST_BIND( PELF( width, Sym, &sym, st_info ) ) ) {
        case STB_LOCAL :
            fprintf( PSTDOUT, "local" );
            break;
        case STB_GLOBAL :
            fprintf( PSTDOUT, "global" );
            break;
        case STB_WEAK :
            fprintf( PSTDOUT, "weak" );
            break;
        case STB_LOPROC :
        case STB_HIPROC :
        default :
            fprintf( PSTDOUT, "Processor specific" );
            break;
        }

        fprintf( PSTDOUT, " " );
        switch( ELF32_ST_TYPE( PELF( width, Sym, &sym, st_info ) ) ) {
        case STT_NOTYPE :
            fprintf( PSTDOUT, "No type" );
            break;
        case STT_OBJECT :
            fprintf( PSTDOUT, "object" );
            break;
        case STT_FUNC :
            fprintf( PSTDOUT, "function" );
            break;
        case STT_SECTION :
            fprintf( PSTDOUT, "associated with section" );
            break;
        case STT_FILE :
            fprintf( PSTDOUT, "file" );
            break;
        case STT_LOPROC :
        case STT_HIPROC :
        default :
            fprintf( PSTDOUT, "Processor specific" );
            break;
        }
    }
}

void elf_print_symbol_table( Elf_Ehdr ehdr, unsigned char *table, size_t length, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab ) {
    size_t       i = 0;
    int      width = elf_width( ehdr );
    PELF_DECLARE2( Sym, sym, table );

    if ( !table ) return;

    fprintf( PSTDOUT, ".symtab [%lu symbols]:\n", length/PELF_SIZEOF( width, Sym ) );

    for ( i = 0; i< length/PELF_SIZEOF( width, Sym ); PELF_NEXT2( sym ), i++ ) {
        fprintf( PSTDOUT, "[%2lu] ", i );
        if ( ELFCLASS64 == width ) {
            elf_print_sym( (Elf_Sym)*sym64, ehdr, strtab, shstrtab, shtab );
        }
        else {
            elf_print_sym( (Elf_Sym)*sym32, ehdr, strtab, shstrtab, shtab );
        }
        fprintf( PSTDOUT, "\n" );
    }
}

