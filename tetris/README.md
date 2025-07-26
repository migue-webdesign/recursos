## 🎮 Proyecto: Tetris en ESP32 con pantalla OLED

Este proyecto recrea el clásico Tetris en una placa ESP32 usando una pantalla OLED SSD1306, botones físicos y un buzzer.

![Demo del Tetris](tetris_Esp32.png)

🔗 Código fuente: [tetris_esp32.ino](TETRIS_ESP32-DevModule.ino)
-----------------------
TABLA DE CONEXIONES

Componente          ESP32 GPIO           Color sugerido        Comentario
Pantalla SDA        21                   Verde                 Conecta a SDA del OLED
Pantalla SCL        22                   Azul                  Conecta a SCL del OLED
Pantalla VCC        3.3V                 Rojo                  Alimentación
Pantalla GND        GND                  Negro                 Tierra
Botón Izquierda     14                   Azul                  INPUT_PULLUP
Botón Derecha       27                   Verde                 INPUT_PULLUP
Botón Rotar         26                   Amarillo              INPUT_PULLUP
Botón Abajo         25                   Blanco                INPUT_PULLUP
Buzzer (+)          4                    Rojo                  Señal con tone()
Buzzer (–)          GND                  Negro                 Tierra

| Componente       | ESP32 GPIO | Color sugerido | Comentario             |
|------------------|------------|----------------|------------------------|
| Pantalla SDA     | 21         | Verde          | Conecta a SDA del OLED |
| Pantalla SCL     | 22         | Azul           | Conecta a SCL del OLED |
| Pantalla VCC     | 3.3V       | Rojo           | Alimentación           |
| Pantalla GND     | GND        | Negro          | Tierra                 |
| Botón Izquierda  | 14         | Azul           | INPUT_PULLUP           |
| Botón Derecha    | 27         | Verde          | INPUT_PULLUP           |
| Botón Rotar      | 26         | Amarillo       | INPUT_PULLUP           |
| Botón Abajo      | 25         | Blanco         | INPUT_PULLUP           |
| Buzzer (+)       | 4          | Rojo           | Señal con tone()       |
| Buzzer (–)       | GND        | Negro          | Tierra                 |
