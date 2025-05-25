#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

#include <stdio.h>

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *image = fopen(filename, "rb");

    // Vérifie si le fichier existe
    if (image == NULL) {
        printf("Erreur : le fichier n'existe pas ou ne peut pas être ouvert !\n");
        return NULL;
    }

    // Lit l'en-tête BMP (54 octets)
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, image);

    // Récupère les informations de l'image depuis l'en-tête
    unsigned int width = *(unsigned int*)&header[18];
    unsigned int height = *(unsigned int*)&header[22];
    unsigned short colorDepth = *(unsigned short*)&header[28];
    unsigned int dataSize = *(unsigned int*)&header[34];

    // Vérifie si l'image est bien en 8 bits
    if (colorDepth != 8) {
        printf("Erreur : l'image n'est pas en niveau de gris (8 bits) !\n");
        fclose(image);
        return NULL;
    }

    // Réservation mémoire pour l'image BMP
    t_bmp8 *bmpImage = (t_bmp8 *)malloc(sizeof(t_bmp8));

    // Copie des en-têtes et de la palette de couleurs
    memcpy(bmpImage->header, header, 54);
    fread(bmpImage->colorTable, sizeof(unsigned char), 1024, image);

    // Stocke les propriétés dans la structure
    bmpImage->width = width;
    bmpImage->height = height;
    bmpImage->colorDepth = colorDepth;
    bmpImage->dataSize = dataSize;

    // Alloue la mémoire pour les données pixel
    bmpImage->data = (unsigned char *)malloc(dataSize);

    // Se place à l’emplacement des données et les lit
    fseek(image, *(unsigned int*)&header[10], SEEK_SET);
    fread(bmpImage->data, sizeof(unsigned char), dataSize, image);

    // Ferme le fichier et retourne l’image chargée
    fclose(image);
    printf("Image chargée avec succès !\n\n");
    return bmpImage;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    // Ouvre le fichier en écriture binaire
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier pour l’écriture.\n");
        return;
    }

    // Écriture de l'en-tête BMP
    if (fwrite(img->header, sizeof(unsigned char), 54, file) != 54) {
        printf("Erreur lors de l'écriture de l'en-tête.\n");
        fclose(file);
        return;
    }

    // Écriture de la table de couleurs
    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("Erreur lors de l'écriture de la palette de couleurs.\n");
        fclose(file);
        return;
    }

    // Écriture des données de pixels
    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        printf("Erreur lors de l'écriture des données image.\n");
        fclose(file);
        return;
    }

    fclose(file);
}

// Libère la mémoire allouée pour l'image BMP
void bmp8_free(t_bmp8 *img) {
    free(img->data);
    free(img);
}

// Affiche les informations principales de l’image
void bmp8_printInfo(t_bmp8 *img) {
    printf("Informations sur l'image\n");
    printf("    Largeur       : %u px\n", img->width);
    printf("    Hauteur       : %u px\n", img->height);
    printf("    Profondeur    : %u bits\n", img->colorDepth);
    printf("    Taille brute  : %d octets\n\n", img->dataSize);
}

// Applique un effet négatif à l'image
void bmp8_negative(t_bmp8 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i + j * img->width] = 255 - img->data[i + j * img->width];
        }
    }
}

// Ajuste la luminosité de l'image
void bmp8_brightness(t_bmp8 *img, int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int index = i * img->width + j;
            int pixel = img->data[index] + value;

            if (pixel > 255) pixel = 255;
            else if (pixel < 0) pixel = 0;

            img->data[index] = pixel;
        }
    }
}

// Applique un seuillage binaire
void bmp8_threshold(t_bmp8 *img, int threshold) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int index = i * img->width + j;
            img->data[index] = (img->data[index] >= threshold) ? 255 : 0;
        }
    }
}

// Applique un filtre de convolution à l'image
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    // Balayage des pixels en évitant les bords
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            float value = 0.0;

            for (int i = -kernelSize / 2; i <= kernelSize / 2; i++) {
                for (int j = -kernelSize / 2; j <= kernelSize / 2; j++) {
                    int neighborX = x + j;
                    int neighborY = y + i;
                    int index = neighborY * img->width + neighborX;
                    value += img->data[index] * kernel[i + kernelSize / 2][j + kernelSize / 2];
                }
            }

            int currentIndex = y * img->width + x;
            if (value > 255) value = 255;
            else if (value < 0) value = 0;

            img->data[currentIndex] = (unsigned char)value;
        }
    }
}
