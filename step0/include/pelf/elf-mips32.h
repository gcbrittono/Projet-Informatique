
/**
 * @file elf32-mips.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Sat Nov  2 19:48:32 2013
 * @brief ELF 32 bit for MIPS.
 *
 * ELF 32 bit for MIPS.
 */

#ifndef _ELF32_MIPS_H_
#define _ELF32_MIPS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pelf/pelf.h>


#define EF_MIPS_NORERODER      0x00000001
#define EF_MIPS_PIC            0x00000002
#define EF_MIPS_CPIC           0x00000004
#define EF_MIPS_ARCH           0xf0000000

#define R_MIPS_NONE            0
#define R_MIPS_16              1
#define R_MIPS_32              2
#define R_MIPS_REL32           3
#define R_MIPS_26              4
#define R_MIPS_HI16            5
#define R_MIPS_LO16            6
#define R_MIPS_GPREL16         7
#define R_MIPS_LITERAL         8
#define R_MIPS_GOT16           9
#define R_MIPS_PC16           10
#define R_MIPS_CALL16         11
#define R_MIPS_GPREL32        12
#define R_MIPS_GOTHI16        21
#define R_MIPS_GOTLO16        22
#define R_MIPS_CALLHI16       30
#define R_MIPS_CALLLO16       31

/*
  End address for memory models: 4GB (large) or 256MB (small)
 */
#define ELF32_MIPS_MEM_LARGE    0x00000000
#define ELF32_MIPS_MEM_SMALL    0x10000000


int   elf_mips_reloc_num( char *rname );
char *elf_mips_print_reltype( unsigned int type );

#ifdef __cplusplus
}
#endif

#endif /* _ELF32_MIPS_H_ */
