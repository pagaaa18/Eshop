#ifndef ESHOP_H
#define ESHOP_H

#define MAX_PRODUCTS 20  // Megistos arithmos proionton
#define MAX_CLIENTS 5  // Megistos arithmos pelaton

typedef struct {
    char description[50];  // Perigrafi proiontos
    float price;  // Timh proiontos
    int item_count;  // Plithos diathesimwn proionton
    int requests;  // Arithmos aitiseon
    int sold;  // Arithmos pwliseon
} Product;

typedef struct {
    int product_id;  // ID proiontos
    int quantity;  // Posotita
} Order;

void init_catalog(Product catalog[]);
void process_order(Product catalog[], Order order, int client_fd);
void print_report(Product catalog[], int total_orders, int successful_orders, int failed_orders, float total_revenue);

#endif // ESHOP_H