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

/*
 * Library routines for XCC executable itself
 */

#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xccP.h"
#include "bundle.i"
#include "xfile.h"

/* TODO: get rid of it */
#define KEY_SHIFT   1


XCC *xcc_xcc_new(void)
{
    XCC *xcc;
    
    xcc = xcc_malloc(sizeof(XCC));
    memset(xcc, 0, sizeof(XCC));
    
    xcc->a_types = xcc_stack_new((XCC_stack_data_free) atype_free);
    xcc->e_types = xcc_stack_new((XCC_stack_data_free) etype_free);
    xcc->elements = xcc_stack_new((XCC_stack_data_free) element_free);
    xcc->preamble = xcc_code_new();
    xcc->postamble = xcc_code_new();

    xcc->currentLine = 1;

    return xcc;
}

void xcc_xcc_free(XCC *xcc)
{
    if (xcc) {
        xcc_stack_free(xcc->a_types);
        xcc_stack_free(xcc->e_types);
        xcc_stack_free(xcc->elements);
        xcc_code_free(xcc->preamble);
        xcc_code_free(xcc->postamble);
        xcc_free(xcc->ns_uri);
        xcc_free(xcc->prefix);
        xcc_free(xcc);
    }
}

AType *atype_new(void)
{
    AType *atype;
    atype = xcc_malloc(sizeof(AType));
    memset(atype, 0, sizeof(AType));
    atype->code = xcc_code_new();
    return atype;
}

void atype_free(AType *atype)
{
    if (atype) {
        xcc_free(atype->name);
        xcc_free(atype->ctype);
        xcc_code_free(atype->code);
        xcc_free(atype);
    }
}

EType *etype_new(void)
{
    EType *etype;
    etype = xcc_malloc(sizeof(EType));
    memset(etype, 0, sizeof(EType));
    etype->code = xcc_code_new();
    return etype;
}

void etype_free(EType *etype)
{
    if (etype) {
        xcc_free(etype->name);
        xcc_free(etype->ctype);
        xcc_code_free(etype->code);
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
    e->code       = xcc_code_new();
    e->same_parents = 1;
    return e;
}

void element_free(Element *e)
{
    if (e) {
        xcc_free(e->name);
        xcc_stack_free(e->attributes);
        xcc_stack_free(e->children);
        xcc_code_free(e->code);
        xcc_free(e);
    }
}

Attribute *attribute_new(void)
{
    Attribute *a;
    a = xcc_malloc(sizeof(Attribute));
    memset(a, 0, sizeof(Attribute));
    a->code = xcc_code_new();
    return a;
}

void attribute_free(Attribute *a)
{
    if (a) {
        xcc_free(a->name);
        xcc_code_free(a->code);
        xcc_free(a);
    }
}

Child *child_new(void)
{
    Child *c;
    c = xcc_malloc(sizeof(Child));
    memset(c, 0, sizeof(Child));
    c->code = xcc_code_new();
    return c;
}

void child_free(Child *c)
{
    if (c) {
        xcc_free(c->name);
        xcc_code_free(c->code);
        xcc_free(c);
    }
}

XCCCode *xcc_code_new(void)
{
    XCCCode *code;
    code = xcc_malloc(sizeof(XCCCode));
    memset(code, 0, sizeof(XCCCode));
    return code;
}

void xcc_code_free(XCCCode *code)
{
    if (code) {
        xcc_free(code->string);
        xcc_free(code);
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

static void dump(XCC *xcc, const char *fmt, ...)
{
    char *buf;
    int len;
    int i, ret;
    FILE *fp = xcc->opts->ofp;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    buf = xcc_malloc(len + 1);
    va_start(ap, fmt);
    ret = vsnprintf(buf, len + 1, fmt, ap);
    va_end(ap);
    if (ret < 0 || ret >= len + 1) {
        xcc_error("dump() failed");
        exit(1);
    }
    fputs(buf, fp);
    for (i = 0; buf[i] != 0; i++) {
        if (buf[i] == '\n') {
            xcc->currentLine++;
        }
    }
    xcc_free(buf);
}

static int output_header(XCC *xcc)
{
    dump(xcc, "#include <xcc.h>\n");
    dump(xcc, "#include <errno.h>\n");
    return XCC_RETURN_SUCCESS;
}

static int output_bundle(XCC *xcc)
{
    int i = 0;
    char *s;
    while ((s = bundle_str[i])) {
        fputs(s, xcc->opts->ofp);
        fputc('\n', xcc->opts->ofp);
        xcc->currentLine++;
        i++;
    }
    
    return XCC_RETURN_SUCCESS;
}


static int dump_code_chunk(XCC *xcc, int line, const char *chunk)
{
    /* Safety check */
    if (!xcc) {
        return XCC_RETURN_FAILURE;
    }
    
    if (!chunk) {
        /* Nothing to output */
        return XCC_RETURN_SUCCESS;
    }
    
    if (!xcc->opts->nolines) {
        dump(xcc, "#line %d \"%s\"\n", line, xcc->opts->ifile);
    }
    dump(xcc, "%s\n", chunk);
    if (!xcc->opts->nolines) {
        dump(xcc, "#line %d \"%s\"\n", xcc->currentLine, xcc->opts->ofile);
    }

    return XCC_RETURN_SUCCESS;
}

static int dump_code(XCC *xcc, const XCCCode *code)
{
    /* Safety check */
    if (!code) {
        return XCC_RETURN_FAILURE;
    } else {
        return dump_code_chunk(xcc, code->line, code->string);
    }
}

static int output_preamble(XCC *xcc)
{
    return dump_code(xcc, xcc->preamble);
}

static int output_postamble(XCC *xcc)
{
    return dump_code(xcc, xcc->postamble);
}

static int output_atype_union(XCC *xcc)
{
    int i, n_atypes;
    
    n_atypes = xcc_stack_depth(xcc->a_types);
    if (n_atypes > 0) {
        dump(xcc, "typedef union {\n");
        for (i = 0; i < n_atypes; i++) {
            AType *atype;
            void *p;
            xcc_stack_get_data(xcc->a_types, i, &p);
            atype = p;
            dump(xcc, "    %s %s;\n", atype->ctype, atype->name);
        }
        dump(xcc, "} XCCAType;\n\n");
    } else {
        /* We don't any have attributes */
        dump(xcc, "typedef void *XCCAType;\n");
    }
    
    return XCC_RETURN_SUCCESS;
}

static int output_etype_union(XCC *xcc)
{
    int i, n_etypes;
    
    n_etypes = xcc_stack_depth(xcc->e_types);
    dump(xcc, "typedef union {\n");
    for (i = 0; i < n_etypes; i++) {
        EType *etype;
        void *p;
        xcc_stack_get_data(xcc->e_types, i, &p);
        etype = p;
        dump(xcc, "    %s %s;\n", etype->ctype, etype->name);
    }
    dump(xcc, "    void * unicast;\n");
    dump(xcc, "} XCCEType;\n\n");
    
    return XCC_RETURN_SUCCESS;
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

static char *replaceVA(const char *str, ...)
{
    va_list var;
    char *f, *r, *buf1 = (char *) str, *buf2 = NULL;
    
    va_start(var, str);
    while ((f = va_arg(var, char *)) != NULL) {
        r = va_arg(var, char *);
        buf2 = replace(buf1, f, r);
        if (buf1 != str) {
            xcc_free(buf1);
        }
        buf1 = buf2;
    }
    va_end(var);
    
    return buf2;
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

static int output_element_tab(XCC *xcc)
{
    int i, n_elements;
    
    n_elements = xcc_stack_depth(xcc->elements);
    dump(xcc, "static XCCElementEntry XCCElementTab[] = {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        char *pname;
        void *p;
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        e->id = i + KEY_SHIFT;
        pname = print_sharp_name(e->name);
        dump(xcc, "    {%d, %s}%s\n", e->id, pname, i == n_elements - 1 ? "":",");
        xcc_free(pname);
    }
    dump(xcc, "};\n\n");

    dump(xcc, "static int get_element_id_by_name(const char *name)\n");
    dump(xcc, "{\n");
    dump(xcc, "    int i;\n");
    dump(xcc, "    for (i = 0; i < %d; i++) {\n", n_elements);
    dump(xcc, "        if (!strcmp(XCCElementTab[i].name, name)) {\n");
    dump(xcc, "            return XCCElementTab[i].key;\n");
    dump(xcc, "        }\n");
    dump(xcc, "    }\n");
    dump(xcc, "    return -1;\n");
    dump(xcc, "}\n\n");

    dump(xcc, "static char *get_element_name_by_id(int id)\n");
    dump(xcc, "{\n");
    dump(xcc, "    int i;\n");
    dump(xcc, "    for (i = 0; i < %d; i++) {\n", n_elements);
    dump(xcc, "        if (XCCElementTab[i].key == id) {\n");
    dump(xcc, "            return XCCElementTab[i].name;\n");
    dump(xcc, "        }\n");
    dump(xcc, "    }\n");
    dump(xcc, "    return NULL;\n");
    dump(xcc, "}\n\n");

    
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

static int output_init_occurrence(XCC *xcc)
{
    int i, n_elements, n_children;
    void *p;
    Element *e;

    n_elements = xcc_stack_depth(xcc->elements);

    dump(xcc, "static XCCOccurrence *init_occurrence(int element_id)\n");
    dump(xcc, "{\n");

    dump(xcc, "    XCCOccurrence *occurrence;\n");

    dump(xcc, "    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;

        n_children = xcc_stack_depth(e->children);
        if (!n_children) {
            dump(xcc, "    case %d:\n", e->id);
        }
    }
    dump(xcc, "        return NULL;\n");
    dump(xcc, "        break;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    occurrence = xcc_malloc(%d*sizeof(XCCOccurrence));\n",
        n_elements);
    dump(xcc, "    if (occurrence) {\n");
    dump(xcc, "        memset(occurrence, 0, %d*sizeof(XCCOccurrence));\n",
        n_elements);
    dump(xcc, "        switch (element_id) {\n");

    for (i = 0; i < n_elements; i++) {
        int j;
        
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        n_children = xcc_stack_depth(e->children);

        if (n_children) {
            dump(xcc, "        case %d:\n", e->id);

            for (j = 0; j < n_children; j++) {
                Child *c;
                Element *ce;
                xcc_stack_get_data(e->children, j, &p);
                c = p;

                ce = get_element_by_name(xcc->elements, c->name);
                if (!ce) {
                    xcc_error("couldn't find definition for element %s", c->name);
                    return XCC_RETURN_FAILURE;
                }

                dump(xcc, "            occurrence[%d].allowed = 1;\n",
                    ce->id - KEY_SHIFT);
                if (c->minOccurs) {
                    dump(xcc, "            occurrence[%d].minOccurs = %d;\n",
                        ce->id - KEY_SHIFT, c->minOccurs);
                }
                if (c->maxOccurs) {
                    dump(xcc, "            occurrence[%d].maxOccurs = %d;\n",
                        ce->id - KEY_SHIFT, c->maxOccurs);
                }
            }

            dump(xcc, "            break;\n");
        }
    }
    
    dump(xcc, "        }\n");
    dump(xcc, "    }\n");


    dump(xcc, "    return occurrence;\n");
    dump(xcc, "}\n\n");
    
    return XCC_RETURN_SUCCESS;
}

static int output_start_handler(XCC *xcc)
{
    int i, n_elements, n_attributes, n_attributes_max = 0;
    char *pns_uri, *buf1;
    Element *e;

    n_elements = xcc_stack_depth(xcc->elements);

    /* find max number of attributes per element */
    for (i = 0; i < n_elements; i++) {
        void *p;
        
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        n_attributes = xcc_stack_depth(e->attributes);
        if (n_attributes > n_attributes_max) {
            n_attributes_max = n_attributes;
        }
    }
    
    dump(xcc, "static void %s_start_handler(void *data, const char *el, const char **attr)\n",
                xcc->prefix);  
    dump(xcc, "{\n");
    dump(xcc, "    XCCParserData *pdata = (XCCParserData *) data;\n");
    dump(xcc, "    XCCNode *pnode = NULL, *node;\n");
    dump(xcc, "    XCCEType element;\n");
    dump(xcc, "    int i, element_id = -1, parent_id = -1, skip = 0, askip;\n");
    dump(xcc, "    const char *avalue;\n");
    dump(xcc, "    char *aname, *el_local;\n");
    if (n_attributes_max > 0) {
        dump(xcc, "    XCCAType attribute;\n");
        dump(xcc, "    char *attribs_required[%d];\n", n_attributes_max);
        dump(xcc, "    int nattribs_required = 0;\n");
    }
    dump(xcc, "    if (pdata->error) {\n");
    dump(xcc, "        return;\n");
    dump(xcc, "    }\n\n");

    if (n_attributes_max > 0) {
        dump(xcc, "    memset(attribs_required, 0, %d*sizeof(char *));\n\n",
            n_attributes_max);
    }
    
    dump(xcc, "    pdata->cbuflen = 0;\n");
    dump(xcc, "    if (pdata->cbufsize) {\n");
    dump(xcc, "        pdata->cbuffer[0] = '\\0';\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    element.unicast = NULL;\n\n");

    pns_uri = print_sharp_name(xcc->ns_uri);
    dump(xcc, "    el_local  = xcc_get_local(el, %s, &skip);\n", pns_uri);
    dump(xcc, "    if (skip) {\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    dump(xcc, "        pnode = NULL;\n");
    dump(xcc, "        parent_id = 0;\n");
    dump(xcc, "    } else {\n");
    dump(xcc, "        void *p;\n");
    dump(xcc, "        xcc_stack_get_last(pdata->nodes, &p);\n");
    dump(xcc, "        pnode = p;\n");
    dump(xcc, "        parent_id = pnode->id;\n");
    dump(xcc, "    }\n");
    dump(xcc, "    if (parent_id < 0) {\n");
    dump(xcc, "        skip = 1;\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    element_id = get_element_id_by_name(el_local);\n");

    dump(xcc, "    if (element_id < 0) {\n");
    dump(xcc, "        if (pdata->exception_handler(XCC_EELEM, el_local, pnode ? pnode->name:NULL, pdata->udata)) {\n");
    dump(xcc, "            skip = 1;\n");
    dump(xcc, "        } else {\n");
    dump(xcc, "            pdata->error = 1;\n");
    dump(xcc, "        }\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");
    
    dump(xcc, "    if (!pnode) {\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");
    
    dump(xcc, "    if (pnode->occurrence) {\n");
    dump(xcc, "        pnode->occurrence[element_id - %d].occurred++;\n",
        KEY_SHIFT);
    dump(xcc, "    }\n\n");
    
    dump(xcc, "    if (!pnode->occurrence || !pnode->occurrence[element_id - %d].allowed) {\n",
        KEY_SHIFT);
    dump(xcc, "        if (pdata->exception_handler(XCC_ECNTX, el_local, pnode ? pnode->name:NULL, pdata->udata)) {\n");
    dump(xcc, "            skip = 1;\n");
    dump(xcc, "        } else {\n");
    dump(xcc, "            pdata->error = 1;\n");
    dump(xcc, "        }\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");
    
    dump(xcc, "    if (pnode->occurrence[element_id - %d].maxOccurs &&\n",
        KEY_SHIFT);
    dump(xcc, "        pnode->occurrence[element_id - %d].occurred > pnode->occurrence[element_id - %d].maxOccurs) {\n",
        KEY_SHIFT, KEY_SHIFT);
    dump(xcc, "        if (pdata->exception_handler(XCC_EEMAX, el_local, pnode->name, pdata->udata)) {\n");
    dump(xcc, "            skip = 1;\n");
    dump(xcc, "        } else {\n");
    dump(xcc, "            pdata->error = 1;\n");
    dump(xcc, "        }\n");
    dump(xcc, "        goto e_switch;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "e_switch:\n\n");

    dump(xcc, "    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        void *p;
        int j, element_id;
        char ebuf[XCC_CHARBUFFSIZE], abuf[XCC_CHARBUFFSIZE],
            pbuf[XCC_CHARBUFFSIZE];
        Attribute *a;
        int nattribs_required = 0;
        
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        element_id  = e->id;

        if (!e->etype) {
            xcc_error("couldn't find element type of %s", e->name);
            return XCC_RETURN_FAILURE;
        }

        if (snprintf(ebuf, XCC_CHARBUFFSIZE, "element.%s", e->etype->name)
            >= XCC_CHARBUFFSIZE) {
            xcc_error("snprintf() failed in func %s line %d",
                __FUNCTION__, __LINE__);
            exit(1);
        }
        
        dump(xcc, "    case %d: /* %s */\n", element_id, e->name);
        if (e->parent_etype) {
            if (snprintf(pbuf, XCC_CHARBUFFSIZE, "((%s) pnode->data)",
                e->parent_etype->ctype) >= XCC_CHARBUFFSIZE) {
                xcc_error("snprintf() failed in func %s line %d",
                    __FUNCTION__, __LINE__);
                exit(1);
            }
        } else {
            strcpy(pbuf, "pnode->data");
        }

        buf1 = replaceVA(e->etype->code->string,
            "$$", ebuf,
            "$U", "pdata->udata",
            "$P", pbuf,
            NULL);
        
        dump_code_chunk(xcc, e->etype->code->line, buf1);
        xcc_free(buf1);
        
        n_attributes = xcc_stack_depth(e->attributes);
        
        /* get required attributes and their number */
        for (j = 0; j < n_attributes; j++) {

            xcc_stack_get_data(e->attributes, j, &p);
            a = p;
            
            if (a->required && n_attributes_max > 0) {
                char *pname = print_sharp_name(a->name);

                dump(xcc, "        attribs_required[%d] = %s;\n",
                    nattribs_required++, pname);
                xcc_free(pname);                
            }
        }
        if (nattribs_required) {
            dump(xcc, "        nattribs_required = %d;\n\n",
                nattribs_required);
        }
        
        dump(xcc, "        for (i = 0; attr[i]; i += 2) {\n");
        dump(xcc, "            askip = 0;\n");
        dump(xcc, "            aname  = xcc_get_local(attr[i], %s, &askip);\n",
            pns_uri);
        dump(xcc, "            avalue = attr[i + 1];\n");
        nattribs_required = 0;
        for (j = 0; j < n_attributes; j++) {
            char *pname;

            xcc_stack_get_data(e->attributes, j, &p);
            a = p;

            pname = print_sharp_name(a->name);
            dump(xcc, "                if (!strcmp(aname, %s)) {\n", pname);
            xcc_free(pname);

            if (snprintf(abuf, XCC_CHARBUFFSIZE, "attribute.%s", a->atype->name)
                >= XCC_CHARBUFFSIZE) {
                xcc_error("snprintf() failed in func %s line %d",
                    __FUNCTION__, __LINE__);
                exit(1);
            }
            buf1 = replaceVA(a->atype->code->string,
                "$$", abuf,
                "$?", "avalue",
                "$U", "pdata->udata",
                "$0", "xcc_get_root(pdata)",
                NULL);
            dump_code_chunk(xcc, a->atype->code->line, buf1);
            xcc_free(buf1);

            buf1 = replaceVA(a->code->string,
                "$$", ebuf,
                "$?", abuf,
                "$U", "pdata->udata",
                "$0", "xcc_get_root(pdata)",
                NULL);

            dump(xcc, "                {\n");
            dump_code_chunk(xcc, a->code->line, buf1);
            xcc_free(buf1);
            dump(xcc, "                }\n");
            
            if (a->required && n_attributes_max > 0) {
                /* clear 'required' flag */
                dump(xcc, "                attribs_required[%d] = NULL;\n",
                    nattribs_required++);
            }
            
            dump(xcc, "                } else\n");
        }
        dump(xcc, "                {\n");
        dump(xcc, "                   if (!askip && pdata->exception_handler(XCC_EATTR, aname, el_local, pdata->udata)) {\n");
        dump(xcc, "                       askip = 1;\n");
        dump(xcc, "                   }\n");
        dump(xcc, "                   if (!askip) {\n");
        dump(xcc, "                       pdata->error = 1;\n");
        dump(xcc, "                   }\n");
        dump(xcc, "                }\n");
        dump(xcc, "                xcc_free(aname);\n");
        dump(xcc, "            }\n");
        dump(xcc, "        break;\n");
    }

    xcc_free(pns_uri);

    dump(xcc, "    }\n\n");

    dump(xcc, "    if (skip) {\n");
    dump(xcc, "        element_id = -1;\n");
    dump(xcc, "    }\n");
    if (n_attributes_max > 0) {
        dump(xcc, "    else {\n");
        dump(xcc, "        for (i = 0; i < nattribs_required; i++) {\n");
        dump(xcc, "            aname = attribs_required[i];\n");
        dump(xcc, "            if (aname) {\n");
        dump(xcc, "                askip = pdata->exception_handler(XCC_EAREQ, aname, el_local, pdata->udata);\n");
        dump(xcc, "                if (!askip) {\n");
        dump(xcc, "                    pdata->error = 1;\n");
        dump(xcc, "                }\n");
        dump(xcc, "            }\n");
        dump(xcc, "        }\n");
        dump(xcc, "    }\n\n");
    }
    dump(xcc, "    node = xcc_node_new();\n");
    dump(xcc, "    node->name = el_local;\n");
    dump(xcc, "    node->id = element_id;\n");
    dump(xcc, "    node->data = element.unicast;\n");
    dump(xcc, "    node->occurrence = init_occurrence(element_id);\n");
    
    dump(xcc, "    xcc_stack_increment(pdata->nodes, node);\n");
    
    dump(xcc, "}\n\n");
    
    return XCC_RETURN_SUCCESS;
}


static int output_end_handler(XCC *xcc)
{
    int i, n_elements;
    char *buf1;
    char pbuf[XCC_CHARBUFFSIZE], ebuf[XCC_CHARBUFFSIZE];

    n_elements = xcc_stack_depth(xcc->elements);

    dump(xcc, "static void %s_end_handler(void *data, const char *el)\n",
        xcc->prefix);  
    dump(xcc, "{\n");
    dump(xcc, "    XCCParserData *pdata = (XCCParserData *) data;\n");
    dump(xcc, "    XCCNode *node, *pnode;\n");
    dump(xcc, "    void *p;\n");
    dump(xcc, "    int element_id, parent_id, parent_child, skip = 0;\n");
    dump(xcc, "    XCCEType element, pelement;\n");
    dump(xcc, "    char *cdata = pdata->cbuffer;\n\n");

    dump(xcc, "    if (pdata->error) {\n");
    dump(xcc, "        return;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    xcc_stack_get_last(pdata->nodes, &p);\n");
    dump(xcc, "    node = p;\n");
    dump(xcc, "    element_id = node->id;\n");
    dump(xcc, "    element.unicast = node->data;\n");

    dump(xcc, "    switch (element_id) {\n");
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int element_id;

        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        if (e->code->string != NULL) {
            if (snprintf(ebuf, XCC_CHARBUFFSIZE, "element.%s", e->etype->name)
                >= XCC_CHARBUFFSIZE) {
                xcc_error("snprintf() failed in func %s line %d",
                    __FUNCTION__, __LINE__);
                exit(1);
            }

            element_id  = e->id;
            dump(xcc, "    case %d: /* %s */\n", element_id, e->name);
            dump(xcc, "        {\n");
            
            buf1 = replaceVA(e->code->string,
                "$$", ebuf,
                "$?", "cdata",
                NULL);
            dump_code_chunk(xcc, e->code->line, buf1);
            xcc_free(buf1);

            dump(xcc, "        }\n");
            dump(xcc, "        break;\n");
        }
    }
    dump(xcc, "    }\n\n");

    dump(xcc, "    if (node->occurrence) {\n");
    dump(xcc, "        unsigned int i;\n");
    dump(xcc, "        for (i = 0; i < %d; i++) {\n", n_elements);
    dump(xcc, "            if (node->occurrence[i].occurred < node->occurrence[i].minOccurs) {\n");
    dump(xcc, "                char *cname = get_element_name_by_id(i + %d);\n", KEY_SHIFT);
    dump(xcc, "                if (!pdata->exception_handler(XCC_EEMIN, cname, node->name, pdata->udata)) {\n");
    dump(xcc, "                    pdata->error = 1;\n");
    dump(xcc, "                }\n");
    dump(xcc, "            }\n");
    dump(xcc, "        }\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    xcc_stack_decrement(pdata->nodes);\n");

    dump(xcc, "    if (xcc_stack_depth(pdata->nodes) == 0) {\n");
    dump(xcc, "        pdata->root = element.unicast;\n");
    dump(xcc, "        parent_id  = 0;\n");
    dump(xcc, "        pelement.unicast = NULL;\n");
    dump(xcc, "    } else {\n");
    dump(xcc, "        xcc_stack_get_last(pdata->nodes, &p);\n");
    dump(xcc, "        pnode = p;\n");
    dump(xcc, "        parent_id  = pnode->id;\n");
    dump(xcc, "        pelement.unicast = pnode->data;\n");
    dump(xcc, "    }\n");

    dump(xcc, "    if (parent_id >= 0 && element_id >= 0) {\n");
    dump(xcc, "        parent_child = %d*parent_id + element_id;\n", n_elements);
    dump(xcc, "    } else {\n");
    dump(xcc, "        parent_child = -1;\n");
    dump(xcc, "        skip = 1;\n");
    dump(xcc, "    }\n\n");
    dump(xcc, "    switch (parent_child) {\n");
    dump(xcc, "    case 1:\n");
    dump(xcc, "        break;\n");
    
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_children, parent_id;
        
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        parent_id  = e->id;
        n_children = xcc_stack_depth(e->children);
        if (snprintf(pbuf, XCC_CHARBUFFSIZE, "pelement.%s", e->etype->name)
            >= XCC_CHARBUFFSIZE) {
            xcc_error("snprintf() failed in func %s line %d",
                __FUNCTION__, __LINE__);
            exit(1);
        }
        for (j = 0; j < n_children; j++) {
            Child *c;
            Element *ce;
            xcc_stack_get_data(e->children, j, &p);
            c = p;
            ce = get_element_by_name(xcc->elements, c->name);
            if (!ce) {
                xcc_error("couldn't find definition for element %s", c->name);
                return XCC_RETURN_FAILURE;
            }
            if (snprintf(ebuf, XCC_CHARBUFFSIZE, "element.%s", ce->etype->name)
                >= XCC_CHARBUFFSIZE) {
                xcc_error("snprintf() failed in func %s line %d",
                    __FUNCTION__, __LINE__);
                exit(1);
            }
            dump(xcc, "    case %d:\n", n_elements*parent_id + ce->id);
            dump(xcc, "        {\n");
            
            buf1 = replaceVA(c->code->string,
                "$$", pbuf,
                "$?", ebuf,
                "$U", "pdata->udata",
                "$0", "xcc_get_root(pdata)",
                NULL);

            dump_code_chunk(xcc, c->code->line, buf1);
            xcc_free(buf1);

            dump(xcc, "        }\n");
            dump(xcc, "        break;\n");
        }
    }

    dump(xcc, "    default:\n");
    dump(xcc, "        if (!skip) {\n");
    dump(xcc, "            pdata->exception_handler(XCC_EINTR, NULL, NULL, pdata->udata);\n");
    dump(xcc, "            pdata->error = 1;\n");
    dump(xcc, "        }\n");
    dump(xcc, "        break;\n");
    dump(xcc, "    }\n\n");

    dump(xcc, "    pdata->cbuflen = 0;\n");
    dump(xcc, "    if (pdata->cbufsize) {\n");
    dump(xcc, "        pdata->cbuffer[0] = '\\0';\n");
    dump(xcc, "    }\n");
    dump(xcc, "}\n\n");
    
    return XCC_RETURN_SUCCESS;
}

static int output_parser(XCC *xcc)
{
    dump(xcc, "int %s_parse(FILE *fp, void **root, void *udata, XCCExceptionHandler exception_handler)\n", xcc->prefix);
    dump(xcc, "{\n");

    dump(xcc, "    if (xcc_run(fp, root, udata, %s_start_handler, %s_end_handler, exception_handler)\n",
        xcc->prefix, xcc->prefix);
    dump(xcc, "        != XCC_RETURN_SUCCESS) {\n");
    dump(xcc, "        return XCC_RETURN_FAILURE;\n");
    dump(xcc, "    }\n");
    
    dump(xcc, "    return XCC_RETURN_SUCCESS;\n");
    dump(xcc, "}\n");

    return XCC_RETURN_SUCCESS;
}


int xcc_output_parser(XCC *xcc)
{
    dump(xcc, "/* Generated by %s */\n\n", xcc_get_version_string());

    if (xcc->opts->bundle) {
        output_bundle(xcc);
    } else {
        output_header(xcc);
    }
    
    output_preamble(xcc);
    
    output_atype_union(xcc);
    output_etype_union(xcc);

    /* TODO: sort elements ?? */

    output_element_tab(xcc);

    output_init_occurrence(xcc);

    output_start_handler(xcc);
    
    output_end_handler(xcc);

    output_parser(xcc);

    output_postamble(xcc);

    return XCC_RETURN_SUCCESS;
}

int xcc_output_schema(const XCC *xcc)
{
    FILE *fp = xcc->opts->ofp;
    XFile *xf = xfile_new(fp);
    Attributes *attrs = attributes_new();
    int i, n_elements;
    char buf[XCC_CHARBUFFSIZE];

    if (!xf || !attrs) {
        return XCC_RETURN_FAILURE;
    }
    
    attributes_reset(attrs);

    xfile_set_ns(xf, "xs", "http://www.w3.org/2001/XMLSchema", 1);

    xfile_begin(xf, 1, NULL, NULL, "schema", attrs);

    if (snprintf(buf, XCC_CHARBUFFSIZE, "Generated by %s",
        xcc_get_version_string()) >= XCC_CHARBUFFSIZE) {
        xcc_error("snprintf() failed in func %s line %d",
            __FUNCTION__, __LINE__);
        exit(1);
    }
    xfile_comment(xf, buf);

    n_elements = xcc_stack_depth(xcc->elements);
    for (i = 0; i < n_elements; i++) {
        Element *e;
        void *p;
        int j, n_children, n_attributes, parent_id;
        
        xcc_stack_get_data(xcc->elements, i, &p);
        e = p;
        parent_id  = e->id;
        
        n_children = xcc_stack_depth(e->children);
        n_attributes = xcc_stack_depth(e->attributes);

        attributes_reset(attrs);
        attributes_set_sval(attrs, "name", e->name);
        
        if (n_children || n_attributes) {
            xfile_begin_element(xf, "element", attrs);
            
            attributes_reset(attrs);
            if (e->code->string) {
                attributes_set_sval(attrs, "mixed", "true");
            }
            xfile_begin_element(xf, "complexType", attrs);
            
            if (n_children) {
                xfile_begin_element(xf, "sequence", NULL);
                for (j = 0; j < n_children; j++) {
                    Child *c;
                    xcc_stack_get_data(e->children, j, &p);
                    c = p;

                    attributes_reset(attrs);
                    attributes_set_sval(attrs, "ref", c->name);
                    attributes_set_ival(attrs, "minOccurs", c->minOccurs);
                    if (c->maxOccurs) {
                        attributes_set_ival(attrs, "maxOccurs", c->maxOccurs);
                    } else {
                        attributes_set_sval(attrs, "maxOccurs", "unbounded");
                    }
                    xfile_empty_element(xf, "element", attrs);
                }
                xfile_end_element(xf, "sequence");
            }
            
            for (j = 0; j < n_attributes; j++) {
                Attribute *a;

                xcc_stack_get_data(e->attributes, j, &p);
                a = p;

                attributes_reset(attrs);
                attributes_set_sval(attrs, "name", a->name);
                if (a->required) {
                    attributes_set_sval(attrs, "use", "required");
                }
                xfile_empty_element(xf, "attribute", attrs);
            }

            xfile_end_element(xf, "complexType");
            
            xfile_end_element(xf, "element");
        } else {
            xfile_empty_element(xf, "element", attrs);
        }

    }

    attributes_free(attrs);
    
    xfile_end(xf);
    xfile_free(xf);

    return XCC_RETURN_SUCCESS;
}

static void usage(const char *arg0, FILE *fp)
{
    fprintf(fp, "Usage: %s [options]\n", arg0);
    fprintf(fp, "Available options:\n");
    fprintf(fp, "  -i <file>  input file [stdin]\n");
    fprintf(fp, "  -o <file>  output file [stdout]\n");
    fprintf(fp, "  -b         bundle auxiliary stuff into parser\n");
    fprintf(fp, "  -l         don't generate `#line' directives\n");
    fprintf(fp, "  -s         output WXS schema\n");
    fprintf(fp, "  -V         display version info and exit\n");
    fprintf(fp, "  -h         print this help and exit\n");
}

int xcc_parse_opts(XCCOpts *xopts, int argc, char * const argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "i:o:blsVh")) != -1) {
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
        case 'l':
            xopts->nolines = 1;
            break;
        case 's':
            xopts->schema = 1;
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
        xopts->ifile = "stdin";
    } else {
        xopts->ifp = fopen(xopts->ifile, "r");
    }
    if (!xopts->ifp) {
        xcc_error("can't open input stream");
        return XCC_RETURN_FAILURE;
    }
    
    if (!xopts->ofile) {
        xopts->ofp = stdout;
        xopts->ofile = "stdout";
    } else {
        xopts->ofp = fopen(xopts->ofile, "wb");
    }
    if (!xopts->ofp) {
        xcc_error("can't open output stream");
        return XCC_RETURN_FAILURE;
    }
    
    return XCC_RETURN_SUCCESS;
}
