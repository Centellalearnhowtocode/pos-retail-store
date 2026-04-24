#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        return 0;
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
            scanf("%s", name[0]);
            printf("Enter your number of men you want");
            scanf("%d", &n);
            
            for(int i=0; i<n; i++){
                if(strcmp(name[0], "cake") == 0){
                    printf("<<========================================>>\n");
                    printf("||               Menu of cake             ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Chocolate Cake 1$                    ||\n");
                    printf("|| . Cookie 0.5$                          ||\n");
                    printf("|| . Crosong 0.75$                        ||\n");
                    printf("|| . Add Tea                              ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", cake[i]);
                }else if(strcmp(name[0], "drink") == 0){
                    printf("<<========================================>>\n");
                    printf("||               Menu of drink            ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Americano 1$                         ||\n");
                    printf("|| . IceLatte 1.5$                        ||\n");
                    printf("|| . HotLatte 2$                          ||\n");
                    printf("|| . MachaLatte 2.5$                      ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", drink[i]);
                }else if(strcmp(name[0], "snack") == 0){
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
                    scanf("%s", snack[i]);   
                }else if(strcmp(name[0], "ice cream") == 0){
                    printf("<<========================================>>\n");
                    printf("||               Menu of ice cream        ||\n");
                    printf("<<========================================>>\n");
                    printf("|| . Chocolate 1$                         ||\n");
                    printf("|| . Vanilla 0.75$                        ||\n");
                    printf("|| . Strawberry 0.5$                      ||\n");
                    printf("|| . Matcha 2$                            ||\n");
                    printf("<<========================================>>\n");
                    printf("Enter your menu you want to add :");
                    scanf("%s", icecream[i]);
                }
            }
            break;
        }
    
        case 2:{

            printf("<<==============================================>>\n");
        
            break;
    }
  





    }while (op !=4);
    
        
    
    

}
    }
}


    