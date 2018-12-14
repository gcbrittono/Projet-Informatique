
/**
 * @file endian.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Sep 22 18:43:12 2017
 * @brief Endianness stuff.
 *
 * Endianness stuff.
 */

#include <pelf/elf.h>

static void elf_endian_flip16( unsigned short *v ) {
    unsigned char *p = (unsigned char *)v;
    unsigned char  s = p[1];
    p[1]             = p[0];
    p[0]             = s;
}

static void elf_endian_flip32( unsigned int *v ) {
    unsigned char *p = (unsigned char *)v;
    unsigned char  s = p[3];
    p[3]             = p[0];
    p[0]             = s;

    s                = p[2];
    p[2]             = p[1];
    p[1]             = s;
}

static void elf_endian_flip64( size_t *v ) {
    unsigned char *p = (unsigned char *)v;
    unsigned char  s = p[7];
    p[7]             = p[0];
    p[0]             = s;

    s                = p[6];
    p[6]             = p[1];
    p[1]             = s;

    s                = p[5];
    p[5]             = p[2];
    p[2]             = s;

    s                = p[4];
    p[4]             = p[3];
    p[3]             = s;
}

void elf32_endian_flip_Ehdr( Elf32_Ehdr *header ) {
    elf_endian_flip16( &(header->e_type) );
    elf_endian_flip16( &(header->e_machine) );
    elf_endian_flip32( &(header->e_version) );
    elf_endian_flip32( &(header->e_entry) );
    elf_endian_flip32( &(header->e_phoff) );
    elf_endian_flip32( &(header->e_shoff) );
    elf_endian_flip32( &(header->e_flags) );
    elf_endian_flip16( &(header->e_ehsize) );
    elf_endian_flip16( &(header->e_phentsize) );
    elf_endian_flip16( &(header->e_phnum) );
    elf_endian_flip16( &(header->e_shentsize) );
    elf_endian_flip16( &(header->e_shnum) );
    elf_endian_flip16( &(header->e_shstrndx) );
}

void elf32_endian_flip_Shdr( Elf32_Shdr *header ) {
    elf_endian_flip32( &(header->sh_name) );
    elf_endian_flip32( &(header->sh_type) );
    elf_endian_flip32( &(header->sh_flags) );
    elf_endian_flip32( &(header->sh_addr) );
    elf_endian_flip32( &(header->sh_offset) );
    elf_endian_flip32( &(header->sh_size) );
    elf_endian_flip32( &(header->sh_link) );
    elf_endian_flip32( &(header->sh_info) );
    elf_endian_flip32( &(header->sh_addralign) );
    elf_endian_flip32( &(header->sh_entsize) );
}


void elf64_endian_flip_Ehdr( Elf64_Ehdr *header ) {
    elf_endian_flip16( &(header->e_type) );
    elf_endian_flip16( &(header->e_machine) );
    elf_endian_flip32( &(header->e_version) );
    elf_endian_flip64( &(header->e_entry) );
    elf_endian_flip64( &(header->e_phoff) );
    elf_endian_flip64( &(header->e_shoff) );
    elf_endian_flip32( &(header->e_flags) );
    elf_endian_flip16( &(header->e_ehsize) );
    elf_endian_flip16( &(header->e_phentsize) );
    elf_endian_flip16( &(header->e_phnum) );
    elf_endian_flip16( &(header->e_shentsize) );
    elf_endian_flip16( &(header->e_shnum) );
    elf_endian_flip16( &(header->e_shstrndx) );
}

void elf64_endian_flip_Shdr( Elf64_Shdr *header ) {
    elf_endian_flip32( &(header->sh_name) );
    elf_endian_flip32( &(header->sh_type) );
    elf_endian_flip32( (unsigned int *)&(header->sh_flags) );
    elf_endian_flip32( (unsigned int *)&(header->sh_addr) );
    elf_endian_flip32( (unsigned int *)&(header->sh_offset) );
    elf_endian_flip32( (unsigned int *)&(header->sh_size) );
    elf_endian_flip32( &(header->sh_link) );
    elf_endian_flip32( &(header->sh_info) );
    elf_endian_flip32( (unsigned int *)&(header->sh_addralign) );
    elf_endian_flip32( (unsigned int *)&(header->sh_entsize) );
}

void elf32_endian_flip_Sym( Elf32_Sym *sym ) {
    elf_endian_flip32( &(sym->st_name) );
    elf_endian_flip16( &(sym->st_shndx) );
    elf_endian_flip32( &(sym->st_value) );
    elf_endian_flip32( &(sym->st_size) );
}

void elf32_endian_flip_Rel( Elf32_Rel *rel ) {
    elf_endian_flip32( &(rel->r_offset) );
    elf_endian_flip32( &(rel->r_info) );
}

void elf32_endian_flip_Rela( Elf32_Rela *rel ) {
    elf_endian_flip32( &(rel->r_offset) );
    elf_endian_flip32( &(rel->r_info) );
    elf_endian_flip32( &(rel->r_addend) );
}


void elf64_endian_flip_Sym( Elf64_Sym *sym ) {
    elf_endian_flip32( &(sym->st_name) );
    elf_endian_flip16( &(sym->st_shndx) );
    elf_endian_flip64( &(sym->st_value) );
    elf_endian_flip64( &(sym->st_size) );
}

void elf64_endian_flip_Rel( Elf64_Rel *rel ) {
    return elf64_endian_flip_Rel( (Elf64_Rel*)rel );
}

void elf64_endian_flip_Rela( Elf64_Rela *rel ) {
    elf_endian_flip64( &(rel->r_offset) );
    elf_endian_flip64( &(rel->r_info) );
    elf_endian_flip64( &(rel->r_addend) );
}


