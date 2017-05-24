#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Semaforlar
sem_t berberler;                
sem_t musteriler;               
sem_t mutex;  

// Değişkenler
int koltukS = 0;
int musteriS = 0;
int sandalyeS = 0;
int bosSandalyeS = 0;
int simdikiMusteri = 0;
int sandalye = 0;
int *koltuk;

void Berber (void *kimlik) {
    int sayi = *(int*)kimlik + 1;   // Berber kimliği
    int mID, sonrakiMusteri;        

    printf("%d. Berber\t-->\tdükkana geldi.\n", sayi);

    while (1) {
        if (!mID)
            printf("%d. Berber\t-->\tuyudu.\n\n", sayi);

        sem_wait(&berberler);   // Erişimi kilitle
        sem_wait(&mutex);       

        // Bekleyenlerin arasından müşteri seç
        simdikiMusteri = (++simdikiMusteri) % sandalyeS;
        sonrakiMusteri = simdikiMusteri;
        mID = koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri] = pthread_self();

        sem_post(&mutex);       // Erişimleri kaldır
        sem_post(&musteriler);  

        printf("%d. Berber\t-->\t%d. müşterinin saçını kesmeye başladı.\n\n", sayi, mID);
        sleep(1);
        printf("%d. Berber\t-->\t%d. müşterinin saçını kesmeyi bitirdi.\n\n", sayi, mID);
        
        if (sandalyeS == bosSandalyeS) {    // Bekleyen müşteri yoksa uyu
            printf("%d. Berber\t-->\tuyudu.\n\n", sayi);
        }
    }
    
    pthread_exit(0);
}

void Musteri (void *kimlik) {
    int sayi = *(int*)kimlik + 1;
    int oturulanSandalye, bID;

    sem_wait(&mutex);   // Erişimi kilitle

    printf("%d. Müşteri\t-->\tdükkana geldi.\n", sayi);

    // Bekleme odasında boş sandalye varsa
    if (bosSandalyeS > 0) {
        bosSandalyeS--;

        printf("%d. Müşteri\t-->\tbekleme salonunda bekliyor.\n\n", sayi);

        // Bekleme salonundan bir sandalye seçip otur
        sandalye = (++sandalye) % sandalyeS;
        oturulanSandalye = sandalye;
        koltuk[oturulanSandalye] = sayi;

        sem_post(&mutex);           // Erişim kilidini kaldır
        sem_post(&berberler);       // Sıradaki uygun berberi uyandır

        sem_wait(&musteriler);      // Bekleyen müşteriler kuyruğuna katıl
        sem_wait(&mutex);           // Koltuğa erişimi kilitle 

        // Berber koltuğuna geç 
        bID = koltuk[oturulanSandalye];
        bosSandalyeS++;

        sem_post(&mutex);
    }
    else {      // Bekleme salonunda boş sandalye yoksa dükkandan çık
        sem_post(&mutex);
        printf("%d. Müşteri\t-->\tbekleme salonunda yer bulamadı. Dükkandan ayrılıyor.\n\n", sayi);
    }
    pthread_exit(0);
}

int main (int argc) {
    // Gerekli verileri kullanıcıdan al
    printf("Musteri sayisini girin: ");
    scanf("%d", &musteriS);

    printf("Bekleme salonundaki sandalye sayisini girin: ");
    scanf("%d", &sandalyeS);

    printf("Berber koltuğu sayisini girin: ");
    scanf("%d", &koltukS);

    bosSandalyeS = sandalyeS;                           // Boş sandalyeleri belirle
    koltuk = (int*) malloc(sizeof(int) * sandalyeS);    // Koltuk dizisini oluştur.

    int berberKimlikleri[koltukS];

    pthread_t berber[koltukS];      // Berber iplikleri
    pthread_t musteri[musteriS];    // Müşteri iplikleri

    // Semaforlar başlat
    sem_init(&berberler, 0, 0);
    sem_init(&musteriler, 0, 0);
    sem_init(&mutex, 0, 1);

    printf("\nBerber dükkanı açıldı.\n\n");

    // Berber ipliklerini oluştur
    for (int i = 0; i < koltukS; i++) {
        pthread_create(&berber[i], NULL, (void*)Berber, (void*)&i);
        sleep(1);   // İplik oluşması için 1 sn
    }

    // Müşteri ipliklerini oluştur
    for (int i = 0; i < musteriS; i++) {
        pthread_create(&musteri[i], NULL, (void*)Musteri, (void*)&i);
        srand((unsigned int)time(NULL));
        usleep(rand() % (250000 - 50000 + 1) + 50000); // 50000 - 250000 ms
    }

    // Tüm müşterilerin işlerinin bitmesini bekle
    for (int i = 0; i < musteriS; i++) {
        pthread_join(musteri[i], NULL);
    }

    sleep(2);

    // Semaforları yok et
    sem_destroy(&berberler);
    sem_destroy(&musteriler);
    sem_destroy(&mutex);

    printf("Müşterilere hizmet verildi. Dükkan kapatılacak...\n\n");
    
    return 0;
}