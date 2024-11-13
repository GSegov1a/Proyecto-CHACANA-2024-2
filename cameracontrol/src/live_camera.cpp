#include "opencv2/opencv.hpp" 
#include <opencv2/highgui/highgui.hpp>   
#include "ASICamera2.h"
#include "video_processing.h"
#include <iostream>

int main() {
    // VERIFICAMOS CAMARAS CONECTADAS
    int numCameras = ASIGetNumOfConnectedCameras();
    if (numCameras <= 0) {
        std::cerr << "Error: No hay cámaras conectadas o no se pueden detectar." << std::endl;
        return -1;
    }
    std::cout << "Cámaras conectadas: " << numCameras << std::endl;

    // INFORMACIÓN CAMARAS CONECTADAS
    for (int i = 0; i < numCameras; ++i) {
        ASI_CAMERA_INFO camInfo;
        ASI_ERROR_CODE errCode = ASIGetCameraProperty(&camInfo, i);
        if (errCode != ASI_SUCCESS) {
            std::cerr << "Error al obtener las propiedades de la cámara " << i << ". Código de error: " << errCode << std::endl;
            continue;
        }

        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "-----INFORMACIÓN GENERAL-----" << std::endl;
        std::cout << "ID:  " << camInfo.CameraID << std::endl;
        std::cout << "Nombre: " << camInfo.Name << std::endl;
        std::cout << "Resolución Máxima: " << camInfo.MaxWidth << "x" << camInfo.MaxHeight << std::endl;
        std::cout << "Es a Color?: " << (camInfo.IsColorCam == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Profundidad de Bits: " << camInfo.BitDepth << std::endl;

        std::cout << "Bin Soportado: ";
        for (int j = 0; j < 16; ++j) {
            if (camInfo.SupportedBins[j] == 0) {
                break;
            }
            std::cout << camInfo.SupportedBins[j] << " ";
        }
        std::cout << std::endl;

        std::cout << "Formato de Video Soportado: ";
        for (int j = 0; j < 8; ++j) {
            ASI_IMG_TYPE format = camInfo.SupportedVideoFormat[j];
            if (format == ASI_IMG_END) break;
            std::string formatName;
            switch (format) {
                case ASI_IMG_RAW8:
                    formatName = "RAW8";
                    break;
                case ASI_IMG_RGB24:
                    formatName = "RGB24";
                    break;
                case ASI_IMG_RAW16:
                    formatName = "RAW16";
                    break;
                case ASI_IMG_Y8:
                    formatName = "Y8";
                    break;
                default:
                    formatName = "Desconocido";
                    break;
            }
            std::cout << formatName << " ";
        }
        std::cout << std::endl;

        std::cout << "Bayer Pattern: ";
        ASI_BAYER_PATTERN pattern = camInfo.BayerPattern;
        std::string patternName;
        switch (pattern) {
            case ASI_BAYER_RG:
                patternName = "Bayer RG";
                break;
            case ASI_BAYER_BG:
                patternName = "Bayer BG";
                break;
            case ASI_BAYER_GR:
                patternName = "Bayer GR";
                break;
            case ASI_BAYER_GB:
                patternName = "Bayer GB";
                break;
            default:
                patternName = "Desconocido";
                break;
        }
        std::cout << patternName << std::endl;

        std::cout << "Shutter Mecánico?: " << (camInfo.MechanicalShutter == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Trigger Cam: " << (camInfo.IsTriggerCam == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Puerto ST4?: " << (camInfo.ST4Port == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Cooler?: " << (camInfo.IsCoolerCam == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Camara USB 3.0?: " << (camInfo.IsUSB3Camera == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Host USB 3.0?: " << (camInfo.IsUSB3Host == ASI_TRUE ? "SI" : "NO") << std::endl;
        std::cout << "Tamaño de Pixel: " << camInfo.PixelSize << "um" << std::endl;
        std::cout << "ElecPerADU: " << camInfo.ElecPerADU << std::endl;
        
        // ABRIMOS LA CAMARA
        errCode = ASIOpenCamera(i);
        if (errCode != ASI_SUCCESS) {
            std::cerr << "Error al abrir la cámara " << i << ". Código de error: " << errCode << std::endl;
            continue;
        }

        // OBTENEMOS INFORMACIÓN DE LOS CONTROLES DE LA CAMARA
        int numControls;
        errCode = ASIGetNumOfControls(i, &numControls);
        if (errCode != ASI_SUCCESS) {
            std::cerr << "Error al obtener el número de controles de la cámara " << i << ". Código de error: " << errCode << std::endl;
            ASICloseCamera(i);
            continue;
        }

        std::cout << "-----INFORMACIÓN CONTROLES-----" << std::endl;
        std::cout << "Número de controles: " << numControls << std::endl;

        for (int j = 0; j < numControls; ++j) {
            ASI_CONTROL_CAPS controlCaps;
            errCode = ASIGetControlCaps(i, j, &controlCaps);
            if (errCode != ASI_SUCCESS) {
                std::cerr << "Error al obtener los detalles del control " << j << " de la cámara " << i << ". Código de error: " << errCode << std::endl;
                continue;
            }

            std::cout << "-Nombre: " << controlCaps.Name << std::endl;
            std::cout << "   Control ID: " << j << std::endl;
            std::cout << "   Descripción: " << controlCaps.Description << std::endl;
            std::cout << "   Valor Mínimo / Máximo / Default: " << controlCaps.MinValue << " / " << controlCaps.MaxValue << " / " << controlCaps.DefaultValue << std::endl;
            std::cout << "   Control Auto Ajustable?: " << (controlCaps.IsAutoSupported == ASI_TRUE ? "SI" : "NO") << std::endl;
            std::cout << "   Seteable?: " << (controlCaps.IsWritable == ASI_TRUE ? "SI" : "NO") << std::endl;
        }

        // CERRAMOS LA CAMARA
        errCode = ASICloseCamera(i);
        if (errCode != ASI_SUCCESS) {
            std::cerr << "Error al cerrar la cámara " << i << ". Código de error: " << errCode << std::endl;
        }

        std::cout << "------------------------------------------------------------" << std::endl;
    }
    

    //COMENZAMOS GRABACION
    int cameraId = 0;
    int cameraWidth = 1800; // VALORES DE ANCHO MAXIMA DE LA CAMARA 3096
    int cameraHeight = 1500; // VALORES DE ALTO MAXIMA DE LA CAMARA 2080
    
    ASI_IMG_TYPE imgType = ASI_IMG_RAW8; //IMPORTANTE: EN CASO DE SER RAW16 MULTIPLICAR BUFFERSIZE POR 2
    int cameraBinning = 1;
    int cameraExp = 100000; // en microsegundos 1000 us = 1 ms
    int cameraGain = 400;
    int cameraBandWidth = 75;
    int cameraHightSpeedMode = 0;

    int realCameraWidth = cameraWidth / cameraBinning;
    int realCameraHeight = cameraHeight / cameraBinning;

    int bufferSize = realCameraWidth * realCameraHeight; 
    unsigned char* frameBuffer = new unsigned char[bufferSize];

    ASI_ERROR_CODE errCode;

    // INICIALIZAR VENTANTA EN VIVO
    cv::namedWindow("Live Camera", cv::WINDOW_NORMAL);
    cv::resizeWindow("Live Camera", realCameraWidth / 2 , realCameraHeight / 2 );

    // INICIALIZAR GUARDADO DE VIDEO
    std::string videoFileName = "records/output_bin" + std::to_string(cameraBinning) + "_exp" + std::to_string(cameraExp) + "us" + "_gain" + std::to_string(cameraGain) + "_bandwidth" + std::to_string(cameraBandWidth) + "_" + std::to_string(realCameraWidth) + "x" + std::to_string(realCameraHeight) + ".avi";
    cv::VideoWriter videoWriter;
    videoWriter.open(videoFileName, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 10, cv::Size(realCameraWidth, realCameraHeight), false);
    
    if (!videoWriter.isOpened()) {
        std::cerr << "Error al abrir el VideoWriter." << std::endl;
        return -1;
    }

    std::cout << "COMIENZA GRABACION CON CAMARA 0" << std::endl;
    
    errCode = ASIOpenCamera(cameraId);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al abrir la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASIInitCamera(cameraId);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al inicializar la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetControlValue(cameraId, ASI_EXPOSURE, cameraExp, ASI_FALSE);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear la exposicion en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetControlValue(cameraId, ASI_BANDWIDTHOVERLOAD, cameraBandWidth, ASI_FALSE);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear el ancho de banda en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetControlValue(cameraId, ASI_HIGH_SPEED_MODE, cameraHightSpeedMode, ASI_FALSE);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear el high speed mode en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetControlValue(cameraId, ASI_EXPOSURE, cameraExp, ASI_FALSE);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear la exposicion en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetControlValue(cameraId, ASI_GAIN, cameraGain, ASI_FALSE);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear la ganancia en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASISetROIFormat(cameraId, realCameraWidth, realCameraHeight, cameraBinning, imgType);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al setear el ROI format en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }

    errCode = ASIStartVideoCapture(cameraId);
    if (errCode != ASI_SUCCESS) {
        std::cerr << "Error al comenzar la grabacion en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
        return -1;
    }


    int framesCount = 0;
    double fps = 0.0;
    double tickFrequency = cv::getTickFrequency();
    double lastTime = cv::getTickCount(); // Variable para calcular FPS cada iteracion
    double startTime = cv::getTickCount(); // Variable para calcular cuanto tiempo se grabo en total

    while (true) {
        errCode = ASIGetVideoData(cameraId, frameBuffer, bufferSize, 2*cameraExp + 500);
        if (errCode != ASI_SUCCESS) {
            std::cerr << "Error al obtener los datos de un frame en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
            continue;
        }
        framesCount += 1;

        // Convertir el buffer a cv::Mat (formato de imagen en escala de grises RAW8)
        cv::Mat img(realCameraHeight, realCameraWidth, CV_8UC1, frameBuffer);



        videoWriter.write(img);
        // Mostrar el frame en la ventana
        cv::imshow("Live Camera", img);
        
        // Calcular el tiempo transcurrido y el FPS
        double currentTime = cv::getTickCount();
        double timeElapsed = (currentTime - lastTime) / tickFrequency;
        fps = 1.0 / timeElapsed;
        lastTime = currentTime; 

        // Mostrar FPS en la consola
        std::cout << "FPS: " << fps << std::endl;

        // Esperar 1 ms entre frames (permite cerrar la ventana con 'q')
        if (cv::waitKey(1) == 'q') {
            break;
        }
        
    }

    // Calcular el tiempo total de grabación y printear
    double endTime = cv::getTickCount();
    double recordingDuration = (endTime - startTime) / tickFrequency;

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Tiempo total de grabacion: " << recordingDuration << " segundos." << std::endl;
    std::cout << "Frames grabados: " << framesCount << std::endl;

    delete[] frameBuffer;

    errCode = ASIStopVideoCapture(cameraId);
    if (errCode != ASI_SUCCESS) {
       std::cerr << "Error al terminar la grabacion en la cámara de grabacion. " << "Código de error: " << errCode << std::endl;    
    }

    errCode = ASICloseCamera(cameraId);
    if (errCode != ASI_SUCCESS) {
       std::cerr << "Error al cerrar la cámara de grabacion. " << "Código de error: " << errCode << std::endl;
    }

    videoWriter.release(); // Liberar el VideoWriter


    //ANÁLISIS DE VIDEO

    // Configuración inicial
    std::string video_path = videoFileName;
    int luminosity_threshold = 80;  // Umbral de luminosidad
    int edge_margin = 50;           // Margen en píxeles
    int min_movement_size = 5;      // Tamaño mínimo en píxeles de la región de cambio drástico

    // Inicializa el objeto de captura de video
    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir el video o leer el primer fotograma." << std::endl;
        return -1;
    }

    // Lee el primer fotograma para establecer el estado inicial
    cv::Mat prev_frame, prev_gray;
    cap >> prev_frame;
    if (prev_frame.empty()) {
        std::cerr << "El primer fotograma está vacío." << std::endl;
        return -1;
    }

    // Convierte el primer fotograma a escala de grises
    cv::cvtColor(prev_frame, prev_gray, cv::COLOR_BGR2GRAY);

    // Procesa el video fotograma por fotograma
    while (true) {
        cv::Mat frame, gray;
        cap >> frame;
        if (frame.empty()) break;  // Fin del video

        // Convierte el fotograma actual a escala de grises
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Procesa la diferencia entre fotogramas para detectar áreas de cambio luminoso
        cv::Mat bright_areas = processFrameDifference(prev_gray, gray, luminosity_threshold, edge_margin);

        // Detecta los cambios luminosos y dibuja en el fotograma
        detectLuminosityChanges(frame, bright_areas, min_movement_size, edge_margin);

        // Actualiza el fotograma anterior
        prev_gray = gray.clone();
    }

    // Libera el objeto de captura y cierra todas las ventanas
    cap.release();

    cv::destroyAllWindows(); // Cerrar la ventana

    return 0;
}
