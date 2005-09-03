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

/* Private stuff */

#ifndef __XCCP_H_
#define __XCCP_H_

#include "xcc.h"

typedef struct _XCC {
    XCCStack *a_types;
    XCCStack *e_types;
    XCCStack *elements;
    XCCString *preamble;
    XCCString *postamble;
    char *ns_uri;
    char *prefix;
} XCC;

typedef struct {
    char *ifile;
    char *ofile;
    FILE *ifp;
    FILE *ofp;
    int  bundle;
    int  schema;
} XCCOpts;


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
    EType *parent_etype;
    int same_parents;
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
    unsigned int minOccurs;
    unsigned int maxOccurs;
} Child;

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

int xcc_output_parser(const XCC *xcc, FILE *fp, int bundle);

int xcc_output_schema(const XCC *xcc, FILE *fp);

int xcc_parse_opts(XCCOpts *xopts, int argc, char * const argv[]);

#endif /* __XCCP_H_ */
