### Controle de LEDs e Display com Joystick no Raspberry Pi Pico

Este projeto tem como objetivo consolidar o uso de conversores analógico-digitais (ADC) no RP2040 e explorar as funcionalidades da placa BitDogLab. A aplicação principal envolve o controle de LEDs RGB e a exibição de um quadrado móvel no display SSD1306, utilizando um joystick analógico.

🎯 Objetivos

- Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
- Utilizar PWM para ajustar a intensidade de LEDs RGB com base no joystick.
- Representar a posição do joystick no display SSD1306.
- Aplicar comunicação I2C para interação com o display.
- Implementar interrupções (IRQ) e debouncing para os botões.

🛠️ Componentes Utilizados

- Raspberry Pi Pico (RP2040)
- Placa BitDogLab
- Joystick analógico (GPIOs 26 e 27)
- Botão do Joystick (GPIO 22)
- Botão A (GPIO 5)
- LED RGB (GPIOs 11, 12 e 13)
- Display OLED SSD1306 (I2C - GPIOs 14 e 15)

📌 Funcionalidades Implementadas

- Controle de LEDs via PWM
    - O LED Azul tem sua intensidade ajustada com base no eixo Y do joystick.
    - O LED Vermelho é controlado pelo eixo X do joystick.
    - Ambos variam de intensidade suavemente conforme o joystick é movido.

- Exibição Gráfica no Display
    - O display OLED exibe um quadrado 8x8 que se move conforme os valores do joystick.
    - O botão do joystick altera o estilo da borda do display.

- Interação com Botões
    - O botão do joystick alterna o LED Verde e modifica a borda do display.
    - O botão A ativa/desativa o controle PWM dos LEDs RGB.

- Modo BOOTSEL com Botão B
    - O botão B pode ser usado para reiniciar no modo BOOTSEL.

📌 Requisitos

- Implementação de interrupções (IRQ) para os botões.
- Debouncing via software para evitar leituras erradas.
- Código bem estruturado e comentado para facilitar manutenção.
- Utilização do protocolo I2C para comunicação com o display.

🚀 Como Compilar e Executar

- Configure o Pico SDK no ambiente de desenvolvimento.
- Clone o repositório e entre no diretório:

        git clone <seu-repo>
        cd <seu-repo>

- Crie a pasta build e compile o projeto:

        mkdir build && cd build
        cmake ..
        make

Conecte o Raspberry Pi Pico no modo BOOTSEL e envie o arquivo .uf2 gerado:

    cp U4C8O12T_WLS.uf2 /media/<seu_usuario>/RPI-RP2/

📽️ Demonstração

Um vídeo demonstrando o funcionamento pode ser acessado aqui.
https://youtu.be/PN6iLa0_h0Y

📜 Licença

Este projeto está licenciado sob a MIT License.