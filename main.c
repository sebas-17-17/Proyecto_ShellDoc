#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

// Semáforo para el Integrante 3 (Renderizador )
// sem_post equivale a semSignal
#define SEM_QUARTO "/sem_quarto"
#define SEM_CLOUD "/sem_cloud"  //nuevo semaforo para sincronizar proceso de despliegue ---

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
    fprintf(archivo, "title: \"Documentación de Sesiones Linux con Quarto, Hilos y Procesos en C\"\n");
    fprintf(archivo, "author: \"Grupo 3 - Sistemas Operativos\"\n");
    fprintf(archivo, "format: html\n");
    fprintf(archivo, "theme: cosmo\n");
    fprintf(archivo, "---\n\n");

    fprintf(archivo, "# Proyecto ShellDoc\n\n");
    fprintf(archivo, "Documento generado automáticamente por un programa en C interactuando con la Shell de Linux.\n\n");

    //Comando para indetificar user 
    fprintf(archivo, "## Identificar usuario y servidor\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ whoami && hostname\n");
    fflush(archivo); 
    fclose(archivo);

    system("(whoami && hostname) >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "\n```\n\n");


    //Comando para ver memoria RAM
    fprintf(archivo, "## Estado de la Memoria RAM\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ free -h\n");
    fflush(archivo); 
    fclose(archivo);

    system("free -h >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "\n```\n\n");


    //Comando para ver el almacenamiento
    fprintf(archivo, "## Uso del Disco Principal\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ df -h /\n");
    fflush(archivo); 
    fclose(archivo);

    system("df -h / >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "\n```\n\n");


    //Comando para ver los procesos 
    fprintf(archivo, "## Procesos con Mayor Consumo de Memoria (ps)\n\n");
    fprintf(archivo, "Muestra el PID, el PPID y memoeria utilizados:\n\n");
    fprintf(archivo, "```bash\n");
    fprintf(archivo, "$ ps -eo pid,ppid,cmd,%%mem --sort=-%%mem | head -n 11\n");
    fflush(archivo); 
    fclose(archivo);

    system("ps -eo pid,ppid,cmd,%mem --sort=-%mem | head -n 11 >> documento.qmd"); 

    archivo = fopen("documento.qmd", "a");
    fprintf(archivo, "\n```\n");


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

void proceso_renderizado_quarto() {
    printf("Proceso 2 (Renderizador) en espera de señal del Proceso 1.\n");
    sem_t *sem = sem_open(SEM_QUARTO, 0);
    if (sem != SEM_FAILED) {
        sem_wait(sem);
        sem_close(sem);
    } else {
        perror("Error al abrir el semáforo en el Proceso 2");
        exit(EXIT_FAILURE);
    }
    
    printf("Señal recibida por proceso 2. El archivo QMD está listo.\n");
    
    // 1. Renderiza el HTML local estándar
    int resultado = system("quarto render documento.qmd");
    
    if (resultado == 0) {
        printf("El archivo documento.html ha sido generado mediante Quarto.\n");
        
        // 2. AUTOMATIZACIÓN LOCAL: Levanta el servidor local en puerto 8080 en segundo plano (&)
        // El " > /dev/null 2>&1 &" hace que corra oculto sin congelar la terminal
        printf("Levantando servidor de previsualización local en http://localhost:8080/ \n");
        system("quarto preview documento.qmd --port 8080 --host 0.0.0.0 > /dev/null 2>&1 &");
        
    } else {
        printf("Error en proceso 2: No se pudo renderizar documento.qmd.\n");
    }

    // 3. AUTOMATIZACIÓN EN LA NUBE (Quarto Pub / GitHub)
    // Notifica al Proceso 3 para que haga el git push. 
    // Como activamos el "Automatically publish on push" en Posit Connect, Quarto Pub se actualizará SOLO.
    sem_t *sem_cloud = sem_open(SEM_CLOUD, 0);
    if (sem_cloud != SEM_FAILED) {
        printf("Proceso 2 notificando al proceso de despliegue en la nube\n");
        sem_post(sem_cloud);
        sem_close(sem_cloud);
    }
}

           //proceso 3 I4--- 
void proceso_deploy() {
	printf("Proceso 3 (despliegue) en espera de señal del proceso 2\n");

	sem_t *sem = sem_open(SEM_CLOUD,0);



	if (sem == SEM_FAILED) {
	    perror("Error al abrir SEM_CLOUD");
	    exit(EXIT_FAILURE);
	}
    	
	sem_wait(sem);
	sem_close(sem);

	printf("Proceso 3 ejecutando despliegue a github\n");

	system("git add .");
	system("git commit -m \"deploy automatico\"");
	system("git push");

	printf("Despliegue completado \n");
   
}


int main() {
    pid_t pid1, pid2, pid3;  
    sem_t *sem, *sem_cloud;

    printf("Iniciando control de procesos y semáforos...\n");

    // 1. CREAR EL SEMÁFORO NOMBRADO QUE ESPERA EL INTEGRANTE 1 Y 3
    sem = sem_open(SEM_QUARTO, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("Error al crear el semáforo SEM_QUARTO");
        exit(EXIT_FAILURE);
    }
    // modificacion I4 creacion de semaforo para controla el deploy (proceso3)
    sem_cloud = sem_open(SEM_CLOUD, O_CREAT, 0644, 0);
    if (sem_cloud == SEM_FAILED) {
	perror("Error al crear el semaforo SEM_CLOUD");
	exit(EXIT_FAILURE);
    }

   // PROCESO 1    

    // 2. CREAR EL PROCESO HIJO MEDIANTE FORK
    pid1 = fork();

    if (pid1 < 0) {
        perror("Error al ejecutar fork");

        exit(EXIT_FAILURE);
    } 
    else if (pid1 == 0) {
        // --- PROCESO HIJO 1 ---
        // Invoca la función de captura intacta como pide la guía
        proceso_captura_terminal();
        exit(EXIT_SUCCESS);
    
    }
    //proceso 2 render
    pid2 = fork();

    if (pid2 == 0) {
        proceso_renderizado_quarto();
        exit(EXIT_SUCCESS);
    }
    //proceso 3 deploy 
    pid3 = fork();

    if (pid3 == 0) {
	proceso_deploy();
	exit(EXIT_SUCCESS);
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

        // 3. LIMPIEZA DEL SEMÁFORO AL FINALIZAR EL PROGRAMA
    sem_close(sem);
    sem_unlink(SEM_QUARTO);

    sem_close(sem_cloud);
    sem_unlink(SEM_CLOUD); 
        
        printf("Flujo del completado con éxito.\n");
    

    return 0;
}
