#include <stdio.h>
#include <stdint.h>

// Offsets dans l'en-tête BMP
#define OFFSET_MAGIC_NUMBER 0x00      // Position du type de fichier
#define OFFSET_FILE_SIZE    0x02      // Taille totale du fichier
#define OFFSET_PIXEL_ARRAY  0x0A      // Début des données image
#define OFFSET_IMG_WIDTH    0x12      // Largeur de l'image
#define OFFSET_IMG_HEIGHT   0x16      // Hauteur de l'image
#define OFFSET_BIT_DEPTH    0x1C      // Profondeur en bits
#define OFFSET_RAW_SIZE     0x22      // Taille brute des pixels

// Signature BMP standard ('BM' en hexadécimal)
#define BMP_SIGNATURE 0x4D42

// Dimensions des en-têtes
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

// Profondeur de couleur par défaut (24 bits = 3 octets par pixel)
#define COLOR_DEPTH_24BPP 0x18

// Structure représentant l'en-tête principal du fichier BMP
typedef struct {
    uint16_t type;         // Type du fichier (doit être 'BM')
    uint32_t size;         // Taille du fichier
    uint16_t reserved1;    // Réservé
    uint16_t reserved2;    // Réservé
    uint32_t offset;       // Position de départ des données pixel
} t_bmp_header;

// Structure contenant les métadonnées de l'image BMP
typedef struct {
    uint32_t size;             // Taille de cette structure
    int32_t width;             // Largeur en pixels
    int32_t height;            // Hauteur en pixels
    uint16_t planes;           // Nombre de plans (doit être 1)
    uint16_t bits;             // Nombre de bits par pixel
    uint32_t compression;      // Type de compression (0 = aucune)
    uint32_t imagesize;        // Taille brute des données pixel
    int32_t xresolution;       // Résolution horizontale (px/m)
    int32_t yresolution;       // Résolution verticale (px/m)
    uint32_t ncolors;          // Nombre de couleurs dans la palette
    uint32_t importantcolors;  // Couleurs importantes
} t_bmp_info;

// Structure représentant un pixel RGB (format 24 bits)
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

// Structure regroupant tous les éléments d'une image BMP 24 bits
typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;  // Tableau dynamique des pixels
} t_bmp24;

// Fonctions de gestion mémoire et lecture/écriture d'image
t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);

// Fonctions pour lire les données d'un fichier BMP
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp24 * image, FILE * file);

// Fonctions pour écrire les données d'une image BMP dans un fichier
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp24 * image, FILE * file);

// Fonctions principales pour charger et sauvegarder une image BMP
t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);
