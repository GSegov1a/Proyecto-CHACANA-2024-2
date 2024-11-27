#include "video_processing.h"
#include <iostream>
#include <ctime> // Para obtener la fecha y hora actuales
#include <sstream> // Para convertir los valores a cadenas de texto
#include <sys/stat.h>

// Procesa la diferencia entre dos fotogramas y devuelve las áreas brillantes
cv::Mat processFrameDifference(const cv::Mat& prev_gray, const cv::Mat& gray, int luminosity_threshold, int edge_margin) {
    cv::Mat frame_diff;
    cv::absdiff(gray, prev_gray, frame_diff);

    // Define la zona central ignorando los bordes
    cv::Rect central_region(edge_margin, edge_margin, frame_diff.cols - 2 * edge_margin, frame_diff.rows - 2 * edge_margin);
    cv::Mat central_frame_diff = frame_diff(central_region);

    // Crea una máscara para detectar regiones luminosas que superan el umbral
    cv::Mat bright_areas;
    cv::threshold(central_frame_diff, bright_areas, luminosity_threshold, 255, cv::THRESH_BINARY);

    // Dilata para unir regiones cercanas y filtrar ruido
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(bright_areas, bright_areas, kernel);

    return bright_areas;
}

// Detecta cambios luminosos y dibuja un rectángulo en las áreas con cambio drástico
bool detectLuminosityChanges(const cv::Mat& frame, const cv::Mat& bright_areas, int min_movement_size, int edge_margin, bool detection_made) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bright_areas, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Revisa cada contorno para detectar áreas grandes
    for (const auto& contour : contours) {
        cv::Rect bounding_box = cv::boundingRect(contour);
        if (bounding_box.width >= min_movement_size || bounding_box.height >= min_movement_size) {
            detection_made = true; //se detecto un cambio
            bounding_box.x += edge_margin;
            bounding_box.y += edge_margin;
            std::cout << "Cambio drástico detectado en la región: posición inicial (x=" << bounding_box.x
                      << ", y=" << bounding_box.y << "), ancho=" << bounding_box.width
                      << ", alto=" << bounding_box.height << std::endl;

            // Dibuja un rectángulo alrededor de la región detectada
            cv::rectangle(frame, bounding_box, cv::Scalar(0, 0, 255), 2);
                        // Crear un nombre único para el archivo de imagen
                                    // Crear un formato para la fecha y hora: "yyyy-mm-dd_hh-mm-ss"
                                                // Obtener la fecha y hora actuales
 std::time_t current_time = std::time(nullptr);
    std::tm* local_time = std::localtime(&current_time);

    // Formato de la fecha: año-mes-día_hora-minuto-segundo
    std::stringstream date_stream;
    date_stream << (local_time->tm_year + 1900) << "-" 
                << (local_time->tm_mon + 1) << "-" 
                << local_time->tm_mday;

    std::string date_str = date_stream.str();

    // Crear el directorio "records/cambios_detectados/fecha_de_video" si no existe
    std::string directory = "records/cambio_detectado/" + date_str;
    struct stat info;
    if (stat(directory.c_str(), &info) != 0) {
        // El directorio no existe, lo creamos
        if (mkdir(directory.c_str(), 0777) == -1) {
            std::cerr << "Error al crear el directorio: " << directory << std::endl;
            
        }
    }

    // Definir el nombre del archivo con un nombre único
    std::string file_name = directory + "frame_" + date_str + std::to_string(std::time(nullptr))+ ".png";
    
            // Guardar la imagen en la carpeta "records"
            if (cv::imwrite(file_name, frame)) {
                //std::cout << "Fotograma guardado en: " << filename << std::endl;
            } else {
                std::cout << "Error al guardar la imagen." << std::endl;
            }

            // Muestra el cambio detectado
            //cv::imshow("Cambio Drástico Detectado", frame);
            //cv::waitKey(1);
              // Pausa hasta que se cierre la ventana
            break;
        }
    }
    return detection_made;
}
