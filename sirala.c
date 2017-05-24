#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define N 10      // Rastgele oluşacak sayı miktari

// Sıralanacak dizinin başlangıç ve bitiş noktalarını belirlemeye yarayan struct
typedef struct {
    int bas;
    int son;
} veri;

int numbers[N];         // Rastgele oluşturulacak dizi
int siralanmis[N];      // Parça parça sıralandıktan sonra birleştireceğimiz dizi

// Sıralama Fonksiyonu
void sirala (veri *v) {
    // Insertion sıralama algoritması
    int i = 0, key, j = 0;
    for (i = (v->bas + 1); i < v->son; i++){
        key = numbers[i];
        j = i-1;
        while (j >= v->bas && numbers[j] > key){
            numbers[j+1] = numbers[j];
            j = j-1;
        }
        numbers[j+1] = key;
    }
}

// Parça parça sıralanan dizinin birleştirileceği fonksiyon
void birlestir () {
    int i = 0, j = N/2, k = 0;

    for (k = 0; k < N; ++k) {
        if (i == N/2)
            siralanmis[k] = numbers[j++];
        else if (j == N)
            siralanmis[k] = numbers[i++];
        else {
            if (numbers[i] < numbers[j])
                siralanmis[k] = numbers[i++];
            else
                siralanmis[k] = numbers[j++];
        }  
    }
}

int main() {
    
    unsigned char is_used[N] = { 0 };   // Random oluşturulan sayının daha önce kullanılığ kullanılmadığını kontrol etmek için kullanılan dizi
    pthread_t th[3];                    // Bizden istenen 3 tane iplik
    veri veri[2];                       // Yukarıda tanımlanan struct'tan oluşturulan dizi

    srand(time(NULL));

    // Floyd algoritması ile her sayının farklı olması sağlandı
    for (int i = 0; i < N; i++) {   
        int r = (rand() % N);                   // Rastgele sayı üretme
        if (is_used[r] == 0) {                  // Bu üretilen sayı daha önce kullanılmadıysa
            numbers[i] = r;                     // Üretilen sayıyı diziye at
            is_used[r] = 1;                     // Kullanıldı olarak işaretle
            printf("%d\n", numbers[i]);         
        }
        else
            i--;                                // Üretilen sayı kullanılmışsa döngü değişkeninin değerini azalt
    }
    printf("\n\n");

    //THREADS
    veri[0].bas = 0;
    veri[0].son = N / 2;
    pthread_create(&th[0], NULL, (void *)sirala, &veri[0]);     // Dizinin ilk yarısını sıralar

    veri[1].bas = N / 2;
    veri[1].son = N;
    pthread_create(&th[1], NULL, (void *)sirala, &veri[1]);     // Dizinin son yarısını sıralar

    // Dizilerin sıralanmasını bekle
    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);
    
    // Sıralanan dizileri gör
    printf("Sıralanmış Diziler...\n\n");
    for (int m = 0; m < N; m++) {
        if (m % (N / 2) == 0 && m != 0)
            printf("\n");
        printf("-->\t%d\n", numbers[m]);
    }
    printf("\n\n");

    pthread_create(&th[2], NULL, (void *)birlestir, NULL);      // Parçalı şekilde sıralanan dizileri birleştir
    pthread_join(th[2], NULL);

    // Dosyaya yazma işlemi
    FILE *pDosya; 
    pDosya = fopen("son.txt", "w");

    printf("Birleştirilmiş Dizi...\n\n");
    for (int m = 0; m < N; m++) {
        if (m % 5 == 0 && m != 0)
            fprintf(pDosya, "\n");
        fprintf(pDosya, "%d\t", siralanmis[m]);
        printf("-->\t%d\n", siralanmis[m]);
    }

    fclose(pDosya); 

    printf("\n\n");
    pthread_exit(NULL);
    
    return 0;
}

