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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <expat.h>

#include "xccP.h"

void xcc_get_version_numbers(int *major, int *minor, int *nano)
{
    *major = XCC_VERSION_MAJOR;
    *minor = XCC_VERSION_MINOR;
    *nano  = XCC_VERSION_NANO;
}

char *xcc_get_version_string(void)
{
    return XCC_VERSION_STRING;
}

void xcc_error(const char *fmt, ...)
{
    va_list ap;
    char new_fmt[128];
    sprintf(new_fmt, "xcc: %s\n", fmt);

    va_start(ap, fmt);
    
    vfprintf(stderr, new_fmt, ap);
    va_end(ap);
}

void xcc_free(void *p)
{
    if (p) {
        free(p);
    }
}

void *xcc_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    } else {
        void *p = malloc(size);
        if (p) {
            return p;
        } else {
            fprintf(stderr, "Memory exhausted\n");
            abort();
        }
    }
}

char *xcc_strdup(const char *s)
{
    char *ret;
    if (s) {
        ret = xcc_malloc(strlen(s) + 1);
        if (ret) {
            strcpy(ret, s);
        }
    } else {
        ret = NULL;
    }
    return ret;
}

static unsigned int xcc_strlen(const char *s)
{
    if (s) {
        return strlen(s);
    } else {
        return 0;
    }
}

XCCStack *xcc_stack_new(XCC_stack_data_free data_free)
{
    XCCStack *xs = xcc_malloc(sizeof(XCCStack));
    
    if (xs) {
        xs->size      = 0;
        xs->depth     = 0;
        xs->entries   = NULL;
        xs->data_free = data_free;
    }
    
    return xs;
}

void xcc_stack_free(XCCStack *xs)
{
    if (xs) {
        while (xs->depth) {
            void *e;
            xs->depth--;
            e = xs->entries[xs->depth];
            if (e && xs->data_free) {
                xs->data_free(e);
            }
        }
        xcc_free(xs->entries);
        
        xcc_free(xs);
    }
}

int xcc_stack_increment(XCCStack *xs, const void *data)
{
    if (xs->size <= xs->depth) {
        unsigned int new_size = xs->size + XSTACK_CHUNK_SIZE;
        void **p = xcc_realloc(xs->entries, new_size*sizeof(void *));
        if (!p) {
            return XCC_RETURN_FAILURE;
        } else {
            xs->entries = p;
            xs->size = new_size;
        }
    }
    xs->entries[xs->depth] = (void *) data;
    xs->depth++;
    
    return XCC_RETURN_SUCCESS;
}

int xcc_stack_decrement(XCCStack *xs)
{
    if (xs->depth < 1) {
        return XCC_RETURN_FAILURE;
    } else {
        void *e;
        xs->depth--;
        e = xs->entries[xs->depth];
        if (e && xs->data_free) {
            xs->data_free(e);
        }
        
        return XCC_RETURN_SUCCESS;
    }
}

int xcc_stack_get_first(const XCCStack *xs, void **data)
{
    if (xs && xs->depth > 0) {
        *data = xs->entries[0];
        return XCC_RETURN_SUCCESS;
    } else {
        return XCC_RETURN_FAILURE;
    }
}

int xcc_stack_get_last(const XCCStack *xs, void **data)
{
    if (xs && xs->depth > 0) {
        *data = xs->entries[xs->depth - 1];
        return XCC_RETURN_SUCCESS;
    } else {
        return XCC_RETURN_FAILURE;
    }
}

int xcc_stack_get_data(const XCCStack *xs, unsigned int ind, void **data)
{
    if (xs && xs->depth > ind) {
        *data = xs->entries[ind];
        return XCC_RETURN_SUCCESS;
    } else {
        return XCC_RETURN_FAILURE;
    }
}

int xcc_stack_depth(const XCCStack *xs)
{
    return xs->depth;
}


XCCString *xcc_string_new(void)
{
    XCCString *xstr;
    
    xstr = xcc_malloc(sizeof(XCCString));
    if (xstr) {
        xstr->s = NULL;
        xstr->length = 0;
    }
    
    return xstr;
}

void xcc_string_free(XCCString *xstr)
{
    if (xstr) {
        xcc_free(xstr->s);
        xcc_free(xstr);
    }
}

int xcc_string_set(XCCString *xstr, const char *s)
{
    if (xstr) {
        xcc_free(xstr->s);
        xstr->s = xcc_strdup(s);
        xstr->length = xcc_strlen(xstr->s);
    }
    
    return XCC_RETURN_SUCCESS;
}

XCC *xcc_xcc_new(void)
{
    XCC *xcc;
    xcc = xcc_malloc(sizeof(XCC));
    memset(xcc, 0, sizeof(XCC));
    xcc->a_types = xcc_stack_new((XCC_stack_data_free) atype_free);
    xcc->e_types = xcc_stack_new((XCC_stack_data_free) etype_free);
    xcc->elements = xcc_stack_new((XCC_stack_data_free) element_free);
    return xcc;
}

void xcc_xcc_free(XCC *xcc)
{
    if (xcc) {
        xcc_stack_free(xcc->a_types);
        xcc_stack_free(xcc->e_types);
        xcc_stack_free(xcc->elements);
        xcc_string_free(xcc->preamble);
        xcc_string_free(xcc->postamble);
        xcc_free(xcc);
    }
}

AType *atype_new(void)
{
    AType *atype;
    atype = xcc_malloc(sizeof(AType));
    memset(atype, 0, sizeof(AType));
    return atype;
}

void atype_free(AType *atype)
{
    if (atype) {
        xcc_free(atype->name);
        xcc_free(atype->ctype);
        xcc_free(atype->ccode);
        xcc_free(atype);
    }
}

EType *etype_new(void)
{
    EType *etype;
    etype = xcc_malloc(sizeof(EType));
    memset(etype, 0, sizeof(EType));
    return etype;
}

void etype_free(EType *etype)
{
    if (etype) {
        xcc_free(etype->name);
        xcc_free(etype->ctype);
        xcc_free(etype->ccode);
        xcc_free(etype);
    }
}

Element *element_new(void)
{
    Element *e;
    e = xcc_malloc(sizeof(Element));
    memset(e, 0, sizeof(Element));
    e->attributes = xcc_stack_new((XCC_stack_data_free) attribute_free);
    e->children   = xcc_stack_new((XCC_stack_data_free) child_free);
    e->data       = xcc_string_new();
    return e;
}

void element_free(Element *e)
{
    if (e) {
        xcc_free(e->name);
        xcc_stack_free(e->attributes);
        xcc_stack_free(e->children);
        xcc_string_free(e->data);
        xcc_free(e);
    }
}

Attribute *attribute_new(void)
{
    Attribute *a;
    a = xcc_malloc(sizeof(Attribute));
    memset(a, 0, sizeof(Attribute));
    return a;
}

void attribute_free(Attribute *a)
{
    if (a) {
        xcc_free(a->name);
        xcc_free(a->ccode);
        xcc_free(a);
    }
}

Child *child_new(void)
{
    Child *c;
    c = xcc_malloc(sizeof(Child));
    memset(c, 0, sizeof(Child));
    return c;
}

void child_free(Child *c)
{
    if (c) {
        xcc_free(c->name);
        xcc_free(c->ccode);
        xcc_free(c);
    }
}

XCCNode *xcc_node_new(void)
{
    XCCNode *n;
    n = xcc_malloc(sizeof(XCCNode));
    memset(n, 0, sizeof(XCCNode));
    return n;
}

void xcc_node_free(XCCNode *n)
{
    if (n) {
        xcc_free(n->name);
        xcc_free(n);
    }
}

AType *get_atype_by_name(XCCStack *a_types, const char *name)
{
    int i, n_atypes = xcc_stack_depth(a_types);
    for (i = 0; i < n_atypes; i++) {
        AType *atype;
        void *p;
        xcc_stack_get_data(a_types, i, &p);
        atype = p;
        if (!strcmp(atype->name, name)) {
            return atype;
        }
    }
    
    return NULL;
}

EType *get_etype_by_name(XCCStack *e_types, const char *name)
{
    int i, n_etypes = xcc_stack_depth(e_types);
    for (i = 0; i < n_etypes; i++) {
        EType *etype;
        void *p;
        xcc_stack_get_data(e_types, i, &p);
        etype = p;
        if (!strcmp(etype->name, name)) {
            return etype;
        }
    }
    
    return NULL;
}

int output_header(void)
{
    printf("/* Produced by %s */\n\n", xcc_get_version_string());
    printf("#include <xcc.h>\n");
    return XCC_RETURN_SUCCESS;
}

int output_preamble(const XCCString *pre)
{
    if (pre && pre->s) {
        printf("%s\n", pre->s);
    }
    return XCC_RETURN_SUCCESS;
}

int output_postamble(const XCCString *post)
{
    if (post && post->s) {
        printf("%s\n", post->s);
    }
    return XCC_RETURN_SUCCESS;
}

int output_atype_union(const XCCStack *a_types)
{
    int i, n_atypes;
    
    n_atypes = xcc_stack_depth(a_types);
    printf("typedef union {\n");
    for (i = 0; i < n_atypes; i++) {
        AType *atype;
        void *p;
        xcc_stack_get_data(a_types, i, &p);
        atype = p;
        printf("    %s %s;\n", atype->ctype, atype->name);
    }
    printf("} XCCAType;\n\n");
    
    return XCC_RETURN_SUCCESS;
}

int output_etype_union(const XCCStack *e_types)
{
    int i, n_etypes;
    
    n_etypes = xcc_stack_depth(e_types);
    printf("typedef union {\n");
    for (i = 0; i < n_etypes; i++) {
        EType *etype;
        void *p;
        xcc_stack_get_data(e_types, i, &p);
        etype = p;
        printf("    %s %s;\n", etype->ctype, etype->name);
    }
    printf("    void * unicast;\n");
    printf("} XCCEType;\n\n");
    
    return XCC_RETURN_SUCCESS;
}

static int get_element_id_by_name(const XCCStack *elements, const char *name)
{
    int i, n_elements = xcc_stack_depth(elements);
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        xcc_stack_get_data(elements, i, &p);
        e = p;
        if (!strcmp(e->name, name)) {
            return e->id;
        }
    }
    
    return -1;
}

static char *replace(const char *str, const char *f, const char *r)
{
    int inlen, outlen, flen, rlen, diff;
    char *p, *rp, *p_old;
    char *retval;
    
    inlen  = strlen(str);
    flen   = strlen(f);
    rlen   = strlen(r);
    
    outlen = inlen + 1;
    
    p = strstr(str, f);
    while (p) {
        outlen += (rlen - flen);
        p += flen;
        p = strstr(p, f);
    }
    
    retval = xcc_malloc(outlen);
    rp = retval;
    
    p_old = (char *) str;
    p = strstr(str, f);
    while (p) {
        diff = p - p_old;
        if (diff) {
            memcpy(rp, p_old, diff);
            rp += diff;
        }
        memcpy(rp, r, rlen);
        rp += rlen;
        
        p  += flen;
        p_old = p;
        
        p = strstr(p_old, f);
    }
    
    diff = inlen - (p_old - str);
    if (diff) {
        memcpy(rp, p_old, diff);
    }
    
    retval[outlen - 1] = '\0';
    
    return retval;
}

char *xcc_get_local(const char *name, const char *ns_uri, int *skip)
{
    char *sep, *local_name;
    if (ns_uri && (sep = strchr(name, XCC_NS_SEPARATOR))) {
        char *buf = strstr(name, ns_uri);
        if (buf != name) {
            *skip = 1;
        }
        local_name = xcc_strdup(sep + 1);
    } else {
        local_name = xcc_strdup(name);
    }
    
    return local_name;
}

static char *print_sharp_name(const char *name)
{
    char *pname;
    
    if (!name) {
        pname = xcc_strdup("NULL");
    } else
    if (name[0] == '#') {
        pname = xcc_strdup(name + 1);
    } else {
        pname = xcc_malloc(strlen(name) + 3);
        if (pname) {
            sprintf(pname, "\"%s\"", name);
        }
    }
    
    return pname;
}

int output_element_tab(const XCCStack *elements)
{
    int i, n_elements;
    
    n_elements = xcc_stack_depth(elements);
    printf("static XCCElementEntry XCCElementTab[] = {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        char *pname;
        void *p;
        xcc_stack_get_data(elements, i, &p);
        e = p;
        e->id = i + 1;
        pname = print_sharp_name(e->name);
        printf("    {%d, %s}%s\n", e->id, pname, i == n_elements - 1 ? "":",");
        xcc_free(pname);
    }
    printf("};\n\n");

    printf("static int get_element_id_by_name(const char *name)\n");
    printf("{\n");
    printf("    int i;\n");
    printf("    for (i = 0; i < %d; i++) {\n", n_elements);
    printf("        if (!strcmp(XCCElementTab[i].name, name)) {\n");
    printf("            return XCCElementTab[i].key;\n");
    printf("        }\n");
    printf("    }\n");
    printf("    return -1;\n");
    printf("}\n\n");
    
    return XCC_RETURN_SUCCESS;
}

int output_start_handler(const XCCStack *elements, const char *ns_uri)
{
    int i, n_elements;
    char *pns_uri, *buf1, *buf2;

    n_elements = xcc_stack_depth(elements);

    printf("void xcc_start_handler(void *data, const char *el, const char **attr)\n");  
    printf("{\n");
    printf("    XCCParserData *pdata = (XCCParserData *) data;\n");
    printf("    XCCNode *pnode = NULL, *node;\n");
    printf("    XCCEType element;\n");
    printf("    XCCAType attribute;\n");
    printf("    int i, element_id, parent_id, parent_child, skip = 0;\n");
    printf("    const char *avalue;\n");
    printf("    char *aname, *el_local;\n");
    printf("\n");
    printf("    if (pdata->error) {\n");
    printf("        return;\n");
    printf("    }\n\n");
    printf("    pdata->cbuflen = 0;\n");
    printf("    if (pdata->cbufsize) {\n");
    printf("        pdata->cbuffer[0] = '\\0';\n");
    printf("    }\n");

    pns_uri = print_sharp_name(ns_uri);
    printf("    el_local  = xcc_get_local(el, %s, &skip);\n", pns_uri);
    printf("    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    printf("        parent_id = 0;\n");
    printf("    } else {\n");
    printf("        void *p;\n");
    printf("        xcc_stack_get_last(pdata->nodes, &p);\n");
    printf("        pnode = p;\n");
    printf("        parent_id = pnode->id;\n");
    printf("    }\n");
    printf("    if (parent_id < 0) {\n");
    printf("        skip = 1;\n");
    printf("    }\n");
    printf("    if (skip) {\n");
    printf("        element_id = -1;\n");
    printf("    } else {\n");
    printf("        element_id = get_element_id_by_name(el_local);\n");
    printf("    }\n");

    printf("    if (parent_id >= 0 && element_id >= 0) {\n");
    printf("        parent_child = %d*parent_id + element_id;\n", n_elements);
    printf("    } else {\n");
    printf("        parent_child = -1;\n");
    printf("    }\n\n");
    printf("    switch (parent_child) {\n");
    printf("    case 1:\n");
    
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_children, parent_id;
        
        xcc_stack_get_data(elements, i, &p);
        e = p;
        parent_id  = e->id;
        n_children = xcc_stack_depth(e->children);
        for (j = 0; j < n_children; j++) {
            Child *c;
            int element_id;
            xcc_stack_get_data(e->children, j, &p);
            c = p;
            element_id = get_element_id_by_name(elements, c->name);
            printf("    case %d:\n", n_elements*parent_id + element_id);
        }
    }
    
    printf("        break;\n");
    printf("    default:\n");
    printf("        if (!skip) {\n");
    printf("            xcc_error(\"unexpected \\\"%%s\\\"->\\\"%%s\\\" combination\", pnode ? pnode->name:\"xml\", el_local);\n");
    printf("            pdata->error = 1;\n");
    printf("        }\n");
    printf("        break;\n");
    printf("    }\n\n");

    printf("    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_attributes, element_id;
        char ebuf[128], abuf[128];
        
        xcc_stack_get_data(elements, i, &p);
        e = p;
        element_id  = e->id;

        sprintf(ebuf, "element.%s", e->etype->name);
        
        printf("    case %d: /* %s */\n", element_id, e->name);
        buf1 = replace(e->etype->ccode, "$$", ebuf);
        buf2 = replace(buf1, "$U", "pdata->udata");
        xcc_free(buf1);
        buf1 = replace(buf2, "$P", "pnode->data");
        xcc_free(buf2);
        printf("            %s\n", buf1);
        xcc_free(buf1);
        n_attributes = xcc_stack_depth(e->attributes);
        printf("            for (i = 0; attr[i]; i += 2) {\n");
        printf("                int askip = 0;\n");
        printf("                aname  = xcc_get_local(attr[i], %s, &askip);\n",
            pns_uri);
        printf("                avalue = attr[i + 1];\n");
        for (j = 0; j < n_attributes; j++) {
            Attribute *a;
            char *pname;

            xcc_stack_get_data(e->attributes, j, &p);
            a = p;

            pname = print_sharp_name(a->name);
            printf("                if (!strcmp(aname, %s)) {\n", pname);
            xcc_free(pname);
            sprintf(abuf, "attribute.%s", a->atype->name);
            buf1 = replace(a->atype->ccode, "$$", abuf);
            buf2 = replace(buf1, "$?", "avalue");
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            printf("                    %s\n", buf2);
            xcc_free(buf2);
            buf1 = replace(a->ccode, "$$", ebuf);
            buf2 = replace(buf1, "$?", abuf);
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            printf("                {\n");
            printf("                        %s\n", buf2);
            printf("                }\n");
            xcc_free(buf2);
            printf("                } else\n");
        }
        printf("                if (!askip) {\n");
        printf("                    xcc_error(\"unknown attribute \\\"%%s\\\" of element \\\"%%s\\\"\", aname, el_local);\n");
        printf("                }\n");
        printf("                xcc_free(aname);\n");
        printf("            }\n");
        printf("        break;\n");
    }

    xcc_free(pns_uri);

    printf("    default:\n");
    printf("        element.unicast = NULL;\n");
    printf("        if (!skip) {\n");
    printf("            xcc_error(\"unknown element \\\"%%s\\\"\", el_local);\n");
    printf("            pdata->error = 1;\n");
    printf("        }\n");
    printf("        break;\n");
    printf("    }\n\n");

    
    printf("    node = xcc_node_new();\n");
    printf("    node->name = el_local;\n");
    printf("    node->id = element_id;\n");
    printf("    node->data = element.unicast;\n");
    
    printf("    xcc_stack_increment(pdata->nodes, node);\n");
    
    printf("}\n\n");
    
    return XCC_RETURN_SUCCESS;
}

static Element *get_element_by_name(const XCCStack *elements, const char *name)
{
    int i, n_elements = xcc_stack_depth(elements);
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        xcc_stack_get_data(elements, i, &p);
        e = p;
        if (!strcmp(e->name, name)) {
            return e;
        }
    }
    
    return NULL;
}

int output_end_handler(const XCCStack *elements)
{
    int i, n_elements;
    char *buf1, *buf2;
    char pbuf[128], ebuf[128];

    n_elements = xcc_stack_depth(elements);

    printf("void xcc_end_handler(void *data, const char *el)\n");  
    printf("{\n");
    printf("    XCCParserData *pdata = (XCCParserData *) data;\n");
    printf("    XCCNode *node, *pnode;\n");
    printf("    void *p;\n");
    printf("    int element_id, parent_id, parent_child, skip = 0;\n");

    printf("    XCCEType element, pelement;\n");
    printf("    char *cdata = pdata->cbuffer;\n\n");
    printf("    if (pdata->error) {\n");
    printf("        return;\n");
    printf("    }\n\n");
    printf("    xcc_stack_get_last(pdata->nodes, &p);\n");
    printf("    node = p;\n");
    printf("    element_id = node->id;\n");
    printf("    element.unicast = node->data;\n");

    printf("    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int element_id;

        xcc_stack_get_data(elements, i, &p);
        e = p;
        if (e->data->s != NULL) {
            sprintf(ebuf, "element.%s", e->etype->name);
            element_id  = e->id;
            printf("    case %d:\n", element_id);
            printf("        {\n");
            buf1 = replace(e->data->s, "$$", ebuf);
            buf2 = replace(buf1, "$?", "cdata");
            printf("            %s\n", buf2);
            xcc_free(buf1);
            xcc_free(buf2);
            printf("        }\n");
            printf("        break;\n");
        }
    }
        
    printf("    }\n\n");

    printf("    xcc_stack_decrement(pdata->nodes);\n");

    printf("    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    printf("        pdata->root = element.unicast;\n");
    printf("        parent_id  = 0;\n");
    printf("        pelement.unicast = NULL;\n");
    printf("    } else {\n");
    printf("        xcc_stack_get_last(pdata->nodes, &p);\n");
    printf("        pnode = p;\n");
    printf("        parent_id  = pnode->id;\n");
    printf("        pelement.unicast = pnode->data;\n");
    printf("    }\n");

    printf("    if (parent_id >= 0 && element_id >= 0) {\n");
    printf("        parent_child = %d*parent_id + element_id;\n", n_elements);
    printf("    } else {\n");
    printf("        parent_child = -1;\n");
    printf("        skip = 1;\n");
    printf("    }\n\n");
    printf("    switch (parent_child) {\n");
    printf("    case 1:\n");
    printf("        break;\n");
    
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_children, parent_id;
        
        xcc_stack_get_data(elements, i, &p);
        e = p;
        parent_id  = e->id;
        n_children = xcc_stack_depth(e->children);
        sprintf(pbuf, "pelement.%s", e->etype->name);
        for (j = 0; j < n_children; j++) {
            Child *c;
            Element *ce;
            xcc_stack_get_data(e->children, j, &p);
            c = p;
            ce = get_element_by_name(elements, c->name);
            sprintf(ebuf, "element.%s", ce->etype->name);
            printf("    case %d:\n", n_elements*parent_id + ce->id);
            printf("        {\n");
            buf1 = replace(c->ccode, "$$", pbuf);
            buf2 = replace(buf1, "$?", ebuf);
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            printf("        %s\n", buf2);
            xcc_free(buf2);
            printf("        }\n");
            printf("        break;\n");
        }
    }

    printf("    default:\n");
    printf("        if (!skip) {\n");
    printf("            xcc_error(\"internal error\");\n");
    printf("            pdata->error = 1;\n");
    printf("        }\n");
    printf("        break;\n");
    printf("    }\n\n");

    printf("    pdata->cbuflen = 0;\n");
    printf("    if (pdata->cbufsize) {\n");
    printf("        pdata->cbuffer[0] = '\\0';\n");
    printf("    }\n");
    printf("}\n");
    
    return XCC_RETURN_SUCCESS;
}

void *xcc_get_root(XCCParserData *pdata)
{
    void *p;
    if (xcc_stack_get_data(pdata->nodes, 0, &p) != XCC_RETURN_SUCCESS) {
        return NULL;
    } else {
        XCCNode *node = p;
        return node->data;
    }
}

void xcc_char_data_handler(void *data, const char *s, int len)
{
    XCCParserData *pdata = (XCCParserData *) data;
    int new_len;

    if (pdata->error) {
        return;
    }
    
    new_len = pdata->cbuflen + len;
    
    if (new_len >= pdata->cbufsize) {
        pdata->cbuffer = xcc_realloc(pdata->cbuffer, (new_len + 1));
        pdata->cbufsize = new_len + 1;
    }
    
    memcpy(pdata->cbuffer + pdata->cbuflen, s, len);
    pdata->cbuffer[new_len] = '\0';
    pdata->cbuflen = new_len;
}

int xcc_parse(FILE *fp, void *udata, void **root,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler)
{
    XML_Parser xp;
    XCCParserData pdata;
    char Buff[BUFFSIZE];
    
    xp = XML_ParserCreateNS(NULL, XCC_NS_SEPARATOR);
    if (!xp) {
        *root = NULL;
        xcc_error("Couldn't allocate memory for parser");
        return XCC_RETURN_FAILURE;
    }

    /* Set XML_Parser's user data */
    pdata.error    = 0;
    
    pdata.cbuffer  = NULL;
    pdata.cbufsize = 0;
    pdata.cbuflen  = 0;
 
    pdata.nodes    = xcc_stack_new((XCC_stack_data_free) xcc_node_free);
    pdata.root     = NULL;
    
    pdata.udata    = udata;
    
    XML_SetUserData(xp, (void *) &pdata);

    XML_SetElementHandler(xp, start_element_handler, end_element_handler);

    /* Set the char data handler */
    XML_SetCharacterDataHandler(xp, xcc_char_data_handler);

    while (!pdata.error) {
        int done;
        int len;

        len = fread(Buff, 1, BUFFSIZE, fp);
        if (ferror(fp)) {
            xcc_error("Read error");
            pdata.error = 1;
            break;
        }
        done = feof(fp);

        if (!XML_Parse(xp, Buff, len, done)) {
            fprintf(stderr, "Parse error at line %d:\n%s\n",
	            XML_GetCurrentLineNumber(xp),
	            XML_ErrorString(XML_GetErrorCode(xp)));
            pdata.error = 1;
            break;
        }

        if (done) {
            break;
        }
    }
    
    /* Free allocated storage */
    XML_ParserFree(xp);
    xcc_stack_free(pdata.nodes);
    xcc_free(pdata.cbuffer);
    
    *root = pdata.root;
    if (!pdata.error) {
        return XCC_RETURN_SUCCESS;
    } else {
        return XCC_RETURN_FAILURE;
    }
}
