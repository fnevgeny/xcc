/*
 * XCC - XML Compiler-Compiler
 * 
 * Copyright (c) 2000-2002 Evgeny Stambulchik
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

/*
 * Hand-written parser for bootstrapping; works only with xcc.xcc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <expat.h>

#include <xcc.h>

typedef struct _bParserData {
    char *cbuffer;
    int cbufsize;
    int cbuflen;
    
    XCCStack *a_types;
    XCCStack *e_types;
    XCCString *preamble;
    XCCString *postamble;
    XCCStack *elements;
} bParserData;


static void register_attribute_type(XCCStack *a_types, const char **attr)
{
    int i;
    AType *atype;
    
    atype = atype_new();
    
    for (i = 0; attr[i]; i += 2) {
        if (!strcmp(attr[i], "name")) {
            atype->name = xstrdup(attr[i + 1]);
        } else
        if (!strcmp(attr[i], "ctype")) {
            atype->ctype = xstrdup(attr[i + 1]);
        }
    }
    xcc_stack_increment(a_types, atype);
}

static void register_element_type(XCCStack *e_types, const char **attr)
{
    int i;
    EType *etype;
    
    etype = etype_new();
    
    for (i = 0; attr[i]; i += 2) {
        if (!strcmp(attr[i], "name")) {
            etype->name = xstrdup(attr[i + 1]);
        } else
        if (!strcmp(attr[i], "ctype")) {
            etype->ctype = xstrdup(attr[i + 1]);
        }
    }
    xcc_stack_increment(e_types, etype);
}

static void register_element(XCCStack *elements, XCCStack *e_types, const char **attr)
{
    int i;
    Element *e;
    
    e = element_new();
    
    for (i = 0; attr[i]; i += 2) {
        if (!strcmp(attr[i], "name")) {
            e->name = xstrdup(attr[i + 1]);
        } else
        if (!strcmp(attr[i], "type")) {
            e->etype = get_etype_by_name(e_types, attr[i + 1]);
        }
    }
    xcc_stack_increment(elements, e);
}

static void register_element_attribute(Element *e, XCCStack *a_types, const char **attr)
{
    int i;
    Attribute *a;
    
    a = attribute_new();
    
    for (i = 0; attr[i]; i += 2) {
        if (!strcmp(attr[i], "name")) {
            a->name = xstrdup(attr[i + 1]);
        } else
        if (!strcmp(attr[i], "type")) {
            a->atype = get_atype_by_name(a_types, attr[i + 1]);
        }
    }

    xcc_stack_increment(e->attributes, a);
}

static void register_element_child(Element *e, const char **attr)
{
    int i;
    Child *c;
    
    c = child_new();
    
    for (i = 0; attr[i]; i += 2) {
        if (!strcmp(attr[i], "name")) {
            c->name = xstrdup(attr[i + 1]);
        }
    }

    xcc_stack_increment(e->children, c);
}

static void start(void *data, const char *el, const char **attr) {
    bParserData *pdata = (bParserData *) data;

    if (!strcmp(el, "parser")) {
        ;
    } else
    if (!strcmp(el, "attribute-type")) {
        register_attribute_type(pdata->a_types, attr);
    } else
    if (!strcmp(el, "element-type")) {
        register_element_type(pdata->e_types, attr);
    } else
    if (!strcmp(el, "element")) {
        register_element(pdata->elements, pdata->e_types, attr);
    } else
    if (!strcmp(el, "attribute")) {
        Element *e;
        xcc_stack_get_last(pdata->elements, (void **) &e);
        register_element_attribute(e, pdata->a_types, attr);
    } else
    if (!strcmp(el, "child")) {
        Element *e;
        xcc_stack_get_last(pdata->elements, (void **) &e);
        register_element_child(e, attr);
    } else
    if (!strcmp(el, "data")) {
        ;
    } else
    if (!strcmp(el, "preamble")) {
        ;
    } else
    if (!strcmp(el, "postamble")) {
        ;
    } else {
        printf("Unknown tag '%s'\n", el);
    }

    pdata->cbuflen = 0;
}

static void end(void *data, const char *el) {
    bParserData *pdata = (bParserData *) data;
    
    if (!strcmp(el, "attribute-type")) {
        AType *atype;
        xcc_stack_get_last(pdata->a_types, (void **) &atype);
        atype->ccode = xstrdup(pdata->cbuffer);
    } else
    if (!strcmp(el, "element-type")) {
        EType *etype;
        xcc_stack_get_last(pdata->e_types, (void **) &etype);
        etype->ccode = xstrdup(pdata->cbuffer);
    } else
    if (!strcmp(el, "attribute")) {
        Element *e;
        Attribute *a;
        xcc_stack_get_last(pdata->elements, (void **) &e);
        xcc_stack_get_last(e->attributes, (void **) &a);
        a->ccode = xstrdup(pdata->cbuffer);
    } else
    if (!strcmp(el, "child")) {
        Element *e;
        Child *c;
        xcc_stack_get_last(pdata->elements, (void **) &e);
        xcc_stack_get_last(e->children, (void **) &c);
        c->ccode = xstrdup(pdata->cbuffer);
    } else
    if (!strcmp(el, "data")) {
        Element *e;
        xcc_stack_get_last(pdata->elements, (void **) &e);
        xcc_string_set(e->data, pdata->cbuffer);
    } else
    if (!strcmp(el, "preamble")) {
        xcc_string_set(pdata->preamble, pdata->cbuffer);
    } else
    if (!strcmp(el, "postamble")) {
        xcc_string_set(pdata->postamble, pdata->cbuffer);
    }
    
    pdata->cbuflen = 0;
}

static void char_data_handler(void *data, const XML_Char *s, int len)
{
    bParserData *pdata = (bParserData *) data;
    int new_len;
    
    new_len = pdata->cbuflen + len;
    
    if (new_len >= pdata->cbufsize) {
        pdata->cbuffer = xcc_realloc(pdata->cbuffer, (new_len + 1));
        pdata->cbufsize = new_len + 1;
    }
    
    memcpy(pdata->cbuffer + pdata->cbuflen, s, len);
    pdata->cbuffer[new_len] = '\0';
    pdata->cbuflen = new_len;
}

#define BUFFSIZE	8192

int main(void) {
    XML_Parser xp;
    bParserData pdata;
    char Buff[BUFFSIZE];
    
    xp = XML_ParserCreate(NULL);
    if (!xp) {
        fprintf(stderr, "Couldn't allocate memory for parser\n");
        exit(-1);
    }

    /* Set user data */
    pdata.cbuffer  = NULL;
    pdata.cbufsize = 0;
    pdata.cbuflen  = 0;
 
    pdata.a_types   = xcc_stack_new(NULL);
    pdata.e_types   = xcc_stack_new(NULL);
    pdata.preamble  = xcc_string_new();
    pdata.postamble = xcc_string_new();
    pdata.elements  = xcc_stack_new(NULL);
    
    XML_SetUserData(xp, (void *) &pdata);

    XML_SetElementHandler(xp, start, end);

    /* Set the char data handler */
    XML_SetCharacterDataHandler(xp, char_data_handler) ;
       

    for (;;) {
        int done;
        int len;

        len = fread(Buff, 1, BUFFSIZE, stdin);
        if (ferror(stdin)) {
            fprintf(stderr, "Read error\n");
            exit(1);
        }
        done = feof(stdin);

        if (!XML_Parse(xp, Buff, len, done)) {
            fprintf(stderr, "Parse error at line %d:\n%s\n",
	            XML_GetCurrentLineNumber(xp),
	            XML_ErrorString(XML_GetErrorCode(xp)));
            exit(1);
        }

        if (done) {
            break;
        }
    }
    
    output_header();
    
    output_preamble(pdata.preamble);
    
    output_atype_union(pdata.a_types);
    output_etype_union(pdata.e_types);

    /* sort elements */
    output_element_tab(pdata.elements);

    output_start_handler(pdata.elements);
    
    output_end_handler(pdata.elements);

    output_postamble(pdata.postamble);

    exit(0);
}
