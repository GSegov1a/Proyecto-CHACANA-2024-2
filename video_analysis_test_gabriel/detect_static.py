import cv2

video_path = './video_analysis/output_bin1_exp100000us_gain300_bandwidth75_1800x1500.avi'
cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print("Error opening video stream or file")
    exit()


ret, frame1 = cap.read()
if not ret:
    print("No se pudo leer el primer cuadro")
    exit()

cv2.namedWindow('Deteccion', cv2.WINDOW_NORMAL)
q
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Convertir el segundo cuadro a escala de grises
    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)


    # Aplicar un umbral para resaltar las áreas con brillo
    _, thresh = cv2.threshold(frame_gray, 50, 255, cv2.THRESH_BINARY)
    cv2.imshow('Threshold', thresh)
    # Obtener las coordenadas de todos los puntos blancos
    white_points = cv2.findNonZero(thresh)

    # Dilatar la imagen para llenar los huecos
    thresh = cv2.dilate(thresh, None, iterations=10)
    cv2.imshow('Threshold Dilatado', thresh)


    # Encuentra los contornos
    contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)


    # Inicializar un cuadro para dibujar los contornos
    frame_with_contours = frame.copy()


    # Dibujar los contornos en el cuadro original
    for contour in contours:
        (x, y, w, h) = cv2.boundingRect(contour)
        print(f"Cordenadas: ({(2*x+w)/2}, {(2*y+h)/2})")
        cv2.rectangle(frame_with_contours, (x, y), (x + w, y + h), (0, 255, 0), 2)  # Marco verde


    # Mostrar el cuadro con los contornos
    cv2.imshow('Deteccion', frame_with_contours)


    # Esperar a que el usuario presione una tecla
    key = cv2.waitKey(0)
    if key == ord('q'):
        break
    elif key == ord('n'):
        continue 


cap.release()
cv2.destroyAllWindows()

