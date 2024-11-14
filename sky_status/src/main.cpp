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


// Función para eliminar el archivo PID cuando el proceso termine
void delete_pid_file() {
    if (remove("./logs/pid.log") != 0) {
        perror("Error al eliminar el archivo PID");
    } else {
        printf("Archivo PID eliminado exitosamente.\n");
    }
}

// Recibir Señal Terminar Proceso
void end_signal(int sig){
    if (sig == SIGTERM) {
        printf("Proceso Finalizado\n");
        delete_pid_file();
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

// Obtener el mes y el año actual
std::string obtenerMesYAnio() {
    std::time_t now = std::time(nullptr);

    std::tm* local_time = std::localtime(&now);
    
    int mes = local_time->tm_mon + 1;  // El mes comienza desde 0, así que se suma 1
    int anio = local_time->tm_year + 1900;  // El año comienza desde 1900, así que se suma 1900
    
    return std::to_string(mes) + "-" + std::to_string(anio);
}

// Verificar si es un nuevo mes
bool esNuevoMes() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    int dia = local_time->tm_mday; 
  
        if (dia == 1) {
        return true;
    } else {
        return false;
    }
}

void configurarOut(){
    if (esNuevoMes()) {
        std::string mes_anio = obtenerMesYAnio();
        std::string log_path = "./logs/" + mes_anio + ".log";
        int log = open(log_path.c_str(), O_RDWR | O_CREAT | O_APPEND, 0600);
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
    }
    return;
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
    std::string mes_anio = obtenerMesYAnio();
    std::string log_path = "./logs/" + mes_anio + ".log";
    int log = open(log_path.c_str(), O_RDWR | O_CREAT | O_APPEND, 0600);
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

    // Bucle Daemon
    while (true){
        configurarOut();
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

        if (str_is_day == "Noche" && str_weather_code == "Despejado") {
            std::system("cd /home/oduc/Escritorio/Proyecto-CHACANA-2024-2/cameracontrol && make run ARGS=20 > /dev/null 2>&1");
        }
        
        fprintf(stdout, "[%s] Estado: [%s][%s][%s]\n", date.c_str(), str_is_day.c_str(), str_weather_code.c_str(), str_temperature.c_str());
        fflush(stdout);
        sleep(60*60);
    }
    
}

