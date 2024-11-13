import cv2
import numpy as np

# Ruta al archivo de video (cambiarlo según conveniencia)
video_path = 'rocimarquez/videobueno.AVI'

# Umbral alto para detectar un cambio drástico en la luminosidad
luminosity_threshold = 200  # Ajusta según la intensidad del brillo esperado
edge_margin = 50  # Margen en píxeles para excluir los bordes

# Inicializa el objeto de captura de video
cap = cv2.VideoCapture(video_path)

# Lee el primer fotograma para inicializar
ret, prev_frame = cap.read()
if not ret:
    print("No se pudo leer el video.")
    cap.release()
    exit()

# Convierte el primer fotograma a escala de grises
prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)

# Obtén las dimensiones del fotograma
height, width = prev_gray.shape

# Procesa el video fotograma por fotograma
frame_count = 0
found_change = False

while True:
    # Lee el siguiente fotograma
    ret, frame = cap.read()
    if not ret:
        break

    # Convierte el fotograma actual a escala de grises
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Calcula la diferencia absoluta entre el fotograma actual y el anterior
    frame_diff = cv2.absdiff(gray, prev_gray)
    
    # Define la zona central ignorando el borde
    central_region = frame_diff[edge_margin:height-edge_margin, edge_margin:width-edge_margin]
    
    # Encuentra la posición del primer pixel que supera el umbral en la zona central
    change_indices = np.where(central_region > luminosity_threshold)
    if change_indices[0].size > 0 and not found_change:
        # Ajusta las coordenadas para la zona central
        y, x = change_indices[0][0] + edge_margin, change_indices[1][0] + edge_margin
        print(f"Cambio drástico detectado en el fotograma {frame_count} en el pixel (x={x}, y={y})")
        found_change = True
        
        # Dibuja un círculo en el fotograma para señalar el pixel con el cambio
        marked_frame = frame.copy()
        cv2.circle(marked_frame, (x, y), radius=5, color=(0, 0, 255), thickness=2)
        
        # Configura la ventana para mostrar la imagen completa
        cv2.namedWindow('Cambio Drástico Detectado', cv2.WINDOW_NORMAL)
        cv2.imshow('Cambio Drástico Detectado', marked_frame)
        cv2.waitKey(0)  # Espera a que el usuario cierre la ventana

    # Actualiza el fotograma anterior
    prev_gray = gray
    frame_count += 1

# Libera el objeto de captura y cierra todas las ventanas
cap.release()
cv2.destroyAllWindows()
