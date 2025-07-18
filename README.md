Projeto de Monitoramento Climático com ESP32 Doit Devkit Esp32-wroom 32 Modulo Microcontrolador em Alagoas

Frequancia: 240 MHz
Memória Flash: 4 MB
30 pinos
Alimentação 4,5V a 9V
Marca Blutu

📖 Sobre o Projeto
Este repositório contém o código-fonte e a documentação de um sistema de monitoramento de temperatura e umidade distribuído. O projeto utiliza três microcontroladores ESP32 DOIT DEVKIT V1 equipados com sensores DHT22 para coletar dados climáticos em tempo real de diferentes localidades no estado de Alagoas.

O principal objetivo é criar uma rede de baixo custo para a coleta de dados ambientais, que podem ser utilizados para análise meteorológica, projetos acadêmicos, automação na agricultura ou simples consulta pública.

📍 Locais de Monitoramento
As estações de coleta de dados foram estrategicamente instaladas nas seguintes cidades:

Belo Monte, AL

Arapiraca, AL

União dos Palmares, AL

🛠️ Hardware Utilizado
Cada uma das três unidades de monitoramento é composta pelos seguintes componentes:
Componente: ESP32 DOIT DEVKIT V1 (SKU 196)
Sensor de Temperatura e Umidade DHT22 
Resistor de 10kΩ: Resistor de pull-up para garantir a estabilidade na comunicação com o DHT22.
Protoboard e Jumpers
Fonte de Alimentação 3.3V

🔌 Diagrama de Conexão
A conexão entre o ESP32 e o sensor DHT22 é realizada da seguinte forma:

graph TD
    subgraph "Conexões"
        A[ESP32 DEVKIT V1] -- 3V3 --> B(VCC do DHT22);
        A -- GND --> C(GND do DHT22);
        A -- GPIO 4 --> D(DATA do DHT22);
        B -- Resistor 10kΩ --> D;
    end

🔧 Software e Dependências
O firmware do ESP32 foi desenvolvido utilizando a IDE do Arduino com o suporte ao Arduino Core for the ESP32.

Bibliotecas Necessárias
Para compilar o código, é preciso instalar as seguintes bibliotecas através do Gerenciador de Bibliotecas da IDE do Arduino:

DHT sensor library by Adafruit

Adafruit Unified Sensor by Adafruit

WiFi.h (geralmente já incluída no Core do ESP32)

(Adicione outras bibliotecas se usar, como PubSubClient.h para MQTT)

🚀 Como Utilizar
Siga os passos abaixo para configurar e rodar o projeto em um novo ESP32.

Clonar o Repositório:

git clone https://github.com/seu-usuario/seu-repositorio.git

Configurar o Ambiente:

Abra o projeto na IDE do Arduino.

Instale as bibliotecas listadas na seção anterior.

Personalizar o Código:

Abra o arquivo principal .ino.

Insira as credenciais da sua rede Wi-Fi nas constantes ssid e password.

Se estiver enviando os dados para uma plataforma de IoT (ThingSpeak, Tago.io, etc.), configure os tokens e chaves de API correspondentes.

Compilar e Fazer Upload:

Conecte o seu ESP32 DOIT DEVKIT V1 ao computador.

Na IDE, vá em Ferramentas > Placa e selecione "DOIT ESP32 DEVKIT V1".

Selecione a porta COM correta em Ferramentas > Porta.

Clique no botão de Upload.

Verificar o Funcionamento:

Após o upload, abra o Monitor Serial (Ctrl+Shift+M) com a taxa de 115200 bauds para visualizar as leituras do sensor e o status da conexão.

📈 Status e Próximos Passos
O projeto encontra-se em fase de implementação. As três estações estão operacionais e coletando dados.

Melhorias Futuras
[ ] Desenvolver um dashboard web para visualização dos dados em tempo real.

[ ] Implementar armazenamento dos dados em um banco de dados na nuvem (ex: Firebase, InfluxDB).

[ ] Criar um sistema de alertas por e-mail ou Telegram para variações climáticas extremas.

[ ] Projetar e imprimir um case em 3D para proteger os componentes em ambiente externo.

[ ] Adicionar um modo de deep sleep para economizar energia.

🤝 Contribuições
Contribuições são muito bem-vindas! Se você tem alguma ideia para melhorar este projeto, sinta-se à vontade para abrir uma issue para discussão ou enviar um pull request com suas modificações.

📄 Licença
Este projeto está sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.
