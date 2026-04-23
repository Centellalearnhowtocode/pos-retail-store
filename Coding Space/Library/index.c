#include <stdio.h>
#include <stdlib.h>
int main(){
    int op;
    int id[20];
    char name[20][20];
    float price[20];
    int code;
    int n;
    int choice;
    char cake[20][20];
    char drink[20][20];
    char snack[20][20];
    char icecream[20][20];

    printf("Enter your code :");
    scanf("%d",&code);
    if(code==1234){
        printf("successfully login\n");
    }else{
        printf("Wrong code\n");
        exit(0);
    }
    do{

    printf("<<==============================================>>\n");
    printf("||               System of mart                 ||\n");
    printf("<<==============================================>>\n");
    printf("|| 1. Show menu                                 ||\n");
    printf("|| 2. Out put menu                              ||\n");
    printf("|| 3. Remove item                               ||\n");
    printf("|| 4. Exit                                      ||\n");
    printf("<<==============================================>>\n");
    printf("Enter your choice : ");
    scanf("%d", &op);

    switch (op){
        case 1:{
            printf("<<==============================================>>\n");
            printf("||               Menu of mart                   ||\n");
            printf("<<==============================================>>\n");
            printf("|| . Cake                                       ||\n");
            printf("|| . Drink                                      ||\n");
            printf("|| . Snack                                      ||\n");
            printf("|| . Ice Cream                                  ||\n");
            printf("<<==============================================>>\n");
            
            printf("Enter your menu you want to add :");
            scanf("%s", name[n]);
                if(name=="cake"){
                    printf("<<========================================>>\n");
                    printf("||               Menu of cake             ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Chocolate Cake 1$                    ||\n");
                    printf("|| . Cookie 0.5$                          ||\n");
                    printf("|| . Crosong 0.75$                        ||\n");
                    printf("|| . Add Tea                              ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", cake[n]);
                }else if(name=="drink"){
                    printf("<<========================================>>\n");
                    printf("||               Menu of drink            ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Americano 1$                         ||\n");
                    printf("|| . IceLatte 1.5$                        ||\n");
                    printf("|| . HotLatte 2$                          ||\n");
                    printf("|| . MachaLatte 2.5$                      ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", drink[n]);
                }else if(name=="snack"){
                    printf("<<========================================>>\n");
                    printf("||               Menu of snack            ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Sandwich 1$                          ||\n");
                    printf("|| . Salad 0.75$                          ||\n");
                    printf("|| . French Fries 0.5$                    ||\n");
                    printf("|| . Pizza 3.5$                           ||\n"); 
                    printf("|| . Burger 2.5$                          ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", snack[n]);   
                }else if(name=="ice cream"){
                    printf("<<========================================>>\n");
                    printf("||               Menu of ice cream        ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Chocolate 1$                         ||\n");
                    printf("|| . Vanilla 0.75$                        ||\n");
                    printf("|| . Strawberry 0.5$                      ||\n");
                    printf("|| . Matcha 2$                            ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", icecream[n]);
            break;
        }
        case 2:{
            
        }

    }
  





    }while (op !=5);
    {
        /* code */
    }
    

}