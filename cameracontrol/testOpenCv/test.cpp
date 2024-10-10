#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Abrir la cámara (0 es la cámara por defecto)
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: No se pudo abrir la cámara." << std::endl;
        return -1;
    }

    // Crear una ventana para mostrar el video
    cv::namedWindow("Video en Vivo", cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    while (true) {
        // Capturar un frame
        cap >> frame;

        // Verificar si se capturó el frame correctamente
        if (frame.empty()) {
            std::cerr << "Error: No se pudo capturar el frame." << std::endl;
            break;
        }

        // Mostrar el frame en la ventana
        cv::imshow("Video en Vivo", frame);

        // Salir si se presiona la tecla 'q'
        if (cv::waitKey(30) >= 0) {
            break;
        }
    }

    // Liberar la cámara y destruir la ventana
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
