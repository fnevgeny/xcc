/*
 * XCC - XML Compiler-Compiler
 * 
 * Copyright (c) 2000-2009 Evgeny Stambulchik
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

#include <string.h>
#include <stdarg.h>

#include "xcc.h"

void xcc_get_version_numbers(int *major, int *minor, int *nano)
{
    *major = XCC_VERSION_MAJOR;
    *minor = XCC_VERSION_MINOR;
    *nano  = XCC_VERSION_NANO;
}

void xcc_error(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fputs("xcc: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
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
            xcc_error("memory exhausted");
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
        unsigned int new_size = xs->size + XCC_STACK_CHUNK_SIZE;
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


XCCNode *xcc_node_new(void)
{
    XCCNode *n;
    n = xcc_malloc(sizeof(XCCNode));
    if (n) {
        memset(n, 0, sizeof(XCCNode));
    }
    return n;
}

void xcc_node_free(XCCNode *n)
{
    if (n) {
        xcc_free(n->name);
        xcc_free(n->occurrence);
        xcc_free(n);
    }
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

/* NB: attr1 is NULL-terminated, attr2 is NOT */
char **xcc_augment_attributes(const char **attr,
    unsigned int n2, char **attr2)
{
    unsigned int nattr, i2;
    int nextra = 2*n2;
    
    if (!n2) {
        return (char **) attr;
    }

    for (nattr = 0; attr[nattr]; nattr += 2) {
        const char *aname = attr[nattr];
        for (i2 = 0; i2 < 2*n2; i2 += 2) {
            char *aname2 = attr2[i2];
            if (aname2 && !strcmp(aname, aname2)) {
                attr2[i2] = NULL;
                nextra -= 2;
            }
        } 
    }
    
    if (nextra <= 0) {
        /* actually, it CAN'T be negative... */
        return (char **) attr;
    } else {
        unsigned nnew = nattr + nextra;
        char **attr_new = xcc_malloc((nnew + 1)*sizeof(char *));
        if (attr_new) {
            unsigned int inew = nattr;
            memcpy(attr_new, attr, nattr*sizeof(char *));
            for (i2 = 0; i2 < 2*n2; i2 += 2) {
                if (attr2[i2]) {
                    attr_new[inew] = attr2[i2];
                    attr_new[inew + 1] = attr2[i2 + 1];
                    inew += 2;
                }
            }
            attr_new[nnew] = NULL;
        }
        
        return attr_new;
    }
}

void *xcc_get_root(const XCCParserData *pdata)
{
    void *p;
    if (xcc_stack_get_data(pdata->nodes, 0, &p) != XCC_RETURN_SUCCESS) {
        return NULL;
    } else {
        XCCNode *node = p;
        return node->data;
    }
}

int xcc_get_linenum(const XCCParserData *pdata)
{
    if (pdata && pdata->parser) {
        return XML_GetCurrentLineNumber(pdata->parser);
    } else {
        return -1;
    }
}

static void xcc_char_data_handler(void *data, const char *s, int len)
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


static int xcc_exception_handler(int ierrno,
    const char *entity, const char *context, void *udata)
{
    int handled = 0;
    
    switch (ierrno) {
    case XCC_ECNTX:
        xcc_error("unexpected \"%s\" in the context of \"%s\"", entity, context ? context:"xml");
        break;
    case XCC_EATTR:
        xcc_error("unknown attribute \"%s\" of element \"%s\"", entity, context);
        break;
    case XCC_EELEM:
        xcc_error("unknown element \"%s\" appeared in context of \"%s\"", entity, context);
        break;
    case XCC_EEMIN:
        xcc_error("underrun of occurrences of \"%s\" in the context of \"%s\"", entity, context);
        break;
    case XCC_EEMAX:
        xcc_error("overrun of occurrences of \"%s\" in the context of \"%s\"", entity, context);
        break;
    case XCC_EAREQ:
        xcc_error("required attribute \"%s\" of element \"%s\" is missing", entity, context);
        break;
    case XCC_EINTR:
        xcc_error("internal error");
        break;
    }
    
    return handled;
}

void xcc_abort(XCCParserData *pdata)
{
    pdata->error = 1;
}

int xcc_run(FILE *fp, void **root, void *udata,
              XML_StartElementHandler start_element_handler,
              XML_EndElementHandler end_element_handler,
              XCCExceptionHandler exception_handler)
{
    XML_Parser xp;
    XCCParserData pdata;
    char Buff[XCC_BUFFSIZE];
    
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
    pdata.parser   = xp;

    if (exception_handler) {
        pdata.exception_handler = exception_handler;
    } else {
        pdata.exception_handler = xcc_exception_handler;
    }
    
    XML_SetUserData(xp, (void *) &pdata);

    XML_SetElementHandler(xp, start_element_handler, end_element_handler);

    /* Set the char data handler */
    XML_SetCharacterDataHandler(xp, xcc_char_data_handler);

    while (!pdata.error) {
        int done;
        int len;

        len = fread(Buff, 1, XCC_BUFFSIZE, fp);
        if (ferror(fp)) {
            xcc_error("Read error");
            pdata.error = 1;
            break;
        }
        done = feof(fp);

        if (!XML_Parse(xp, Buff, len, done)) {
            xcc_error("parse error at line %d:\n\t%s",
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
