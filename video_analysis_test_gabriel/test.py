import cv2

# Ruta del video
video_path = './video_analysis/1024x768-bin2-raw8-400gain-100msexp.AVI'
cap = cv2.VideoCapture(video_path)

# Leer los dos primeros frames del video
ret, frame1 = cap.read()
if not ret:
    print("No se pudo leer el primer frame del video.")
    exit()
    
ret, frame2 = cap.read()
if not ret:
    print("No se pudo leer el segundo frame del video.")
    exit()

# Convertir los frames a escala de grises
frame1_gray = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
frame2_gray = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

# Calcular la diferencia absoluta entre los dos frames
diff = cv2.absdiff(frame1_gray, frame2_gray)

# Aplicar un umbral a la diferencia
threshold_value = 30  # Ajusta este valor según sea necesario
T, binary_diff = cv2.threshold(diff, threshold_value, 255, cv2.THRESH_BINARY)

# Mostrar las imágenes
cv2.imshow('Frame 1', frame1)
cv2.imshow('Frame 2', frame2)
cv2.imshow('Diferencia Absoluta', diff)
cv2.imshow('Umbral de Diferencia', binary_diff)

# Esperar hasta que se presione una tecla y cerrar las ventanas
cv2.waitKey(0)
cv2.destroyAllWindows()

# Liberar el objeto de captura de video
cap.release()

