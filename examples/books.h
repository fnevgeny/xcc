typedef struct {
    char     *publisher;
    char     *title;
    unsigned nauthors;
    char     **authors;
    float    price;
} Book;

typedef struct {
    char     *version;
    unsigned nbooks;
    Book     **books;
} Books;

Book *book_new(void);
void book_free(Book *book);
void book_add_author(Book *book, char **author);
void book_info(const Book *book);
Books *books_new(void);
void books_free(Books *books);
void books_add_book(Books *books, Book *book);
