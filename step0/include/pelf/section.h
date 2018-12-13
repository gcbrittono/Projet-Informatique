
/**
 * @file section.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Wed Sep 20 11:10:15 2017
 * @brief Sections.
 *
 * Sections.
 */

#ifndef _SECTION_H_
#define _SECTION_H_

#define SECTION_CHUNK_SZ   1024

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    char           *name; /**! name of the section (e.g. : .text) */
    unsigned int      sz; /**! current size of the section (in number of SECTION_CHUNK_SZ) */
    unsigned char *start; /**! pointeur to the allocated raw data*/
} *section;

section new_section( char *name, unsigned int sz );
void    print_section( section s );
int     grow_section( section s, unsigned int increment );
int     write_section( section s, unsigned char *bytes, unsigned int nbytes, unsigned int offset);
void del_section( section s );

#ifdef __cplusplus
}
#endif

#endif /* _SECTION_H_ */
