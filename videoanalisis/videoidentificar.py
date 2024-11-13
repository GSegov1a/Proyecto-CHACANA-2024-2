import cv2
import numpy as np

# Ruta al archivo de video
video_path = 'Documents/videobueno.AVI'

# Umbral para detectar alta luminosidad
luminosity_threshold = 200  # Ajusta según la intensidad de brillo deseada
edge_margin = 50  # Margen en píxeles para excluir los bordes

# Inicializa el objeto de captura de video
cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print("No se pudo abrir el video.")
    cap.release()
    exit()

# Procesa el video fotograma por fotograma
while True:
    # Lee el siguiente fotograma
    ret, frame = cap.read()
    if not ret:
        break

    # Convierte el fotograma actual a escala de grises
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Define la zona central ignorando los bordes
    central_region = gray[edge_margin:gray.shape[0] - edge_margin, edge_margin:gray.shape[1] - edge_margin]

    # Crea una máscara de las áreas que superan el umbral de luminosidad
    bright_areas = (central_region > luminosity_threshold).astype(np.uint8) * 255

    # Encuentra los contornos de las áreas luminosas
    contours, _ = cv2.findContours(bright_areas, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Dibuja los contornos sobre el fotograma original
    cv2.drawContours(frame[edge_margin:gray.shape[0] - edge_margin, edge_margin:gray.shape[1] - edge_margin], contours, -1, (0, 255, 0), 2)

    # Muestra el fotograma con las áreas luminosas resaltadas
    cv2.imshow('Lugares con Alta Luminosidad', frame)

    # Salir si se presiona la tecla 'q'
    if cv2.waitKey(30) & 0xFF == ord('q'):
        break

# Libera el objeto de captura y cierra todas las ventanas
cap.release()
cv2.destroyAllWindows()
