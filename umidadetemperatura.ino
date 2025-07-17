#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "CLARO_2GC3B8C0";
const char* password = "12C3B8C0";
const char* googleScriptURL = "https://script.google.com/macros/s/AKfycby6ED8BtXkyP03zw0nKYjEmIMdUQJquTJCv1DbZHg5gVLTlxXgewGTc7LhnXjQle4ikVg/exec";

#define rele    10
#define led     8
#define botao   9

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  pinMode(rele, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(botao, INPUT);

  // Página planilha, 1a Coluna, Vetor com a identificação de cada coluna(data completa, somente a data, somente a hora, dado1, dado2, estado de um botão na placa, dado 1 para ler na plcanila, dado 2 para ler na plcanila 
  //montarCabecalho("Arapiraca", "A", {"Data completa", "Data", "Hora", "Dado 1", "Variável", "Botão","Relé","Led"});
  montarCabecalho("Arapiraca", "A", {"Data completa", "Data", "Hora", "Umidade", "Temperatura", "Botão","Relé","Led"});
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    bool botao_status = digitalRead(botao);
    int variavel = 100;

    // pagina da planilha, indicativo que vou salvar 3 dados e o vetor com os dados
    escreverEmLista("Arapiraca", 3, new float[] {2.4, variavel, botao_status});

    // pagina da planilha, ler a celula g2
    String valorCelula = lerCelula("Arapiraca", "G2");
    Serial.println("Valor do Relé: " + valorCelula);
    digitalWrite(rele, valorCelula.toInt());

    // pagina da planilha, , ler a celula h2
    valorCelula = lerCelula("Arapiraca", "H2");
    Serial.println("Valor do Led: " + valorCelula);
    digitalWrite(led, !valorCelula.toInt());

  }
}
