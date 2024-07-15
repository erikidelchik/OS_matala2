#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkIfWon(int arr[3][3],int team){
    //check rows
    for(int i=0;i<3;i++){
        int c = 0;
        for(int j=0;j<3;j++){
            if(arr[i][j]==team)
                c++;
        }
        if(c==3) return 1;
    }

    //check cols
    for(int i=0;i<3;i++){
        int c = 0;
        for(int j=0;j<3;j++){
            if(arr[j][i]==team)
                c++;
        }
        if(c==3) return 1;
    }

    //check diagonal
    int c = 0;
    for(int i=0;i<3;i++){
        if(arr[i][i]==team)
            c++;
    }
    if(c==3) return 1;

    c=0;
    //check anti-diagonal
    for(int i=2;i>=0;i--){
        if(arr[i][i]==team){
            c++;
        }
    }
    if(c==3) return 1;

    return 0;
}


int main(int argc,char *argv[]){
    if(argc!=2){
        printf("Error\n");
        return 1;
    }
    int game[3][3] = {0};
    int numbers[9] = {0};
    char* str = argv[1];
    if(strlen(str)!=9){
        printf("Error\n");
        return 1;
    }
    int num = atoi(str);
    
    for(int i=0;i<9;i++){
        if(num%10==0 || numbers[i]) {
            printf("Error\n");
            return 1;
        }
        numbers[i] = num%10;
        num/=10;
    }
    int size = 8;
    int myTurn;
    while(1){
        //his turn
        while (1) {
            //if this block is already filled
            if (game[(numbers[size]-1)/3][(numbers[size]-1)%3])
                size--;
            else {
                game[(numbers[size]-1)/3][(numbers[size]-1)%3] = 2;
                printf("i played: %d\n",numbers[size]);
                fflush(stdout);

                //check if game over
                int draw = 1;
                for(int i=0;i<3;i++){
                    for(int j=0;j<3;j++){
                        if(!game[i][j]) {
                            draw = 0;
                            break;
                        }
                    }
                }
                if(draw){
                    printf("Draw\n");
                    fflush(stdout);
                    return 0;
                }
                if(checkIfWon(game,2)){
                    printf("i won\n");
                    fflush(stdout);
                    return 0;
                }
                break;
            }
        }
        //my turn
        while (1) {
            printf("enter your choice: \n");
            fflush(stdout);
            scanf("%d", &myTurn);
            if(myTurn<1 || myTurn>9){
                printf("illegal move\n");
                fflush(stdout);
            }
            else if(game[(myTurn-1)/3][(myTurn-1)%3]){
                printf("this block is already filled\n");
                fflush(stdout);
            }
            else{
                myTurn--;
                game[myTurn/3][myTurn%3] = 1;
                if(checkIfWon(game,1)) {
                    printf("i lost\n");
                    fflush(stdout);
                    return 0;
                }
                break;

            }
        }

    }








}
