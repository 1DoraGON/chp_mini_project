#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MAX_LINE_LENGTH 1000
#define MAX_PRODUCTS 1000
#define MAX_USERS 1000
#define MAX_SELLS 1000

typedef struct {
    int id;
    char name[100];
    float price;
    int stock;
} Product;

typedef struct {
    int id;
    char name[100];
} User;

typedef struct {
    int id;
    int user_id;
    int product_id;
    int product_count;
} Sell;

typedef struct {
    int id;
    int user_id;
    char products[100][100];
    float total_price;
} UserSellProduct;

Product products[MAX_PRODUCTS];
User users[MAX_USERS];
Sell sells[MAX_SELLS];
UserSellProduct userSellProducts[MAX_SELLS];

int numProducts = 0;
int numUsers = 0;
int numSells = 0;
int numUserSellProducts = 0;

void joinData() {
    int numTasks, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Broadcast the number of products, users, and sells to all processes
    MPI_Bcast(&numProducts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numUsers, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numSells, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter products, users, and sells data to different processes
    MPI_Scatter(products, numProducts / numTasks, MPI_BYTE, products, numProducts / numTasks, MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Scatter(users, numUsers / numTasks, MPI_BYTE, users, numUsers / numTasks, MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Scatter(sells, numSells / numTasks, MPI_BYTE, sells, numSells / numTasks, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Perform local join on each process
    int start = rank * (numSells / numTasks);
    int end = start + (numSells / numTasks);
    for (int i = start; i < end; i++) {
        int user_id = sells[i].user_id;
        int product_id = sells[i].product_id;

        // Find the user and product information
        char user_name[100];
        char product_name[100];
        for (int j = 0; j < numUsers; j++) {
            if (users[j].id == user_id) {
                strcpy(user_name, users[j].name);
                break;
            }
        }
        for (int j = 0; j < numProducts; j++) {
            if (products[j].id == product_id) {
                strcpy(product_name, products[j].name);
                break;
            }
        }

        // Calculate the total price
        float total_price = sells[i].product_count * products[product_id].price;

        // Store the joined information in the userSellProducts array
        userSellProducts[i].id = sells[i].id;
        userSellProducts[i].user_id = user_id;
        strcpy(userSellProducts[i].products[0], product_name);
        userSellProducts[i].total_price = total_price;
    }

    // Gather the joined data from all processes
    MPI_Gather(userSellProducts, numSells / numTasks, MPI_BYTE, userSellProducts, numSells / numTasks, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Print the joined data on the root process (rank 0)
    if (rank == 0) {
        FILE* file = fopen("user_sell_product.txt", "w");
        if (file == NULL) {
            printf("Error opening 'user_sell_product.txt' file for writing.\n");
            exit(1);
        }

        for (int i = 0; i < numSells; i++) {
            fprintf(file, "id:%d,user_id:%d,products:[\"%s\"],total_price:%.2f\n",
                    userSellProducts[i].id,
                    userSellProducts[i].user_id,
                    userSellProducts[i].products[0],
                    userSellProducts[i].total_price);
        }

        fclose(file);
    }
}

void readProductFile() {
    // Open "product.txt" file for reading
    FILE* file = fopen("product.txt", "r");
    if (file == NULL) {
        printf("Error opening 'product.txt' file.\n");
        exit(1);
    }

    // Read data from the file and populate the products array
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        sscanf(line, "id:%d,name:\"%[^\"]\",price:%f,stock:%d",
               &products[numProducts].id,
               products[numProducts].name,
               &products[numProducts].price,
               &products[numProducts].stock);
        numProducts++;
    }

    // Close the file
    fclose(file);
}

void readUserFile() {
    // Open "users.txt" file for reading
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening 'users.txt' file.\n");
        exit(1);
    }

    // Read data from the file and populate the users array
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        sscanf(line, "user_id:%d,name:\"%[^\"]\"",
               &users[numUsers].id,
               users[numUsers].name);
        numUsers++;
    }

    // Close the file
    fclose(file);
}

void readSellFile() {
    // Open "sells.txt" file for reading
    FILE* file = fopen("sells.txt", "r");
    if (file == NULL) {
        printf("Error opening 'sells.txt' file.\n");
        exit(1);
    }

        // Read data from the file and populate the sells array
        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
            sscanf(line, "sell_id:%d,user_id:%d,product_id:%d,product_count:%d",
                   &sells[numSells].id,
                   &sells[numSells].user_id,
                   &sells[numSells].product_id,
                   &sells[numSells].product_count);
            numSells++;
        }

        // Close the file
        fclose(file);
    }

    int main(int argc, char** argv) {
        // Initialize MPI
        MPI_Init(&argc, &argv);

        // Get the number of processes and the rank of the current process
        int numTasks, rank;
        MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        // Read the input files on all processes
        readProductFile();
        readUserFile();
        readSellFile();

        // Perform the join operation
        joinData(rank, numTasks);

        // Finalize MPI
        MPI_Finalize();

        return 0;
    }

