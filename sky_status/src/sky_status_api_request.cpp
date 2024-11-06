#include "../include/sky_status_api_request.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>


// MANEJAR RESPUESTA API
size_t handleResponse(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// REALIZAR CONSULTA API, RETORNA UN JSON
std::string sky_status_api_request(){
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Inicializa libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Establece la URL de la API
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.open-meteo.com/v1/forecast?latitude=-33.269522&longitude=-70.534691&current_weather=true");

        // Establece la función de callback para manejar la respuesta
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handleResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Realiza la solicitud
        res = curl_easy_perform(curl);

        // Verifica si hubo un error
        if (res != CURLE_OK) {
            readBuffer = "error";
        }

        // Limpia
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return readBuffer;
}
