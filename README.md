## Repositório dedicado à Avaliação Baseada em Problema da disciplina de IOT APLICADA do curso de Engenharia da Computação 2023-2

### Descrição do problema: 
- O projeto envolve a criação de um cofre inovador, dotado de três métodos distintos para abrir a porta. Os mecanismos incluem inserção de senha numérica, leitura de cartão RFID e reconhecimento biométrico. Para garantir a segurança do cofre, é essencial realizar o cadastro prévio dessas credenciais. Durante o acesso subsequente, o usuário deverá fornecer as informações corretas correspondentes a esses três métodos de autenticação para desbloquear com sucesso o cofre. Essa abordagem multifatorial visa oferecer uma camada adicional de segurança e flexibilidade no controle de acesso ao cofre, tornando-o ideal para ambientes que demandam alta segurança e praticidade no gerenciamento de credenciais.

### Dispositivos IoT utilizados:
- **3 ESP32 Oled**
  - 3 necessários para abrir as travas
  - 1 para a conexão geral
- **3 Leds RGB**
  -   azul: **cadastramento**,
  -   verde: **trava aberta**
  -   vermelho: **trava fechada***
- **Sensor Biométrico R502**
- **Teclado Membrana Matricial 4×4 16 Teclas**
- **Leitor RFID**
- **4 Servo Motor 9G SG90**
- **Botão** *(para indicar que o cofre foi fechado e as travas serão fechadas)*

### Descrição dos sensores, atuadores e suas funções:
 
-  **Sensores**:
    -  **Sensor Biométrico R502:**
        - *Função:* Este sensor é utilizado para autenticação biométrica, como leitura de impressões digitais.
    Ele captura e processa as características únicas de uma impressão digital para fins de identificação ou autenticação.
    
    -  **Leitor RFID:**
        - *Função:* O Leitor RFID (Radio-Frequency Identification) é usado para identificar e rastrear tags RFID. 
    Essas tags contêm informações que podem ser lidas pelo leitor.

- **Atuadores**:
  -  **ESP32 OLED:**
     - *Função:* Enquanto o ESP32 é principalmente um microcontrolador com capacidades de conectividade Wi-Fi e Bluetooth, a tela OLED é um tipo de display. 
      O ESP32 OLED geralmente combina essas funcionalidades, permitindo a exibição de informações em tempo real, como dados de sensores, status de conexão, etc.

  -  **LEDs RGB:**
     - *Função:* Os LEDs RGB, são usados para fornecer feedback visual. 
      Eles podem exibir uma variedade de cores, controladas individualmente ou em conjunto, proporcionando uma forma versátil de indicar estados, alertas ou simplesmente adicionar elementos visuais ao dispositivo.

  -  **Servo Motor 9G SG90:**
     - *Função:* O servo motor SG90 é um atuador que pode ser controlado para girar em uma posição específica. 
      Ele é comumente usado em projetos onde é necessário movimento preciso, como em aplicações de robótica, controle de câmera, ou mecanismos de travamento.

  -  **Teclado Membrana Matricial 4×4 16 Teclas:**
     - *Função:* O teclado de membrana matricial é um conjunto de botões organizados em uma matriz. 
    Ele é utilizado para entrada de dados e interação com o dispositivo.

  -  **Botão:**
     - *Função:* O botão é um componente de entrada simples, utilizado para acionar ações quando pressionado. 
      Pode ser usado para iniciar ou interromper processos, alterar modos de operação, entre outras funções, dependendo da programação do dispositivo.

### Front-End:
- Linguagem Typescript
- Biblioteca React
- Framework do React: NextJs
- Framework de Estilização: TailwindCSS
- Biblioteca de componentes: Material Tailwind

Link para acesso: https://bank-security-system.vercel.app
