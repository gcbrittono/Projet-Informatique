
/**
 * @file section.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Wed Sep 20 11:15:28 2017
 * @author François Portet
 * @date Nov 20, 2018
 * @brief Sections.
 *
 * Sections.
 */

#include <stdlib.h>
#include <string.h>

#include <pelf/section.h>

#include <pelf/pelf.h>



/**
 * Creates a new section of a specific size and name
 * @param name name of the section to create
 * @param sz tentative size of the section
 * @return the section of at least sz size
 *
 * the section is a binary data structure that manages size by buffer of SECTION_CHUNK size
 */
section new_section( char *name, unsigned int sz ) {
    section s = calloc( 1, sizeof( *s ) );

    s->name  = strdup( name );
    s->sz    = sz <= SECTION_CHUNK_SZ ? SECTION_CHUNK_SZ : SECTION_CHUNK_SZ*(1+sz/SECTION_CHUNK_SZ);
    s->start = calloc( s->sz, sizeof( s->start[0] ) );
    s->sz    = 0;

    return s;
}

/**
 * @brief utility function to print out section information
 */
void    print_section( section s ) {
    unsigned int offset = 0;

    if ( 0 == s->sz ) {
        fprintf( stdout, "Section %s: empty.\n", s->name );
        return;
    }

    fprintf( stdout, "Section %s, %u bytes:\n", s->name, s->sz );

    for ( offset = 0; offset< s->sz; offset++ ) {
        if ( 0 == offset%4 ) {
            fprintf( stdout, "\n%08x\t", offset );
        }
        fprintf( stdout, "%02x", s->start[offset] );
    }

    for ( ; offset%4; offset++ ) {
        fprintf( stdout, "  " );
    }

    fprintf( stdout, "\n\n" );

}


/**
 * @brief utility function to move the current section data to a bigger space
 * @param s the section to grow
 * @param increment the number of bytes to add to the current size
 */
int     grow_section( section s, unsigned int increment ) {
    if ( s->sz % increment ) {
        s->start   = realloc( s->start, increment+s->sz );
        s->sz     += increment;
    }

    return 0;
}


/**
 * @brief writes data into the section
 * @param s the section to write data to
 * @param bytes the raw data to write into the section
 * @param offset offset from the start of the section to write to
 */

int     write_section( section s, unsigned char *bytes, unsigned int nbytes, unsigned int offset) {

    if( offset+nbytes > s->sz+(s->sz%SECTION_CHUNK_SZ) ) {
        unsigned int nsz = offset+nbytes;

        nsz        = nsz<SECTION_CHUNK_SZ ? SECTION_CHUNK_SZ : SECTION_CHUNK_SZ*(1+nsz/SECTION_CHUNK_SZ);
        s->start   = realloc( s->start, nsz );
        s->sz      = offset+nbytes;
    }

    memcpy( s->start+offset, bytes, nbytes );
    s->sz        = offset+nbytes;

    return 0;
}

/**
 * Frees all memory allocated to the section
 */

void del_section( section s ) {
    if ( s ) {
        free( s->name );
        free( s->start );
        free( s );
    }
}
