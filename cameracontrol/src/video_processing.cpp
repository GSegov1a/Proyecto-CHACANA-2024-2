#include "video_processing.h"
#include <iostream>

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
void detectLuminosityChanges(const cv::Mat& frame, const cv::Mat& bright_areas, int min_movement_size, int edge_margin, bool detection_made) {
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

            // Muestra el cambio detectado
            cv::imshow("Cambio Drástico Detectado", frame);
            cv::waitKey(0);  // Pausa hasta que se cierre la ventana
            break;
        }
    }
}
