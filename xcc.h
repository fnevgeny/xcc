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

#ifndef RETURN_SUCCESS
# define RETURN_SUCCESS   0
#endif
#ifndef RETURN_FAILURE
# define RETURN_FAILURE   1
#endif

void xfree(void *p);
void *xmalloc(size_t size);
#define xrealloc realloc

char *xstrdup(const char *s);

/* ------------------- */

typedef struct _XStack {
    unsigned int size;
    unsigned int depth;
    void **entries;
} XStack;

XStack *xstack_new(void);
void xstack_free(XStack *xs);
int xstack_increment(XStack *xs, const void *data);
int xstack_decrement(XStack *xs);
int xstack_get_first(const XStack *xs, void **data);
int xstack_get_last(const XStack *xs, void **data);
int xstack_get_data(const XStack *xs, unsigned int ind, void **data);
int xstack_depth(const XStack *xs);

typedef struct _XString {
    unsigned int length;
    char *s;
} XString;

XString *xstring_new(void);
int xstring_set(XString *xstr, const char *s);

/* ------------------- */

typedef struct _XCC {
    XStack *a_types;
    XStack *e_types;
    XStack *elements;
    XString *preamble;
    XString *postamble;
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
    XStack *attributes;
    XStack *children;
    XString *data;
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
    char *type;
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

AType *get_atype_by_name(XStack *a_types, const char *name);
EType *get_etype_by_name(XStack *e_types, const char *name);

/* ------------------- */

typedef struct _ParserData {
    char *cbuffer;
    int cbufsize;
    int cbuflen;
    
    XStack *nodes;
    void *root;
    
    void *udata;
} ParserData;

int output_preamble(const XString *pre);
int output_postamble(const XString *post);
int output_atype_union(const XStack *a_types);
int output_etype_union(const XStack *e_types);
int output_element_tab(const XStack *elements);
int output_start_handler(const XStack *elements);
int output_end_handler(const XStack *elements);

void *xcc_get_root(ParserData *pdata);

void xcc_char_data_handler(void *data, const char *s, int len);

int xcc_parse(FILE *fp, void *udata, void **root,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler);

#endif /* __XCC_H_ */
