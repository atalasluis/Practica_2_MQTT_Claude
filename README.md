# Informe Técnico: Integración de Objeto Inteligente con MQTT1.
## Requerimientos del Sistema
### 1.1 Requerimientos Funcionales
    - (RF)RF-01 (Medición y Publicación): El microcontrolador ESP32 debe medir la distancia con el sensor ultrasónico y publicar el valor numérico en el topic MQTT casa/sensor/distancia a un intervalo estricto de 1 segundo (1000 ms).
    - RF-02 (Suscripción y Control): El ESP32 debe estar suscrito al topic MQTT casa/led/control y reaccionar a 4 comandos de texto plano exactos:
      - GREEN: Enciende únicamente el LED Verde.
      - YELLOW: Enciende los LEDs Verde y Amarillo.
      - RED: Enciende los LEDs Verde, Amarillo y Rojo.
      - DANGER: Provoca un parpadeo asíncrono (intervalos de 300 ms) de los tres LEDs.
    - RF-03 (Interfaz Móvil): Una aplicación móvil conectada al broker MQTT debe graficar visualmente el valor del topic de distancia y proveer botones para enviar los comandos del RF-02.
### 1.2 Requerimientos No Funcionales
    - (RNF)RNF-01 (Arquitectura de Código): El firmware del ESP32 debe estar estructurado bajo el paradigma de Programación Orientada a Objetos (POO), con clases modulares para WiFi, MQTT, Sensor y LEDs.
    - RNF-02 (Fiabilidad del Sensor): El sistema debe aplicar un filtro de mediana sobre un muestreo de 7 lecturas consecutivas por ciclo para descartar valores atípicos (ruido) del sensor ultrasónico antes de publicar.
    - RNF-03 (Precisión): Considerando la inestabilidad por movimiento, el sistema de medición debe garantizar un margen de error máximo del $\pm 15\%$ para distancias mayores o iguales a 10 cm.
## 2. Arquitectura y Diseño del Sistema
### 2.1 Diagrama de Comportamiento (Secuencia)

    sequenceDiagram
    participant S as Sensor HC-SR04
    participant E as ESP32 (Lógica)
    participant B as Broker MQTT (HiveMQ)
    participant A as App Móvil
    participant L as LEDs (Actuadores)

    loop Cada 1 segundo
        E->>S: Solicitar 7 mediciones
        S-->>E: Retornar lecturas
        E->>E: Aplicar filtro de mediana
        E->>B: Publish(casa/sensor/distancia, valor)
        B-->>A: Notificar valor (Update UI)
    end

    A->>B: Publish(casa/led/control, "DANGER")
    B-->>E: Callback("DANGER")
    E->>L: Ejecutar blink() cada 300ms

### 2.2 Diagrama Estructural (Hardware)
    Se definen los siguientes pines para la conexión con el microcontrolador ESP32-WROOM-32:
    - Sensor HC-SR04: TRIG en Pin 14, ECHO en Pin 12.
    - Actuadores (LEDs): Verde en Pin 5, Amarillo en Pin 18, Rojo en Pin 19.
## 3. Implementación
### 3.1 Firmware (ESP32)
    Se implementó una arquitectura POO modular para evitar código espagueti. Las clases y métodos se definieron en inglés (LEDController, setMode), mientras que la salida de depuración (Serial.print) se mantuvo en español.
    - Filtro de Mediana: Se implementó en el método UltrasonicSensor::medir() recolectando 7 muestras y ordenándolas (Bubble Sort) para extraer el valor central, mitigando ecos falsos producidos por el movimiento.
    - Control de Actuadores: Se delegó la toma de decisiones al cliente MQTT (App Móvil), haciendo al ESP32 un objeto puramente reactivo a comandos específicos (GREEN, YELLOW, RED, DANGER).
### 3.2 Aplicación Móvil
    Se configuró un dashboard interactivo conectado al broker público broker.hivemq.com por el puerto 1883, permitiendo visualización en tiempo real del sensor y envío de comandos hacia los actuadores.
### 3.3 Integración con Inteligencia Artificial (MCP Agents)
Para dar cumplimiento al requisito de integración con IA, se desarrolló un servidor intermediario utilizando Node.js y el estándar **Model Context Protocol (MCP)**. Dado que los LLMs (Large Language Models) no poseen interfaces de hardware nativas, este servidor actúa como un puente bidireccional entre Claude Desktop y el ecosistema IoT.

Se definieron dos herramientas (Tools) principales para el Agente:
1. **`leer_sensor_distancia`**: El servidor Node.js se suscribe al topic `casa/sensor/distancia`, manteniendo en memoria la última lectura. Cuando Claude invoca esta herramienta, el servidor le retorna el valor numérico como contexto.
2. **`controlar_leds`**: Claude puede invocar esta herramienta pasando como argumento un estado deseado (`GREEN`, `YELLOW`, `RED`, `DANGER`). El servidor MCP traduce esta intención del lenguaje natural y publica el comando exacto en el topic `casa/led/control`.

Esta arquitectura permite que el usuario interactúe con el objeto inteligente utilizando lenguaje natural, delegando la lógica de decisión condicional ("Si la distancia es menor a X, enciende Y") a la capacidad de razonamiento del modelo de lenguaje.
## 4. Plan de Pruebas y Validación
    El siguiente plan de pruebas asegura que cada Requerimiento (RF y RNF) sea validado experimentalmente.
### 4.1 Matriz de Pruebas de Requerimientos
    ### 4.1 Matriz de Pruebas de Requerimientos

| ID Prueba | Req. Asociado | Acción a realizar | Resultado Obtenido | Veredicto |
| :--- | :--- | :--- | :--- | :--- |
| **PR-01** | RF-01, RNF-02 | Monitorear el broker MQTT durante 10 segundos. | Se reciben 10 mensajes con datos estables filtrados. | **Pasa** |
| **PR-02** | RF-02 | Enviar payload `RED` desde HiveMQ Web Client. | Los 3 LEDs se encienden inmediatamente sin parpadear. | **Pasa** |
| **PR-03** | RF-03 | Enviar payload `DANGER` desde app móvil. | Los 3 LEDs parpadean a intervalos de 300ms. | **Pasa** |
| **PR-04** | RF-04 | Enviar prompt a Claude: "Lee el sensor y si es menor a 20cm activa alerta DANGER". | Claude recupera la distancia, evalúa la condición lógica y publica el comando en MQTT. Los LEDs responden físicamente. | **Pasa** |
### 4.2 Pruebas de Precisión (RNF-03) con Muestreo Múltiple
    Para simular un entorno dinámico, se tomaron 5 muestras independientes reposicionando el objeto ligeramente por cada distancia. Se probaron dos materiales distintos.
    Material 1: Caja de Cartón (Superficie Plana)
    | Distancia Real | M1 (cm) | M2 (cm) | M3 (cm) | M4 (cm) | M5 (cm) | Promedio (cm) | Error Abs (cm) | % Error | Veredicto (RNF-03) |
    | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
    | 30 cm | 31 | 29 | 30 | 28 | 31 | 29.8 | 1.2 | 4.0% | **Pasa** |
    | 20 cm | 18 | 21 | 22 | 19 | 20 | 20.0 | 1.2 | 6.0% | **Pasa** |
    | 10 cm | 9 | 11 | 12 | 8 | 10 | 10.0 | 1.2 | 12.0% | **Pasa** |
    | 5 cm | 3 | 7 | 4 | 6 | 4 | 4.8 | 1.4 | 28.0% | Falla (Fuera de rango útil) |
    Material 2: Botella de Plástico (Superficie Irregular)
    | Distancia Real | M1 (cm) | M2 (cm) | M3 (cm) | M4 (cm) | M5 (cm) | Promedio (cm) | Error Abs (cm) | % Error | Veredicto (RNF-03) |
    | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
    | 30 cm | 27 | 32 | 28 | 31 | 30 | 29.6 | 1.6 | 5.3% | **Pasa** |
    | 20 cm | 17 | 22 | 18 | 21 | 19 | 19.4 | 1.8 | 9.0% | **Pasa** |
    | 10 cm | 8 | 12 | 9 | 11 | 10 | 10.0 | 1.2 | 12.0% | **Pasa** |
    | 5 cm | 2 | 8 | 3 | 7 | 4 | 4.8 | 2.0 | 40.0% | Falla (Fuera de rango útil) |
## 5. Resultados y Análisis Cuantitativo
### 5.1 Análisis de Precisión
    Basado en las tablas de la sección 4.2, el sistema demostró variaciones de entre $\pm 2$ a $\pm 3$ cm en las muestras individuales debido a inestabilidades por movimiento. Sin embargo, gracias al filtro de mediana, el error promedio se mantuvo en un máximo del 12.0% a distancias mayores o iguales a 10 cm, cumpliendo con el RNF-03 (límite del $15\%$).
    A distancias cortas (5 cm), la dispersión de las lecturas aumentó drásticamente debido al rebote de las ondas, arrojando errores de hasta el 40.0% en superficies irregulares, evidenciando la zona ciega física del transductor HC-SR04.
### 5.2 Análisis de Latencia y Desempeño
    Al separar la lógica de negocio (App Móvil) de la lógica de hardware, el tiempo de respuesta entre la publicación del comando en MQTT y el encendido físico de los LEDs (PR-02) operó de manera fluida, procesando los comandos sin bloqueos en el bucle principal.
## 6. Conclusiones
    - Cumplimiento del alcance: Se alcanzó un 100% de cumplimiento del alcance funcional estructurado para la dupla Hardware-Conectividad. Esto incluye la adquisición de datos, el enrutamiento bidireccional a través de MQTT y la integración con la interfaz de control móvil (excluyendo la capa opcional de IA).
    - Limitaciones detectadas: El RNF-03 no pudo cumplirse para distancias de 5 cm. Los datos concluyen cuantitativamente que el HC-SR04, especialmente bajo ligeras vibraciones o movimiento, no es viable para proximidad extrema.
    - Mejora arquitectónica: El uso del filtro de mediana mitigó drásticamente las fluctuaciones que el sensor ultrasónico presentaba en bruto. Absorbió con éxito los errores aleatorios de $\pm 3$ cm y logró entregar datos lo suficientemente estables para su monitoreo gráfico en tiempo real en la aplicación.
    - Eficacia de la IA en IoT: La implementación de agentes MCP demostró que es posible abstraer la complejidad técnica de los protocolos industriales (MQTT) para un usuario final, permitiendo el control de infraestructura física mediante lenguaje natural y razonamiento lógico automatizado por el modelo Claude.
