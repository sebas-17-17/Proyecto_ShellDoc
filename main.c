#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>

// Semáforo para el Integrante 3 (Renderizador)
#define SEM_QUARTO "/sem_quarto"

void proceso_captura_terminal() {
    printf("Captura de comandos ejecutados en la sesión Linux\n");

    //Crear o abrir el archivo .qmd para Quarto
    FILE *archivo = fopen("documento.qmd", "w");
    if (archivo == NULL) {
        perror("Error al crear el archivo documento.qmd");
        exit(1);
    }

    //Encabezado de Quarto
    fprintf(archivo, "---\n");
    fprintf(archivo, "title: \"Auditoría de Sistema y Procesos Linux\"\n");
    fprintf(archivo, "author: \"Grupo 5 - ShellDoc Web\"\n");
    fprintf(archivo, "format: html\n");
    fprintf(archivo, "theme: cosmic\n");
    fprintf(archivo, "---\n\n");

    fprintf(archivo, "Proyecto SheelDoc\n\n");
    fprintf(archivo, "Documento fue generado automáticamente por un programa en C interactuando con la Shell de Linux.\n\n");

    //Comando para indetificar user 
    fprintf(archivo, "Identificar usuario y servidor\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ whoami && hostname\n");
    fflush(archivo); 
    fclose(archivo);

    system("whoami && hostname >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "```\n\n");


    //Comando para ver memoria RAM
    fprintf(archivo, "Estado de la Memoria RAM\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ free -h\n");
    fflush(archivo); 
    fclose(archivo);

    system("free -h >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "```\n\n");


    //Comando para ver el almacenamiento
    fprintf(archivo, "Uso del Disco Principal\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ df -h /\n");
    fflush(archivo); 
    fclose(archivo);

    system("df -h / >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "```\n\n");


    //Comando para ver los procesos 
    fprintf(archivo, "Procesos con Mayor Consumo de Memoria (ps)\n\n");
    fprintf(archivo, "Muestra el PID y el PPID de los procesos activos:\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ ps -eo pid,ppid,cmd,%%mem --sort=-%%mem | head -n 11\n");
    fflush(archivo); 
    fclose(archivo);

    system("ps -eo pid,ppid,cmd,%mem --sort=-%mem | head -n 11 >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "```\n");


    //Cerrar el archivo
    fclose(archivo);

    //SEÑAL PARA EL INTEGRANTE 3 (Renderizador)
    //Se abre el semáforo que creará el Integrante 2 en el main
    sem_t *sem = sem_open(SEM_QUARTO, 0); 
    if (sem != SEM_FAILED) {
        printf("Enviando señal semSignal(sem_quarto) para iniciar renderizado\n");
        sem_post(sem); // Esto equivale al semSignal()
        sem_close(sem);
    } else {
        printf("Semáforo no encontrado\n");
    }
}

int main() {
    // Llamada de prueba 
    proceso_captura_terminal();
    return 0;
}
