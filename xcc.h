/*
 * XCC - XML Compiler-Compiler
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2000 Evgeny Stambulchik
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

#ifndef __XCC_H_
#define __XCC_H_

#include <stdio.h>
#include <expat.h>

#define XCC_VERSION_STRING  "xcc-0.0.1"

#define XCC_RETURN_SUCCESS   0
#define XCC_RETURN_FAILURE   1

char *xcc_version_string(void);

void xcc_free(void *p);
void *xcc_malloc(size_t size);
#define xcc_realloc realloc

char *xstrdup(const char *s);

/* ------------------- */

typedef struct _XCCStack {
    unsigned int size;
    unsigned int depth;
    void **entries;
} XCCStack;

XCCStack *xcc_stack_new(void);
void xcc_stack_free(XCCStack *xs);
int xcc_stack_increment(XCCStack *xs, const void *data);
int xcc_stack_decrement(XCCStack *xs);
int xcc_stack_get_first(const XCCStack *xs, void **data);
int xcc_stack_get_last(const XCCStack *xs, void **data);
int xcc_stack_get_data(const XCCStack *xs, unsigned int ind, void **data);
int xcc_stack_depth(const XCCStack *xs);

typedef struct _XCCString {
    unsigned int length;
    char *s;
} XCCString;

XCCString *xcc_string_new(void);
void xcc_string_free(XCCString *xstr);
int xcc_string_set(XCCString *xstr, const char *s);

/* ------------------- */

typedef struct _XCC {
    XCCStack *a_types;
    XCCStack *e_types;
    XCCStack *elements;
    XCCString *preamble;
    XCCString *postamble;
} XCC;

typedef struct _XCCElementEntry {
    int key;
    char *name;
} XCCElementEntry;

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

typedef struct _Node {
    char *name;
    int  id;
    void *data;
} Node;

typedef struct _Attribute {
    char *name;
    AType *atype;
    char *ccode;
} Attribute;

typedef struct _Child {
    char *name;
    char *ccode;
} Child;

XCC *xcc_new(void);
AType *atype_new(void);
EType *etype_new(void);
Element *element_new(void);
Attribute *attribute_new(void);
Child *child_new(void);
Node *node_new(void);

void xcc_error(char *msg);

AType *get_atype_by_name(XCCStack *a_types, const char *name);
EType *get_etype_by_name(XCCStack *e_types, const char *name);

/* ------------------- */

typedef struct _XCCParserData {
    char *cbuffer;
    int cbufsize;
    int cbuflen;
    
    XCCStack *nodes;
    void *root;
    
    void *udata;
} XCCParserData;

int output_preamble(const XCCString *pre);
int output_postamble(const XCCString *post);
int output_atype_union(const XCCStack *a_types);
int output_etype_union(const XCCStack *e_types);
int output_element_tab(const XCCStack *elements);
int output_start_handler(const XCCStack *elements);
int output_end_handler(const XCCStack *elements);

void *xcc_get_root(XCCParserData *pdata);

void xcc_char_data_handler(void *data, const char *s, int len);

int xcc_parse(FILE *fp, void *udata, void **root,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler);

#endif /* __XCC_H_ */
