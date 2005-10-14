/*
 * XCC - XML Compiler-Compiler
 * 
 * Copyright (c) 2000-2005 Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* As a special exception, when this file is SOLELY used to provide the
   functionality as needed by an XCC-generated output at run-time (either
   copied by XCC into the output file or otherwise linked to), you may use
   this file without further restrictions, provided that its contents are
   preserved verbatim. */

/* Public stuff */

#ifndef __XCC_H_
#define __XCC_H_

#include <stdio.h>
#include <string.h>
#include <expat.h>

#define XCC_VERSION_MAJOR   0
#define XCC_VERSION_MINOR   5
#define XCC_VERSION_NANO    0
#define XCC_VERSION_STRING  "xcc-0.5.0"

#define XCC_RETURN_SUCCESS   0
#define XCC_RETURN_FAILURE   1

#define XCC_NS_SEPARATOR    '|'

#define XCC_STACK_CHUNK_SIZE  16

#define XCC_BUFFSIZE        8192

#define XCC_DEFAULT_PREFIX    "xcc"


/* error codes */
#define XCC_ECNTX   1
#define XCC_EATTR   2
#define XCC_EELEM   3
#define XCC_EEMIN   4
#define XCC_EEMAX   5
#define XCC_EAREQ   6
#define XCC_EINTR   7

/* ------------------- */

#define xcc_realloc realloc

typedef void (*XCC_stack_data_free)(void *data); 

typedef int (*XCCExceptionHandler)(int ierrno,
    const char *entity, const char *context, void *udata);

typedef struct _XCCStack {
    unsigned int size;
    unsigned int depth;
    void **entries;
    XCC_stack_data_free data_free;
} XCCStack;

typedef struct _XCCElementEntry {
    int key;
    char *name;
} XCCElementEntry;

typedef struct _XCCParserData {
    int error;
    
    char *cbuffer;
    int cbufsize;
    int cbuflen;
    
    XCCStack *nodes;
    void *root;
    
    void *udata;
    
    XCCExceptionHandler exception_handler;

    XML_Parser parser;

} XCCParserData;

typedef struct {
    int          allowed;
    unsigned int minOccurs;
    unsigned int maxOccurs;
    unsigned int occurred;
} XCCOccurrence;

typedef struct _XCCNode {
    char *name;
    int  id;
    void *data;
    XCCOccurrence *occurrence;
} XCCNode;

void xcc_get_version_numbers(int *major, int *minor, int *nano);
char *xcc_get_version_string(void);

void *xcc_malloc(size_t size);
void xcc_free(void *p);

void xcc_error(const char *fmt, ...);

int xcc_stack_increment(XCCStack *xs, const void *data);
int xcc_stack_decrement(XCCStack *xs);
int xcc_stack_get_last(const XCCStack *xs, void **data);
int xcc_stack_depth(const XCCStack *xs);

XCCNode *xcc_node_new(void);

char *xcc_get_local(const char *name, const char *ns_uri, int *skip);

void *xcc_get_root(XCCParserData *pdata);

int xcc_run(FILE *fp, void **root, void *udata,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler,
              XCCExceptionHandler exception_handler);

#endif /* __XCC_H_ */
