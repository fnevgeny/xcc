/*
 * XCC - XML Compiler-Compiler
 * 
 * Copyright (c) 2000-2004 Evgeny Stambulchik
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

/* Private stuff */

#ifndef __XCCP_H_
#define __XCCP_H_

#include "xcc.h"

#define XCC_NS_SEPARATOR    '|'

#define XSTACK_CHUNK_SIZE   16

#define BUFFSIZE	    8192

typedef struct _XCC {
    XCCStack *a_types;
    XCCStack *e_types;
    XCCStack *elements;
    XCCString *preamble;
    XCCString *postamble;
    char *ns_uri;
} XCC;

typedef void (*XCC_stack_data_free)(void *data); 

struct _XCCStack {
    unsigned int size;
    unsigned int depth;
    void **entries;
    XCC_stack_data_free data_free;
};

typedef struct _AType {
    char *name;
    char *ctype;
    char *ccode;
} AType;

typedef struct _EType {
    char *name;
    char *ctype;
    char *ccode;
} EType;

typedef struct _Element {
    char *name;
    EType *etype;
    XCCStack *attributes;
    XCCStack *children;
    XCCString *data;
    int id;
} Element;

typedef struct _Attribute {
    char *name;
    AType *atype;
    char *ccode;
} Attribute;

typedef struct _Child {
    char *name;
    char *ccode;
} Child;

void *xcc_malloc(size_t size);
#define xcc_realloc realloc

char *xcc_strdup(const char *s);

AType *atype_new(void);
void atype_free(AType *atype);

EType *etype_new(void);
void etype_free(EType *etype);

Element *element_new(void);
void element_free(Element *e);

Attribute *attribute_new(void);
void attribute_free(Attribute *a);

Child *child_new(void);
void child_free(Child *c);

void xcc_node_free(XCCNode *n);

XCCStack *xcc_stack_new(XCC_stack_data_free data_free);
void xcc_stack_free(XCCStack *xs);
int xcc_stack_get_data(const XCCStack *xs, unsigned int ind, void **data);
int xcc_stack_get_first(const XCCStack *xs, void **data);

AType *get_atype_by_name(XCCStack *a_types, const char *name);
EType *get_etype_by_name(XCCStack *e_types, const char *name);

XCC *xcc_xcc_new(void);
void xcc_xcc_free(XCC *xcc);

void xcc_char_data_handler(void *data, const char *s, int len);

int output_header(void);
int output_preamble(const XCCString *pre);
int output_postamble(const XCCString *post);
int output_atype_union(const XCCStack *a_types);
int output_etype_union(const XCCStack *e_types);
int output_element_tab(const XCCStack *elements);
int output_start_handler(const XCCStack *elements, const char *ns_uri);
int output_end_handler(const XCCStack *elements);

#endif /* __XCCP_H_ */