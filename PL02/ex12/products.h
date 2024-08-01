#ifndef PRODUCTS_H
#define PRODUCTS_H
    #include "product.h"
    void fill_product_information(Product *products_ptr);
    int find_product_index(Product *products_ptr, int barcode, int num_of_products);
#endif