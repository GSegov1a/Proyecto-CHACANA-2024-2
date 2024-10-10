#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>
#include "../include/sky_status_api_request.hpp"



// Recibir Señal Terminar Proceso
void end_signal(int sig){
    if (sig == SIGTERM) {
        printf("Proceso Finalizado\n");
        exit(EXIT_SUCCESS);
    }
}

// Obtener la Hora Actual
std::string obtenerHoraActual() {
    std::time_t now = std::time(nullptr);
    char* date_time = std::ctime(&now);
    std::string string_date = std::string(date_time);
    
    string_date.pop_back();
    // Convertir a std::string para manejar la cadena de manera segura
    return string_date;
}



// Hilo Principal
int main() {
    pid_t sid;
    pid_t pid;
    
    // Crear un proceso hijo
    pid = fork();
    if (pid < 0){
        perror("Error: Fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);   
    }

    // Crear Nueva Sesion
    sid = setsid();
    if (sid < 0) {
        perror("Error: Set ID");
        exit(EXIT_FAILURE);
    }

    //Cerrar Descriptores Estandar
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Configurar Log
    int log = open("./logs/running.log", O_RDWR | O_CREAT | O_APPEND, 0600);
    if (log < 0) {
        perror("Error: Log File");
        exit(EXIT_FAILURE);
    }

    // Redirigir  Logs
    if (dup2(log, STDOUT_FILENO) < 0 || dup2(log, STDERR_FILENO) < 0) {
        perror("Error: Log Redirect");
        close(log);
        exit(EXIT_FAILURE);
    }
    close(log);

    // Abrir Archivo PID
    int pid_file_desc = open("./logs/pid.log", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (pid_file_desc < 0) {
        perror("Error: PID file");
        exit(EXIT_FAILURE);
    }

    // Escribir PID
    FILE* pid_file = fdopen(pid_file_desc,"w");
    if (pid_file) {
        fprintf(pid_file,"%d", getpid());
        fflush(pid_file);
        fclose(pid_file);
    }
    else{
        perror("Error: PID File Open");
        exit(EXIT_FAILURE);
    }


    // Manejar Señal
    if (signal(SIGTERM, end_signal) == SIG_ERR) {
        perror("Error: Handling Signal");
        exit(EXIT_FAILURE);
    }

    // Se Trabaja en la Raiz
    if (chdir("/") < 0) {
        perror("Error: Directory Change");
        exit(EXIT_FAILURE);
    }

    // Bucle Daemon
    while (true){
        std::string date = obtenerHoraActual();
        std::string str_weather = sky_status_api_request();
        std::string str_is_day;
        std::string str_weather_code;
        std::string str_temperature;

        if(str_weather != "error"){
            nlohmann::json json_weather = nlohmann::json::parse(str_weather);
            int is_day = json_weather["current_weather"]["is_day"];
            int weather_code = json_weather["current_weather"]["weathercode"];
            int temp = json_weather["current_weather"]["temperature"];
            str_temperature = std::to_string(temp) + " C";
            
            if (is_day == 0 ){
            str_is_day = "Noche";
            } else if (is_day == 1){
                str_is_day = "Dia";
            }

            if (weather_code == 0){
                str_weather_code = "Despejado";
            } else if (weather_code == 1){
                str_weather_code = "Casi Despejado";
            } else {
                str_weather_code = "No Apto";
            }

        } else {
            str_is_day = "error";
            str_weather_code = "error";
            str_temperature = "error";
        }

        
        fprintf(stdout, "[%s] Estado: [%s][%s][%s]\n", date.c_str(), str_is_day.c_str(), str_weather_code.c_str(), str_temperature.c_str());
        fflush(stdout);
        sleep(60*60);
    }
    
}

