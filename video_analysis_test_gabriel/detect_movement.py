import cv2

video_path = './video_analysis/1024x768-bin2-raw8-400gain-100msexp.AVI'
cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print("Error opening video stream or file")
    exit()

# Lee el primer cuadro y conviértelo a escala de grises
ret, frame1 = cap.read()
if not ret:
    print("No se pudo leer el primer cuadro")
    exit()

frame1_gray = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)

cv2.namedWindow('Detección de Movimiento', cv2.WINDOW_NORMAL)


while cap.isOpened():
    ret, frame2 = cap.read()
    if not ret:
        break

    # Convertir el segundo cuadro a escala de grises
    frame2_gray = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

    # Calcular la diferencia absoluta entre cuadros
    diff = cv2.absdiff(frame1_gray, frame2_gray)

    # Aplicar un umbral para resaltar las áreas con movimiento
    _, thresh = cv2.threshold(diff, 110, 255, cv2.THRESH_BINARY)  # Umbral más bajo


    # Dilatar la imagen para llenar los huecos
    thresh = cv2.dilate(thresh, None, iterations=3)
    cv2.imshow('Umbral Dilatado', thresh)


    # Encuentra los contornos
    contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Inicializar un cuadro para dibujar los contornos
    frame_with_contours = frame2.copy()

    # Dibujar los contornos en el cuadro original
    for contour in contours:
        (x, y, w, h) = cv2.boundingRect(contour)
        cv2.rectangle(frame_with_contours, (x, y), (x + w, y + h), (0, 255, 0), 2)  # Marco verde

    # Mostrar el cuadro con los contornos
    cv2.imshow('Detección de Movimiento', frame_with_contours)

    # Esperar a que el usuario presione una tecla
    key = cv2.waitKey(0)  # Espera indefinidamente hasta que se presione una tecla
    if key == ord('q'):  # Si se presiona 'q', salir
        break
    elif key == ord('n'):  # Si se presiona 'n', avanzar al siguiente cuadro
        frame1_gray = frame2_gray  # Actualizar el cuadro de referencia para la siguiente iteración
        continue  # Avanzar al siguiente cuadro

# Libera el objeto de captura de video y cierra las ventanas
cap.release()
cv2.destroyAllWindows()
