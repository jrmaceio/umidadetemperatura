#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>

// Inclusão da biblioteca para o sensor DHT
#include "DHT.h"

// --- CONFIGURAÇÕES ---
const char* ssid = "CLARO_2GC3B8C0";
const char* password = "12C3B8C0";
const char* googleScriptURL = "https://script.google.com/macros/s/AKfycbyh71u__MFmmi2uaEhTl_fhvzuthZxjD90fjT4gchU2YOwsSfmt-akRIiKi3WakQelH/exec";

// --- PINOS ---
const int RELE_PIN = 25;
const int LED_PIN = 26;
const int BOTAO_PIN = 27;

// Definições para o sensor DHT
const int DHT_PIN = 4;      // Pino onde o sensor está conectado (D4)
// ***** ALTERAÇÃO REALIZADA AQUI *****
#define DHT_TYPE DHT22      // Define o tipo do sensor como DHT22. (Anteriormente DHT11)

//const unsigned long INTERVALO_LOOP = 1800000; // Intervalo entre as leituras (30 minutos)
const unsigned long INTERVALO_LOOP = 5 * 60 * 1000; // 5 minutos * 60 segundos * 1000 milissegundos

// Cria o objeto 'dht' para nos comunicarmos com o sensor
DHT dht(DHT_PIN, DHT_TYPE);

// ====================================================================
// VERSÃO FINAL E CORRETA DA FUNÇÃO
// ====================================================================

String fazerRequisicao(const String& jsonPayload) {
  if (WiFi.status() != WL_CONNECTED) {
    return "ERRO: Sem conexão WiFi";
  }

  HTTPClient http;
  String response = "";

  // Usamos o redirecionamento automático da biblioteca, que agora funcionará
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(googleScriptURL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonPayload.c_str());

  if (httpResponseCode > 0) {
    // A biblioteca já seguiu o redirecionamento, então a resposta é a final
    response = http.getString();
    Serial.printf("[HTTP] Código Final: %d\n", httpResponseCode);
    Serial.println("Resposta do Servidor: " + response);

  } else {
    response = "ERRO: Falha na requisição. " + String(http.errorToString(httpResponseCode).c_str());
  }
  
  http.end();
  return response;
}


bool escreverEmLista(const String& identificacao, int numDados, float dados[]) {
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["action"] = "escreverEmLista";
  jsonDoc["identificacao"] = identificacao;
  JsonArray jsonDados = jsonDoc.createNestedArray("dados");

  // Converte os números para texto para evitar erros de formatação
  char buffer[10];
  for (int i = 0; i < numDados; i++) {
    dtostrf(dados[i], 4, 2, buffer);
    jsonDados.add(String(buffer));
  }

  String jsonString;
  serializeJson(jsonDoc, jsonString);
  
  String response = fazerRequisicao(jsonString);
  Serial.println("Resposta do 'escreverEmLista': " + response);
  return !response.startsWith("ERRO");
}

String lerCelula(const String& identificacao, const String& celula) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["action"] = "lerCelula";
  jsonDoc["identificacao"] = identificacao;
  jsonDoc["celula"] = celula;
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return fazerRequisicao(jsonString);
}

bool escreverEmCelula(const String& identificacao, const String& celula, const String& dado) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["action"] = "escreverEmCelula";
    jsonDoc["identificacao"] = identificacao;
    jsonDoc["celula"] = celula;
    jsonDoc["dado"] = dado;
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    String response = fazerRequisicao(jsonString);
    Serial.println("Resposta do 'escreverEmCelula': " + response);
    return !response.startsWith("ERRO");
}

void montarCabecalho(const String& boardID, const String& colunaInicial, const std::vector<String>& cabecalhos) {
    String celulaVerificacao = lerCelula(boardID, colunaInicial + "1");
    if (celulaVerificacao != cabecalhos[0]) {
        Serial.println("Cabeçalho não encontrado ou diferente. Configurando a planilha...");
        char coluna = colunaInicial[0];
        for (size_t i = 0; i < cabecalhos.size(); i++) {
            String celulaAlvo = String(coluna) + "1";
            int tentativas = 0;
            while (!escreverEmCelula(boardID, celulaAlvo, cabecalhos[i]) && tentativas < 3) {
                Serial.println("Falha ao escrever cabeçalho. Tentando novamente...");
                delay(1000);
                tentativas++;
            }
            coluna++;
        }
        Serial.println("Configuração do cabeçalho finalizada.");
    } else {
        Serial.println("Cabeçalho da planilha já está correto.");
    }
}


// --- SETUP ---
void setup() {
  Serial.begin(115200);

  //debug
  //Serial.println(googleScriptURL); 

  delay(1000); 
  Serial.println("\n\n--- INICIANDO SETUP ---");
  
  pinMode(RELE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP); 

  digitalWrite(RELE_PIN, LOW); 
  digitalWrite(LED_PIN, LOW);  

  // Inicia o sensor DHT
  dht.begin();
  Serial.println("SETUP: Sensor DHT22 iniciado.");

  Serial.println("SETUP: Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  int tentativasWifi = 0;
  while (WiFi.status() != WL_CONNECTED && tentativasWifi < 20) {
    delay(500);
    Serial.print(".");
    tentativasWifi++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSETUP: WiFi Conectado!");
    Serial.println("SETUP: Antes de montarCabecalho...");
    
    montarCabecalho("Arapiraca", "A", {"Data completa", "Data", "Hora", "Umidade", "Temperatura", "Botao", "Rele_Planilha", "Led_Planilha"});
    
    Serial.println("SETUP: Depois de montarCabecalho.");
    Serial.println("--- SETUP CONCLUído, ENTRANDO NO LOOP ---");
  } else {
    Serial.println("\nFalha ao conectar no WiFi. Reiniciando...");
    delay(10000);
    ESP.restart();
  }
}

// --- LOOP ---
void loop() {
  Serial.println("\nLOOP: Início da execução.");
  bool statusBotao = (digitalRead(BOTAO_PIN) == LOW);

  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("LOOP: Falha ao ler dados do sensor DHT22!");
  } else {
    Serial.print("LOOP: Leitura do Sensor -> ");
    Serial.print(umidade);
    Serial.print("% Umidade, ");
    Serial.print(temperatura);
    Serial.println("°C");
    escreverEmLista("Arapiraca", 3, new float[3]{umidade, temperatura, (float)statusBotao});
  }
  
  // Pausa para o sensor e para espaçar as requisições
  delay(2000); 

  String valorRelePlanilha = lerCelula("Arapiraca", "G2");
  if (valorRelePlanilha == "1") {
    digitalWrite(RELE_PIN, HIGH);
  } else if (valorRelePlanilha == "0") {
    digitalWrite(RELE_PIN, LOW);
  }

  // Pequena pausa entre as chamadas de leitura
  delay(50); 

  String valorLedPlanilha = lerCelula("Arapiraca", "H2");
  if (valorLedPlanilha == "1") {
    digitalWrite(LED_PIN, HIGH);
  } else if (valorLedPlanilha == "0") {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println("LOOP: Fim da execução, aguardando proximo ciclo.");
  Serial.println("--------------------------------");
  delay(INTERVALO_LOOP); // O intervalo longo principal
}
