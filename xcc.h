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

/* Public stuff */

#ifndef __XCC_H_
#define __XCC_H_

#include <stdio.h>
#include <expat.h>

#define XCC_VERSION_STRING  "xcc-0.0.9"

#define XCC_RETURN_SUCCESS   0
#define XCC_RETURN_FAILURE   1

/* ------------------- */

typedef struct _XCCStack XCCStack;

typedef struct _XCCString {
    unsigned int length;
    char *s;
} XCCString;

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
} XCCParserData;

typedef struct _Node {
    char *name;
    int  id;
    void *data;
} Node;

char *xcc_version_string(void);

void xcc_free(void *p);

void xcc_error(char *msg);

int xcc_stack_increment(XCCStack *xs, const void *data);
int xcc_stack_decrement(XCCStack *xs);
int xcc_stack_get_last(const XCCStack *xs, void **data);
int xcc_stack_depth(const XCCStack *xs);

XCCString *xcc_string_new(void);
void xcc_string_free(XCCString *xstr);
int xcc_string_set(XCCString *xstr, const char *s);

Node *node_new(void);

char *xcc_get_local(const char *name, const char *ns_uri, int *skip);

void *xcc_get_root(XCCParserData *pdata);

int xcc_parse(FILE *fp, void *udata, void **root,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler);

#endif /* __XCC_H_ */
