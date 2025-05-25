#include "bmp24.h"
#include <stdlib.h>
#include <string.h>

// Réservation dynamique pour un tableau 2D de pixels (image)
t_pixel **allocate_pixel_table(int width, int height) {
    t_pixel **table = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (!table) {
        printf("Erreur d'allocation mémoire pour les lignes.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        table[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (!table[i]) {
            printf("Erreur d'allocation mémoire pour les colonnes.\n");
            while (--i >= 0)
                free(table[i]);
            free(table);
            return NULL;
        }
    }

    return table;
}

// Libération complète de la mémoire occupée par le tableau de pixels
void free_pixel_table(t_pixel **table, int height) {
    for (int i = 0; i < height; i++) {
        free(table[i]);
    }
    free(table);
}

// Création d'une image BMP24 initialisée
t_bmp24 *create_bmp24(int width, int height, int color_depth) {
    t_bmp24 *bmp = malloc(sizeof(t_bmp24));
    if (!bmp) {
        printf("Erreur : échec lors de l'allocation mémoire de l'image.\n");
        return NULL;
    }

    bmp->width = width;
    bmp->height = height;
    bmp->colorDepth = color_depth;
    bmp->data = allocate_pixel_table(width, height);

    if (!bmp->data) {
        printf("Erreur : échec lors de l'allocation des pixels.\n");
        free(bmp);
        return NULL;
    }

    return bmp;
}

// Suppression complète d'une image BMP24
void delete_bmp24(t_bmp24 *bmp) {
    free_pixel_table(bmp->data, bmp->height);
    free(bmp);
}

// Lecture d'un bloc de données depuis un fichier à une position donnée
void read_data(uint32_t offset, void *buffer, uint32_t size, size_t count, FILE *f) {
    fseek(f, offset, SEEK_SET);
    fread(buffer, size, count, f);
}

// Écriture d'un bloc de données dans un fichier à une position donnée
void write_data(uint32_t offset, void *buffer, uint32_t size, size_t count, FILE *f) {
    fseek(f, offset, SEEK_SET);
    fwrite(buffer, size, count, f);
}

// Extraction d'un pixel spécifique depuis le fichier BMP
void read_pixel(t_bmp24 *bmp, int x, int y, FILE *f) {
    long offset = bmp->header.offset + ((bmp->height - 1 - y) * bmp->width + x) * 3;
    fseek(f, offset, SEEK_SET);

    unsigned char pixel[3];
    fread(pixel, sizeof(unsigned char), 3, f);

    bmp->data[y][x].blue = pixel[0];
    bmp->data[y][x].green = pixel[1];
    bmp->data[y][x].red = pixel[2];
}

// Lecture de tous les pixels dans le fichier BMP
void read_pixels(t_bmp24 *bmp, FILE *f) {
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            read_pixel(bmp, x, y, f);
        }
    }
}

// Écriture d'un pixel unique dans le fichier BMP
void write_pixel(t_bmp24 *bmp, int x, int y, FILE *f) {
    long offset = bmp->header.offset + ((bmp->height - 1 - y) * bmp->width + x) * 3;
    fseek(f, offset, SEEK_SET);

    unsigned char buffer[3] = {
        bmp->data[y][x].blue,
        bmp->data[y][x].green,
        bmp->data[y][x].red
    };
    fwrite(buffer, sizeof(unsigned char), 3, f);
}

// Sauvegarde complète des pixels dans le fichier
void write_pixels(t_bmp24 *bmp, FILE *f) {
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            write_pixel(bmp, x, y, f);
        }
    }
}

// Chargement d'une image BMP24 depuis un fichier
t_bmp24 *load_bmp24(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Erreur : impossible d'ouvrir le fichier.\n");
        return NULL;
    }

    t_bmp_header header;
    t_bmp_info info;

    read_data(0, &header, sizeof(t_bmp_header), 1, f);
    read_data(sizeof(t_bmp_header), &info, sizeof(t_bmp_info), 1, f);

    if (info.bits != 24) {
        printf("Erreur : l'image doit être en 24 bits.\n");
        fclose(f);
        return NULL;
    }

    t_bmp24 *bmp = create_bmp24(info.width, info.height, info.bits);
    if (!bmp) {
        fclose(f);
        return NULL;
    }

    bmp->header = header;
    bmp->header_info = info;

    read_pixels(bmp, f);
    fclose(f);

    return bmp;
}

// Enregistrement d'une image BMP24 dans un fichier
void save_bmp24(t_bmp24 *bmp, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Erreur : ouverture du fichier échouée.\n");
        return;
    }

    write_data(0, &bmp->header, sizeof(t_bmp_header), 1, f);
    write_data(sizeof(t_bmp_header), &bmp->header_info, sizeof(t_bmp_info), 1, f);
    write_pixels(bmp, f);
    fclose(f);
}

// Inversion des couleurs : effet négatif
void apply_negative_filter(t_bmp24 *bmp) {
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            bmp->data[y][x].red   = 255 - bmp->data[y][x].red;
            bmp->data[y][x].green = 255 - bmp->data[y][x].green;
            bmp->data[y][x].blue  = 255 - bmp->data[y][x].blue;
        }
    }
}

// Conversion de l'image en niveaux de gris
void apply_grey_filter(t_bmp24 *bmp) {
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            unsigned char grey = (
                bmp->data[y][x].red +
                bmp->data[y][x].green +
                bmp->data[y][x].blue
            ) / 3;
            bmp->data[y][x].red = bmp->data[y][x].green = bmp->data[y][x].blue = grey;
        }
    }
}

// Ajuste la luminosité globale de l'image
void adjust_brightness(t_bmp24 *bmp, int brightness) {
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            int r = bmp->data[y][x].red + brightness;
            int g = bmp->data[y][x].green + brightness;
            int b = bmp->data[y][x].blue + brightness;

            bmp->data[y][x].red   = r > 255 ? 255 : (r < 0 ? 0 : r);
            bmp->data[y][x].green = g > 255 ? 255 : (g < 0 ? 0 : g);
            bmp->data[y][x].blue  = b > 255 ? 255 : (b < 0 ? 0 : b);
        }
    }
}

// Applique une matrice de convolution sur un pixel donné
t_pixel convolution_filter(t_bmp24 *bmp, int x, int y, float **kernel, int kernel_size) {
    int half = kernel_size / 2;
    float r_sum = 0, g_sum = 0, b_sum = 0;

    for (int i = -half; i <= half; i++) {
        for (int j = -half; j <= half; j++) {
            int px = x + j;
            int py = y + i;

            if (px < 0) px = 0;
            if (px >= bmp->width) px = bmp->width - 1;
            if (py < 0) py = 0;
            if (py >= bmp->height) py = bmp->height - 1;

            float coeff = kernel[i + half][j + half];
            r_sum += bmp->data[py][px].red * coeff;
            g_sum += bmp->data[py][px].green * coeff;
            b_sum += bmp->data[py][px].blue * coeff;
        }
    }

    t_pixel pixel;
    pixel.red   = (unsigned char)(r_sum > 255 ? 255 : (r_sum < 0 ? 0 : r_sum));
    pixel.green = (unsigned char)(g_sum > 255 ? 255 : (g_sum < 0 ? 0 : g_sum));
    pixel.blue  = (unsigned char)(b_sum > 255 ? 255 : (b_sum < 0 ? 0 : b_sum));
    return pixel;
}
