#include <string.h>

#include "product.h"

void fill_product_information(Product *products_ptr) {
    products_ptr->barcode = 11111;
    strcpy(products_ptr->name, "Maçã");
    products_ptr->price = 2.50;

    products_ptr++;

    products_ptr->barcode = 22222;
    strcpy(products_ptr->name, "Pêra");
    products_ptr->price = 1.99;

    products_ptr++;

    products_ptr->barcode = 33333;
    strcpy(products_ptr->name, "Melão");
    products_ptr->price = 3.50;

    products_ptr++;

    products_ptr->barcode = 44444;
    strcpy(products_ptr->name, "Melância");
    products_ptr->price = 0.98;

    products_ptr++;

    products_ptr->barcode = 55555;
    strcpy(products_ptr->name, "Amendoins");
    products_ptr->price = 1.29;

}

int find_product_index(Product *products_ptr, int barcode, int num_of_products) {
    int index = -1;
    int i = 0;

    while (index == -1 && i < num_of_products) {
        if (barcode == products_ptr->barcode)
            index = i;

        i++;
        products_ptr++;
    }

    return index;

}