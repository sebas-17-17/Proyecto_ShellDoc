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
    fprintf(archivo, "theme: cosmo\n");
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
    pid_t pid1;
    sem_t *sem;

    printf("[Integrante 2] Iniciando control de procesos y semáforos...\n");

    // 1. CREAR EL SEMÁFORO NOMBRADO QUE ESPERA EL INTEGRANTE 1 Y 3
    sem = sem_open(SEM_QUARTO, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("Error al crear el semáforo SEM_QUARTO");
        exit(EXIT_FAILURE);
    }

    // 2. CREAR EL PROCESO HIJO MEDIANTE FORK
    pid1 = fork();

    if (pid1 < 0) {
        perror("Error al ejecutar fork");
        sem_unlink(SEM_QUARTO); 
        exit(EXIT_FAILURE);
    } 
    else if (pid1 == 0) {
        // --- PROCESO HIJO 1 ---
        // Invoca la función de captura intacta como pide la guía
        proceso_captura_terminal(); 
        exit(EXIT_SUCCESS);
    } 
    else {
        // --- PROCESO PADRE ---
        printf("[Padre] Proceso de captura asignado al Hijo 1 con PID: %d\n", pid1);
        
        // Esperamos a que el proceso hijo termine su ejecución por completo
        wait(NULL); 
        
        printf("[Padre] El Hijo 1 ha terminado. Pasando control al renderizador.\n");
        
        // 3. LIMPIEZA DEL SEMÁFORO AL FINALIZAR EL PROGRAMA
        sem_close(sem);
        sem_unlink(SEM_QUARTO); 
        
        printf("[Main] Flujo del Integrante 2 completado con éxito.\n");
    }

    return 0;
}
