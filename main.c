#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

void afficherMenuPrincipal() {
    printf("\nVeuillez choisir une option :\n");
    printf("1. Ouvrir une image\n");
    printf("2. Sauvegarder une image\n");
    printf("3. Appliquer un filtre\n");
    printf("4. Afficher les informations de l'image\n");
    printf("5. Quitter\n");
    printf(">>> Votre choix : ");
}

void afficherMenuFiltres() {
    printf("\nVeuillez choisir un filtre :\n");
    printf("1. Négatif\n");
    printf("2. Luminosité\n");
    printf("3. Binarisation\n");
    printf("4. Flou\n");
    printf("5. Flou gaussien\n");
    printf("6. Netteté\n");
    printf("7. Contours\n");
    printf("8. Relief\n");
    printf("9. Retourner au menu précédent\n");
    printf(">>> Votre choix : ");
}

float** creerMatrice(float valeurs[], int taille) {
    float** matrice = malloc(taille * sizeof(float*));
    for (int i = 0; i < taille; i++) {
        matrice[i] = malloc(taille * sizeof(float));
        for (int j = 0; j < taille; j++) {
            matrice[i][j] = valeurs[i * taille + j];
        }
    }
    return matrice;
}

void libererMatrice(float** matrice, int taille) {
    for (int i = 0; i < taille; i++) {
        free(matrice[i]);
    }
    free(matrice);
}

int main() {
    t_bmp8* image = NULL;
    int choixPrincipal, choixFiltre;
    char chemin[256];

    do {
        afficherMenuPrincipal();
        scanf("%d", &choixPrincipal);
        getchar(); // pour consommer le \n

        switch (choixPrincipal) {
            case 1: // Ouvrir
                printf("Chemin du fichier : ");
                fgets(chemin, sizeof(chemin), stdin);
                chemin[strcspn(chemin, "\n")] = 0; // supprimer le \n

                if (image != NULL) bmp8_free(image);
                image = bmp8_loadImage(chemin);
                if (image) {
                    printf("Image chargée avec succès !\n");
                } else {
                    printf("Erreur lors du chargement de l'image.\n");
                }
                break;

            case 2: // Sauvegarder
                if (!image) {
                    printf("Aucune image chargée.\n");
                    break;
                }
                printf("Chemin du fichier : ");
                fgets(chemin, sizeof(chemin), stdin);
                chemin[strcspn(chemin, "\n")] = 0;
                bmp8_saveImage(chemin, image);
                printf("Image sauvegardée avec succès !\n");
                break;

            case 3: // Appliquer un filtre
                if (!image) {
                    printf("Aucune image chargée.\n");
                    break;
                }
                do {
                    afficherMenuFiltres();
                    scanf("%d", &choixFiltre);
                    getchar(); // consommer \n

                    switch (choixFiltre) {
                        case 1:
                            bmp8_negative(image);
                            printf("Filtre appliqué avec succès !\n");
                            break;

                        case 2: {
                            int val;
                            printf("Valeur de luminosité (entre -255 et 255) : ");
                            scanf("%d", &val);
                            getchar();
                            bmp8_brightness(image, val);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 3: {
                            int seuil;
                            printf("Seuil de binarisation (0-255) : ");
                            scanf("%d", &seuil);
                            getchar();
                            bmp8_threshold(image, seuil);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 4: { // flou moyen
                            float flou[] = {
                                1/9.0, 1/9.0, 1/9.0,
                                1/9.0, 1/9.0, 1/9.0,
                                1/9.0, 1/9.0, 1/9.0
                            };
                            float** kernel = creerMatrice(flou, 3);
                            bmp8_applyFilter(image, kernel, 3);
                            libererMatrice(kernel, 3);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 5: { // flou gaussien
                            float gaussien[] = {
                                1, 2, 1,
                                2, 4, 2,
                                1, 2, 1
                            };
                            for (int i = 0; i < 9; i++) gaussien[i] /= 16.0;
                            float** kernel = creerMatrice(gaussien, 3);
                            bmp8_applyFilter(image, kernel, 3);
                            libererMatrice(kernel, 3);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 6: { // netteté
                            float sharpen[] = {
                                0, -1, 0,
                               -1, 5, -1,
                                0, -1, 0
                            };
                            float** kernel = creerMatrice(sharpen, 3);
                            bmp8_applyFilter(image, kernel, 3);
                            libererMatrice(kernel, 3);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 7: { // contour
                            float edge[] = {
                                -1, -1, -1,
                                -1, 8, -1,
                                -1, -1, -1
                            };
                            float** kernel = creerMatrice(edge, 3);
                            bmp8_applyFilter(image, kernel, 3);
                            libererMatrice(kernel, 3);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 8: { // relief
                            float emboss[] = {
                                -2, -1, 0,
                                -1, 1, 1,
                                 0, 1, 2
                            };
                            float** kernel = creerMatrice(emboss, 3);
                            bmp8_applyFilter(image, kernel, 3);
                            libererMatrice(kernel, 3);
                            printf("Filtre appliqué avec succès !\n");
                            break;
                        }

                        case 9:
                            break;

                        default:
                            printf("Option invalide.\n");
                    }
                } while (choixFiltre != 9);
                break;

            case 4: // Informations
                if (!image) {
                    printf("Aucune image chargée.\n");
                } else {
                    bmp8_printInfo(image);
                }
                break;

            case 5: // Quitter
                printf("Au revoir !\n");
                break;

            default:
                printf("Option invalide.\n");
        }

    } while (choixPrincipal != 5);

    if (image) {
        bmp8_free(image);
    }

    return 0;
}
