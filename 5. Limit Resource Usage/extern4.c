
const int N = 50;
int ara[50];
int main(){

    for(int i=0;i<N;i++) ara[i] = i+5;
    for(int j=0;j<10;j++){
        for(int i=0;i<N;i++) if( ara[i] == i-1 ) return -1; //to have effect
    }
   
    return 0;

}