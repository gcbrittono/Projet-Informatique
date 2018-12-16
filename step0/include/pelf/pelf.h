
/**
 * @file pelf.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Sun Sep 24 22:07:08 2017
 * @brief PELF interface.
 *
 * PELF interface.
 */

#include <string.h>
#include <sys/param.h> /* host endianness */


#ifndef _PELF_H_
#define _PELF_H_

#ifdef __cplusplus
extern "C" {
#endif



#define PSTDOUT stdout

#if __BIG_ENDIAN == __BYTE_ORDER
#define PELF_HOST_ENDIANNESS ELFDATA2MSB
#else
#define PELF_HOST_ENDIANNESS ELFDATA2LSB
#endif

#include <pelf/elf.h>


typedef union {

    Elf32_Ehdr _32;
    Elf64_Ehdr _64;

} Elf_Ehdr;

typedef union {

    Elf32_Shdr _32;
    Elf64_Shdr _64;

} Elf_Shdr;

typedef union {

    Elf32_Sym _32;
    Elf64_Sym _64;

} Elf_Sym;

typedef union {

    Elf32_Rel  _32;
    Elf64_Rel  _64;

} Elf_Rel;

typedef union {

    Elf32_Rela _32;
    Elf64_Rela _64;

} Elf_Rela;

/*
  Going 32/64-bit agnostic.
 */

/* When pointing to a union */
#define PELF(width, type, varptr, field)				\
  ( ( ELFCLASS64 == width ) ?						\
    (((Elf64_##type*) (varptr) )->field) :				\
    (((Elf32_##type*) (varptr) )->field) )

#define PELF_SIZEOF( width, type )					\
  ( ( ELFCLASS64 == width ) ?						\
    sizeof( Elf64_##type ) : sizeof( Elf32_##type ) )


/* The trick is to syntactically treat two vars as if they
   were only one.
 */
#define PELF_DECLARE2( type, name, var )			\
	Elf32_##type * name##32 = (Elf32_##type *)(var);	\
	Elf64_##type * name##64 = (Elf64_##type *)(var)

#define PELF_NEXT2( name )			\
	name##32++, name##64++

/* e.g., when inside a loop, with 2 pointers ELF_DECLARE2'd */
#define PELF2(width, type, varptr, field)				\
  ( ( ELFCLASS64 == width ) ?						\
    ((Elf64_##type *) varptr##64)->field :				\
    ((Elf32_##type *) varptr##32)->field)

#define PELF_FLIP2( width, type, var )			\
  if ( ELFCLASS64 == width )				\
    elf64_endian_flip_##type ( var##64 );		\
  else							\
    elf32_endian_flip_##type ( var##32 )



/* Useful helpers */
#define elf_endianness( ehdr )			\
  ((Elf32_Ehdr *)&(ehdr))->e_ident[ EI_DATA ]

#define elf_width( ehdr )			\
  ((Elf32_Ehdr *)&(ehdr))->e_ident[ EI_CLASS ]

void elf32_endian_flip_Ehdr( Elf32_Ehdr *header );
void elf32_endian_flip_Shdr( Elf32_Shdr *header );
void elf32_endian_flip_Sym( Elf32_Sym *header );
void elf32_endian_flip_Rel( Elf32_Rel *rel );
void elf32_endian_flip_Rela( Elf32_Rela *rel );

void elf64_endian_flip_Ehdr( Elf64_Ehdr *header );
void elf64_endian_flip_Shdr( Elf64_Shdr *header );
void elf64_endian_flip_Sym( Elf64_Sym *header );
void elf64_endian_flip_Rel( Elf64_Rel *rel );
void elf64_endian_flip_Rela( Elf64_Rela *rel );

#define PELF_ERROR( string ) fprintf( stderr, "%s", string )




/*
  User interface begins here.
 */

/* ELF machine-dependent routines */
int   elf_reloc_num( unsigned int machine, char *rname );
int   elf_get_machine( char *machinename );
int   elf_get_machine_class( unsigned int machine );
int   elf_get_machine_endianness( unsigned int machine );
int   elf_get_machine_ehsize( unsigned int machine );
int   elf_get_machine_phentsize( unsigned int machine );
int   elf_get_machine_shentsize( unsigned int machine );

/* Navigate ELF string tables */
int   elf_get_string_offset( unsigned char *start, size_t size, char *name );
int   elf_get_string_index( unsigned char *start, size_t size, char *name );
char *elf_next_string( unsigned char *table, size_t length, char **str );

/* Read ELF sections */
int   elf_read_ehdr( FILE *fp, Elf_Ehdr *ehdr );
unsigned char *elf_read_section_by_name( FILE *fp, Elf_Ehdr ehdr, unsigned char *shstrtab, unsigned char *shtab, size_t *scnsz, char *scname );
unsigned char *elf_read_symtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *symtabsz );
unsigned char *elf_read_shtab( FILE *fp, Elf_Ehdr ehdr, size_t *shtabsz );
unsigned char *elf_read_shstrtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *shstrtabsz );
unsigned char *elf_read_strtab( FILE *fp, Elf_Ehdr ehdr, unsigned char *shtab, size_t *strtabsz );

/* Dump secname section content to disk */
int   elf_dump_section( FILE *fp, char *input, char *secname );

/* Print ELF information */
void elf_print_sym( Elf_Sym sym, Elf_Ehdr ehdr, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab );
void elf_print_symbol_table( Elf_Ehdr ehdr, unsigned char *table, size_t length, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab );
void elf_print_string_table( unsigned char *table, size_t length );
void elf_print_rel_table( char *name, Elf_Ehdr ehdr, unsigned char *table, size_t length, unsigned char *strtab, unsigned char *shstrtab, unsigned char *shtab, unsigned char *symtab );
void elf_print_ehdr( Elf_Ehdr ehdr );
void elf_print_shdr( Elf_Shdr shdr, Elf_Ehdr ehdr, unsigned char *shstrtab );
void elf_print_shtab( unsigned char *shtab, Elf_Ehdr ehdr, size_t offset, unsigned char *shstrtab );

/* Write ELF ET_REL on disk */
int   elf_write_relocatable( char *filename, char *machine, int noreorder,
                             unsigned char *text, unsigned int textsz,
                             unsigned char *data, unsigned int datasz,
                             unsigned char *bss, unsigned int bsssz,
                             unsigned char *shstrtab, unsigned int shstrtabsz,
                             unsigned char *strtab, unsigned int strtabsz,
                             unsigned char *symtab, unsigned int symtabsz,
                             unsigned char *reltext, unsigned int reltextsz,
                             unsigned char *reldata, unsigned int reldatasz );


/* Processor-specific definitions */
#define elf_catch_reloc(name, type)		\
  if ( !strcasecmp(#type, name) ) return type

#define elf_catch_reloc_name( type ) \
  case type :			     \
  return #type;			     \
  break;

#include <pelf/elf-mips32.h>


#ifdef __cplusplus
}
#endif

#endif /* _PELF_H_ */
