#include "ergasia2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Arxikopoiisi tou katalogou me ta proionta
void init_catalog(Product catalog[]) {
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        snprintf(catalog[i].description, sizeof(catalog[i].description), "Product %d", i + 1);  // Arxikopoihsh perigrafis
        catalog[i].price = (i + 1) * 10.0;  // Timh proiontos
        catalog[i].item_count = 2;  // Arxika diathesima 2 antikeimena
        catalog[i].requests = 0;  // Arxika aitiseis 0
        catalog[i].sold = 0;  // Arxikopoiisi pwlhseon 0
    }
}

// Diaxirisi parangelias
void process_order(Product catalog[], Order order, int client_fd) {
    char message[100];
    catalog[order.product_id - 1].requests++;  // Auksisi tou arithmou aitiseon gia to proion
    if (catalog[order.product_id - 1].item_count >= order.quantity) {  // Elegxos an yparxei diathesimos apotipos
        catalog[order.product_id - 1].item_count -= order.quantity;  // Apofhthh apotipos
        catalog[order.product_id - 1].sold += order.quantity;  // Prosthetoume stis pwlhseis
        float cost = catalog[order.product_id - 1].price * order.quantity;  // Subologismos kostous
        snprintf(message, sizeof(message), "Order Successful! Total cost: %.2f\n", cost);  // Epityxhs paraggelia
    } else {
        snprintf(message, sizeof(message), "Order Failed! Not enough stock.\n");  // Apotypia paraggelias
    }
    write(client_fd, message, strlen(message) + 1);  // Apostoli minimatos ston pelati
}

// Emfanish anaklisis katastasis tou katalogou
void print_report(Product catalog[], int total_orders, int successful_orders, int failed_orders, float total_revenue) {
    printf("\n--- Eshop Report ---\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        printf("Product: %s\n", catalog[i].description);  // Ektuposi perigrafis
        printf("  Requests: %d\n", catalog[i].requests);  // Ektuposi aitiseon
        printf("  Sold: %d\n", catalog[i].sold);  // Ektuposi pwlhseon
        printf("  Remaining: %d\n", catalog[i].item_count);  // Ektuposi upoloipon
    }
    printf("\nSummary:\n");
    printf("  Total Orders: %d\n", total_orders);  // Oloklhrwmena parangelies
    printf("  Successful Orders: %d\n", successful_orders);  // Epityxhs paraggelies
    printf("  Failed Orders: %d\n", failed_orders);  // Apotypies paraggelies
    printf("  Total Revenue: %.2f\n", total_revenue);  // Oloklhrwmena esoda
}

int main() {
    // Arxikopoihsh tou srand me xrono kai pid gia tin tuxaiotita
    srand(time(NULL) ^ getpid());  // Xrhsimopoioume to getpid() gia pio tuxaih arxikopoihsh

    Product catalog[MAX_PRODUCTS];  // Pinakas proionton
    init_catalog(catalog);  // Arxikopoihsh katalogou

    int total_orders = 0, successful_orders = 0, failed_orders = 0;  // Arxikopoihsh metritwn
    float revenue = 0;  // Arxikopoihsh esodwn
    
    //Dhmhourgia pinakwn pipes gia epikoinwnia gonea me paidi
    int parent_to_client[MAX_CLIENTS][2];  
    int client_to_parent[MAX_CLIENTS][2];  
    pid_t pids[MAX_CLIENTS];  // Pinakas gia ta pids twn pelatwn

    // Dimiourgia pipes me elegxo lathwn
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pipe(parent_to_client[i]) == -1 || pipe(client_to_parent[i]) == -1) {  // Elegxos an to pipe dhlwthike swsta
            perror("Pipe Error");
            exit(EXIT_FAILURE);
        }
    }

    // Dimiourgia pelatwn
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pids[i] = fork();  // Fork gia kathe pelati
        if (pids[i] < 0) {  // An yparxei sfalma sto fork
            perror("Fork Error");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Kwdikas gia ton pelati
            close(parent_to_client[i][1]);  // Kleisimo tou write pipe
            close(client_to_parent[i][0]);  // Kleisimo tou read pipe

            for (int j = 0; j < 10; j++) {  // Kanei 10 parangelies o pelatis
                Order order;
                order.product_id = rand() % MAX_PRODUCTS + 1;  // Tuxaios epilogos proiontos (1-20)
                order.quantity = 1;  // Posotita 1 gia kaqe paraggelia

                write(client_to_parent[i][1], &order, sizeof(Order));  /*Stenloume tin paraggelia ston gonea mesw pipe 
                kai grafoume ta dedomena*/

                char response[100];
                read(parent_to_client[i][0], response, sizeof(response));  // Diavazoume to apotelesma tis paraggelias
                printf("Client %d: %s", i + 1, response);  // Ektupwsi apotelesmatos paraggelias (clients 1-5)

                sleep(1);  // Perimenoume 1 defterolepto metaksi twn parangelion
            }
            close(parent_to_client[i][0]);  // Kleisimo tou read pipe
            close(client_to_parent[i][1]);  // Kleisimo tou write pipe
            exit(0);  // Termatismos pelati
        }
    }

    // Kwdikas tou gonea
    for (int i = 0; i < MAX_CLIENTS; i++) {
        close(parent_to_client[i][0]);  // Kleisimo read pipe
        close(client_to_parent[i][1]);  // Kleisimo write pipe
    }

    for (int i = 0; i < MAX_CLIENTS * 10; i++) {  // Diavazoume 10 paraggelies gia kaqe pelati
        Order order;
        read(client_to_parent[i % MAX_CLIENTS][0], &order, sizeof(Order));  // Diavazoume tin paraggelia
        total_orders++;  // Auksisi oloklirwmenwn parangelion

        if (catalog[order.product_id - 1].item_count >= order.quantity) {  // Elegxos diathesimotitas (1-20)
            successful_orders++;  // Epityxhs paraggelia
            revenue += catalog[order.product_id - 1].price * order.quantity;  // Prosthiki esodwn
        } else {
            failed_orders++;  // Apotypi paraggelias
        }

        process_order(catalog, order, parent_to_client[i % MAX_CLIENTS][1]);  // Diaxirisi parangelias
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        wait(NULL);  // Perimenoume olous tous pelates na termatisoun
    }

    print_report(catalog, total_orders, successful_orders, failed_orders, revenue);  // Ektupwsi anaklisis katastasis

    return 0;
}
