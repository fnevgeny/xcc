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

/*
 * Library routines for XCC executable itself
 */

#include <string.h>
#include <getopt.h>

#include "xccP.h"
#include "bundle.i"

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

int output_header(FILE *fp)
{
    fprintf(fp, "/* Produced by %s */\n\n", xcc_get_version_string());
    fprintf(fp, "#include <xcc.h>\n");
    return XCC_RETURN_SUCCESS;
}

int output_preamble(const XCCString *pre, FILE *fp)
{
    if (pre && pre->s) {
        fprintf(fp, "%s\n", pre->s);
    }
    return XCC_RETURN_SUCCESS;
}

int output_postamble(const XCCString *post, FILE *fp)
{
    if (post && post->s) {
        fprintf(fp, "%s\n", post->s);
    }
    return XCC_RETURN_SUCCESS;
}

int output_atype_union(const XCCStack *a_types, FILE *fp)
{
    int i, n_atypes;
    
    n_atypes = xcc_stack_depth(a_types);
    fprintf(fp, "typedef union {\n");
    for (i = 0; i < n_atypes; i++) {
        AType *atype;
        void *p;
        xcc_stack_get_data(a_types, i, &p);
        atype = p;
        fprintf(fp, "    %s %s;\n", atype->ctype, atype->name);
    }
    fprintf(fp, "} XCCAType;\n\n");
    
    return XCC_RETURN_SUCCESS;
}

int output_etype_union(const XCCStack *e_types, FILE *fp)
{
    int i, n_etypes;
    
    n_etypes = xcc_stack_depth(e_types);
    fprintf(fp, "typedef union {\n");
    for (i = 0; i < n_etypes; i++) {
        EType *etype;
        void *p;
        xcc_stack_get_data(e_types, i, &p);
        etype = p;
        fprintf(fp, "    %s %s;\n", etype->ctype, etype->name);
    }
    fprintf(fp, "    void * unicast;\n");
    fprintf(fp, "} XCCEType;\n\n");
    
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

int output_element_tab(const XCCStack *elements, FILE *fp)
{
    int i, n_elements;
    
    n_elements = xcc_stack_depth(elements);
    fprintf(fp, "static XCCElementEntry XCCElementTab[] = {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        char *pname;
        void *p;
        xcc_stack_get_data(elements, i, &p);
        e = p;
        e->id = i + 1;
        pname = print_sharp_name(e->name);
        fprintf(fp, "    {%d, %s}%s\n", e->id, pname, i == n_elements - 1 ? "":",");
        xcc_free(pname);
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "static int get_element_id_by_name(const char *name)\n");
    fprintf(fp, "{\n");
    fprintf(fp, "    int i;\n");
    fprintf(fp, "    for (i = 0; i < %d; i++) {\n", n_elements);
    fprintf(fp, "        if (!strcmp(XCCElementTab[i].name, name)) {\n");
    fprintf(fp, "            return XCCElementTab[i].key;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    return -1;\n");
    fprintf(fp, "}\n\n");
    
    return XCC_RETURN_SUCCESS;
}

int output_start_handler(const XCCStack *elements, const char *ns_uri, FILE *fp)
{
    int i, n_elements;
    char *pns_uri, *buf1, *buf2;

    n_elements = xcc_stack_depth(elements);

    fprintf(fp, "void xcc_start_handler(void *data, const char *el, const char **attr)\n");  
    fprintf(fp, "{\n");
    fprintf(fp, "    XCCParserData *pdata = (XCCParserData *) data;\n");
    fprintf(fp, "    XCCNode *pnode = NULL, *node;\n");
    fprintf(fp, "    XCCEType element;\n");
    fprintf(fp, "    XCCAType attribute;\n");
    fprintf(fp, "    int i, element_id, parent_id, parent_child, skip = 0;\n");
    fprintf(fp, "    const char *avalue;\n");
    fprintf(fp, "    char *aname, *el_local;\n");
    fprintf(fp, "\n");
    fprintf(fp, "    if (pdata->error) {\n");
    fprintf(fp, "        return;\n");
    fprintf(fp, "    }\n\n");
    fprintf(fp, "    pdata->cbuflen = 0;\n");
    fprintf(fp, "    if (pdata->cbufsize) {\n");
    fprintf(fp, "        pdata->cbuffer[0] = '\\0';\n");
    fprintf(fp, "    }\n");

    pns_uri = print_sharp_name(ns_uri);
    fprintf(fp, "    el_local  = xcc_get_local(el, %s, &skip);\n", pns_uri);
    fprintf(fp, "    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    fprintf(fp, "        parent_id = 0;\n");
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        void *p;\n");
    fprintf(fp, "        xcc_stack_get_last(pdata->nodes, &p);\n");
    fprintf(fp, "        pnode = p;\n");
    fprintf(fp, "        parent_id = pnode->id;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    if (parent_id < 0) {\n");
    fprintf(fp, "        skip = 1;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    if (skip) {\n");
    fprintf(fp, "        element_id = -1;\n");
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        element_id = get_element_id_by_name(el_local);\n");
    fprintf(fp, "    }\n");

    fprintf(fp, "    if (parent_id >= 0 && element_id >= 0) {\n");
    fprintf(fp, "        parent_child = %d*parent_id + element_id;\n", n_elements);
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        parent_child = -1;\n");
    fprintf(fp, "    }\n\n");
    fprintf(fp, "    switch (parent_child) {\n");
    fprintf(fp, "    case 1:\n");
    
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
            fprintf(fp, "    case %d:\n", n_elements*parent_id + element_id);
        }
    }
    
    fprintf(fp, "        break;\n");
    fprintf(fp, "    default:\n");
    fprintf(fp, "        if (!skip) {\n");
    fprintf(fp, "            xcc_error(\"unexpected \\\"%%s\\\"->\\\"%%s\\\" combination\", pnode ? pnode->name:\"xml\", el_local);\n");
    fprintf(fp, "            pdata->error = 1;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        break;\n");
    fprintf(fp, "    }\n\n");

    fprintf(fp, "    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_attributes, element_id;
        char ebuf[128], abuf[128];
        
        xcc_stack_get_data(elements, i, &p);
        e = p;
        element_id  = e->id;

        sprintf(ebuf, "element.%s", e->etype->name);
        
        fprintf(fp, "    case %d: /* %s */\n", element_id, e->name);
        buf1 = replace(e->etype->ccode, "$$", ebuf);
        buf2 = replace(buf1, "$U", "pdata->udata");
        xcc_free(buf1);
        buf1 = replace(buf2, "$P", "pnode->data");
        xcc_free(buf2);
        fprintf(fp, "            %s\n", buf1);
        xcc_free(buf1);
        n_attributes = xcc_stack_depth(e->attributes);
        fprintf(fp, "            for (i = 0; attr[i]; i += 2) {\n");
        fprintf(fp, "                int askip = 0;\n");
        fprintf(fp, "                aname  = xcc_get_local(attr[i], %s, &askip);\n",
            pns_uri);
        fprintf(fp, "                avalue = attr[i + 1];\n");
        for (j = 0; j < n_attributes; j++) {
            Attribute *a;
            char *pname;

            xcc_stack_get_data(e->attributes, j, &p);
            a = p;

            pname = print_sharp_name(a->name);
            fprintf(fp, "                if (!strcmp(aname, %s)) {\n", pname);
            xcc_free(pname);
            sprintf(abuf, "attribute.%s", a->atype->name);
            buf1 = replace(a->atype->ccode, "$$", abuf);
            buf2 = replace(buf1, "$?", "avalue");
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            fprintf(fp, "                    %s\n", buf2);
            xcc_free(buf2);
            buf1 = replace(a->ccode, "$$", ebuf);
            buf2 = replace(buf1, "$?", abuf);
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            fprintf(fp, "                {\n");
            fprintf(fp, "                        %s\n", buf2);
            fprintf(fp, "                }\n");
            xcc_free(buf2);
            fprintf(fp, "                } else\n");
        }
        fprintf(fp, "                if (!askip) {\n");
        fprintf(fp, "                    xcc_error(\"unknown attribute \\\"%%s\\\" of element \\\"%%s\\\"\", aname, el_local);\n");
        fprintf(fp, "                }\n");
        fprintf(fp, "                xcc_free(aname);\n");
        fprintf(fp, "            }\n");
        fprintf(fp, "        break;\n");
    }

    xcc_free(pns_uri);

    fprintf(fp, "    default:\n");
    fprintf(fp, "        element.unicast = NULL;\n");
    fprintf(fp, "        if (!skip) {\n");
    fprintf(fp, "            xcc_error(\"unknown element \\\"%%s\\\"\", el_local);\n");
    fprintf(fp, "            pdata->error = 1;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        break;\n");
    fprintf(fp, "    }\n\n");

    
    fprintf(fp, "    node = xcc_node_new();\n");
    fprintf(fp, "    node->name = el_local;\n");
    fprintf(fp, "    node->id = element_id;\n");
    fprintf(fp, "    node->data = element.unicast;\n");
    
    fprintf(fp, "    xcc_stack_increment(pdata->nodes, node);\n");
    
    fprintf(fp, "}\n\n");
    
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


int output_end_handler(const XCCStack *elements, FILE *fp)
{
    int i, n_elements;
    char *buf1, *buf2;
    char pbuf[128], ebuf[128];

    n_elements = xcc_stack_depth(elements);

    fprintf(fp, "void xcc_end_handler(void *data, const char *el)\n");  
    fprintf(fp, "{\n");
    fprintf(fp, "    XCCParserData *pdata = (XCCParserData *) data;\n");
    fprintf(fp, "    XCCNode *node, *pnode;\n");
    fprintf(fp, "    void *p;\n");
    fprintf(fp, "    int element_id, parent_id, parent_child, skip = 0;\n");

    fprintf(fp, "    XCCEType element, pelement;\n");
    fprintf(fp, "    char *cdata = pdata->cbuffer;\n\n");
    fprintf(fp, "    if (pdata->error) {\n");
    fprintf(fp, "        return;\n");
    fprintf(fp, "    }\n\n");
    fprintf(fp, "    xcc_stack_get_last(pdata->nodes, &p);\n");
    fprintf(fp, "    node = p;\n");
    fprintf(fp, "    element_id = node->id;\n");
    fprintf(fp, "    element.unicast = node->data;\n");

    fprintf(fp, "    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int element_id;

        xcc_stack_get_data(elements, i, &p);
        e = p;
        if (e->data->s != NULL) {
            sprintf(ebuf, "element.%s", e->etype->name);
            element_id  = e->id;
            fprintf(fp, "    case %d:\n", element_id);
            fprintf(fp, "        {\n");
            buf1 = replace(e->data->s, "$$", ebuf);
            buf2 = replace(buf1, "$?", "cdata");
            fprintf(fp, "            %s\n", buf2);
            xcc_free(buf1);
            xcc_free(buf2);
            fprintf(fp, "        }\n");
            fprintf(fp, "        break;\n");
        }
    }
        
    fprintf(fp, "    }\n\n");

    fprintf(fp, "    xcc_stack_decrement(pdata->nodes);\n");

    fprintf(fp, "    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    fprintf(fp, "        pdata->root = element.unicast;\n");
    fprintf(fp, "        parent_id  = 0;\n");
    fprintf(fp, "        pelement.unicast = NULL;\n");
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        xcc_stack_get_last(pdata->nodes, &p);\n");
    fprintf(fp, "        pnode = p;\n");
    fprintf(fp, "        parent_id  = pnode->id;\n");
    fprintf(fp, "        pelement.unicast = pnode->data;\n");
    fprintf(fp, "    }\n");

    fprintf(fp, "    if (parent_id >= 0 && element_id >= 0) {\n");
    fprintf(fp, "        parent_child = %d*parent_id + element_id;\n", n_elements);
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        parent_child = -1;\n");
    fprintf(fp, "        skip = 1;\n");
    fprintf(fp, "    }\n\n");
    fprintf(fp, "    switch (parent_child) {\n");
    fprintf(fp, "    case 1:\n");
    fprintf(fp, "        break;\n");
    
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
            if (!ce) {
                xcc_error("couldn't find definition for element %s", c->name);
                return XCC_RETURN_FAILURE;
            }
            sprintf(ebuf, "element.%s", ce->etype->name);
            fprintf(fp, "    case %d:\n", n_elements*parent_id + ce->id);
            fprintf(fp, "        {\n");
            buf1 = replace(c->ccode, "$$", pbuf);
            buf2 = replace(buf1, "$?", ebuf);
            xcc_free(buf1);
            buf1 = replace(buf2, "$U", "pdata->udata");
            xcc_free(buf2);
            buf2 = replace(buf1, "$0", "xcc_get_root(pdata)");
            xcc_free(buf1);
            fprintf(fp, "        %s\n", buf2);
            xcc_free(buf2);
            fprintf(fp, "        }\n");
            fprintf(fp, "        break;\n");
        }
    }

    fprintf(fp, "    default:\n");
    fprintf(fp, "        if (!skip) {\n");
    fprintf(fp, "            xcc_error(\"internal error\");\n");
    fprintf(fp, "            pdata->error = 1;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        break;\n");
    fprintf(fp, "    }\n\n");

    fprintf(fp, "    pdata->cbuflen = 0;\n");
    fprintf(fp, "    if (pdata->cbufsize) {\n");
    fprintf(fp, "        pdata->cbuffer[0] = '\\0';\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "}\n");
    
    return XCC_RETURN_SUCCESS;
}

static void usage(const char *arg0, FILE *fp)
{
    fprintf(fp, "Usage: %s [options]\n", arg0);
    fprintf(fp, "Available options:\n");
    fprintf(fp, "  -i <file>  input file [stdin]\n");
    fprintf(fp, "  -o <file>  output file [stdout]\n");
    fprintf(fp, "  -b         bundle auxiliary stuff into parser\n");
    fprintf(fp, "  -V         display version info and exit\n");
    fprintf(fp, "  -h         print this help and exit\n");
}

int xcc_parse_opts(XCCOpts *xopts, int argc, char * const argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "i:o:bVh")) != -1) {
        switch (opt) {
        case 'i':
            xopts->ifile = optarg;
            break;
        case 'o':
            xopts->ofile = optarg;
            break;
        case 'b':
            xopts->bundle = 1;
            break;
        case 'V':
            printf("%s\n", xcc_get_version_string());
            exit(0);
            break;
        case 'h':
            usage(argv[0], stdout);
            exit(0);
            break;
        default:
            usage(argv[0], stderr);
            return XCC_RETURN_FAILURE;
            break;
        }
    }

    if (!xopts->ifile) {
        xopts->ifp = stdin;
    } else {
        xopts->ifp = fopen(xopts->ifile, "r");
    }
    if (!xopts->ifp) {
        fprintf(stderr, "Can't open input stream\n");
        return XCC_RETURN_FAILURE;
    }
    
    if (!xopts->ofile) {
        xopts->ofp = stdout;
    } else {
        xopts->ofp = fopen(xopts->ofile, "wb");
    }
    if (!xopts->ofp) {
        fprintf(stderr, "Can't open output stream\n");
        return XCC_RETURN_FAILURE;
    }
    
    return XCC_RETURN_SUCCESS;
}

int output_bundle(FILE *fp)
{
    int i = 0;
    char *s;
    while ((s = bundle_str[i])) {
        fprintf(fp, "%s\n", s);
        i++;
    }
    
    return XCC_RETURN_SUCCESS;
}
