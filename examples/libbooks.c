#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "books.h"

Book *book_new(void)
{
    Book *book;
    book = malloc(sizeof(Book));
    if (book) {
        memset(book, 0, sizeof(Book));
    }
    return book;
}

void book_free(Book *book)
{
    if (book) {
        if (book->title) {
            free(book->title);
        }
        if (book->publisher) {
            free(book->publisher);
        }
        if (book->nauthors) {
            unsigned  i;
            for (i = 0; i < book->nauthors; i++) {
                free(book->authors[i]);
            }
            free(book->authors);
        }
        
        free(book);
    }
}

void book_add_author(Book *book, char **author)
{
    book->authors = realloc(book->authors, (book->nauthors + 1)*sizeof(char *));
    book->authors[book->nauthors] = *author;
    book->nauthors++;
}

void book_info(const Book *book)
{
    unsigned  i;
    printf("Title: %s\n", book->title);
    printf(" Publisher: %s\n", book->publisher);
    if (book->nauthors) {
        puts(" Author(s):");
        for (i = 0; i < book->nauthors; i++) {
            printf("  [%d] %s\n", i + 1, book->authors[i]);
        }
    }
    if (book->price) {
        printf(" Price: %.2f\n", book->price);
    }
    putchar('\n');
}

Books *books_new(void)
{
    Books *books;
    books = malloc(sizeof(Books));
    if (books) {
        memset(books, 0, sizeof(Books));
    }
    return books;
}

void books_free(Books *books)
{
    if (books) {
        if (books->version) {
            free(books->version);
        }
        if (books->nbooks) {
            unsigned i;
            for (i = 0; i < books->nbooks; i++) {
                book_free(books->books[i]);
            }
            free(books->books);
        }
        free(books);
    }
}

void books_add_book(Books *books, Book *book)
{
    books->books = realloc(books->books, (books->nbooks + 1)*sizeof(Book *));
    books->nbooks++;
    books->books[books->nbooks - 1] = book;
}
