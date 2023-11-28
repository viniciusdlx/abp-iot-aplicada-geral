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
- **Botão** *(para indicar que o cofre foi fechado e as travas serão fechadas)*
- **Servo Motor**

### Front-End:
- Linguagem Typescript
- React
- Framework do React: NextJs
- Framework de Estilização: TailwindCSS
- Biblioteca de componentes: Material Tailwind

Link para acesso: https://abp-iot-aplicada-geral.vercel.app/
