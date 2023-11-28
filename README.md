## Repositório dedicado à Avaliação Baseada em Problema da disciplina de IOT APLICADA do curso de Engenharia da Computação 2023-2

### Descrição do problema: Um cofre que possui 3 mecanismos para abertura da porta, sendo eles: inserir a senha numérica, leitura do RFID e leitura da biometria. Após o cadastro dessas credenciais, ao ser necessário abrir o cofre, precisará informar essas credenciais corretamente. 

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

### Front-End:
- Linguagem Typescript
- React
- Framework do React: NextJs
- Framework de Estilização: TailwindCSS
- Biblioteca de componentes: Material Tailwind

Link para acesso: https://abp-iot-aplicada-geral.vercel.app/
