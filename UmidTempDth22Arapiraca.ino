#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
// Inclusão da biblioteca para o Watchdog Timer (WDT)
#include <esp_task_wdt.h>

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
const int DHT_PIN = 4;
#define DHT_TYPE DHT22

// Intervalo de 40 minutos em milissegundos
const unsigned long INTERVALO_LOOP = 40 * 60 * 1000;

// Tempo limite do Watchdog em segundos. Deve ser maior que o tempo total do loop.
// O tempo do loop será o INTERVALO_LOOP + tempo das requisições e delays.
// Um valor de 2460 segundos (41 minutos) é seguro.
#define WDT_TIMEOUT 2460

DHT dht(DHT_PIN, DHT_TYPE);
// ====================================================================
// FUNÇÕES DE COMUNICAÇÃO COM A PLANILHA (sem alterações)
// ====================================================================

String fazerRequisicao(const String& jsonPayload) {
  if (WiFi.status() != WL_CONNECTED) {
    return "ERRO: Sem conexão WiFi";
  }
  HTTPClient http;
  String response = "";
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(googleScriptURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonPayload.c_str());
  if (httpResponseCode > 0) {
    response = http.getString();
    Serial.printf("[HTTP] Código Final: %d\n", httpResponseCode);
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
  char buffer[10];
  for (int i = 0; i < numDados; i++) {
    dtostrf(dados[i], 4, 2, buffer);
    jsonDados.add(String(buffer));
  }
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  String response = fazerRequisicao(jsonString);
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
    return !response.startsWith("ERRO");
}

void montarCabecalho(const String& boardID, const String& colunaInicial, const std::vector<String>& cabecalhos) {
    String celulaVerificacao = lerCelula(boardID, colunaInicial + "1");
    if (celulaVerificacao != cabecalhos[0]) {
        Serial.println("Cabeçalho não encontrado. Configurando a planilha...");
        char coluna = colunaInicial[0];
        for (size_t i = 0; i < cabecalhos.size(); i++) {
            String celulaAlvo = String(coluna) + "1";
            escreverEmCelula(boardID, celulaAlvo, cabecalhos[i]);
            coluna++;
        }
    } else {
        Serial.println("Cabeçalho da planilha já está correto.");
    }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("\n--- INICIANDO SETUP ---");
  
  // --- CONFIGURAÇÃO DO WATCHDOG ---
  Serial.println("Configurando o Watchdog Timer...");
  // Habilita o watchdog com o tempo limite definido
  esp_task_wdt_init(WDT_TIMEOUT, true); 
  // Adiciona a tarefa atual (loop) ao watchdog para monitoramento
  esp_task_wdt_add(NULL); 
  
  pinMode(RELE_PIN, OUTPUT); [cite: 20]
  pinMode(LED_PIN, OUTPUT); [cite: 3]
  pinMode(BOTAO_PIN, INPUT_PULLUP);
  digitalWrite(RELE_PIN, LOW); [cite: 20]
  digitalWrite(LED_PIN, LOW); [cite: 20]
  dht.begin();

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password); [cite: 2]
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Conectado!");
  montarCabecalho("Arapiraca", "A", {"Data completa", "Data", "Hora", "Umidade", "Temperatura", "Botao", "Rele_Planilha", "Led_Planilha", "Reset"});
  Serial.println("--- SETUP CONCLUÍDO ---");
}

// --- LOOP ---
void loop() {
  Serial.println("\nLOOP: Início da execução.");
  
  // --- ALIMENTAÇÃO DO WATCHDOG ---
  // Reseta o temporizador do watchdog no início de cada loop para evitar o reinício.
  esp_task_wdt_reset();
  
  bool statusBotao = (digitalRead(BOTAO_PIN) == LOW);
  float umidade = dht.readHumidity(); [cite: 23]
  float temperatura = dht.readTemperature(); [cite: 23]

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("LOOP: Falha ao ler dados do sensor DHT22!"); [cite: 24]
  } else {
    Serial.print("LOOP: Leitura -> ");
    Serial.print(umidade);
    Serial.print("% Umidade, ");
    Serial.print(temperatura);
    Serial.println("°C");
    escreverEmLista("Arapiraca", 3, new float[3]{umidade, temperatura, (float)statusBotao}); [cite: 25]
  }
  
  delay(2000); 

  String valorRelePlanilha = lerCelula("Arapiraca", "G2");
  if (valorRelePlanilha == "1") { [cite: 26]
    digitalWrite(RELE_PIN, HIGH); [cite: 27]
  } else if (valorRelePlanilha == "0") { [cite: 27]
    digitalWrite(RELE_PIN, LOW); [cite: 27]
  }

  delay(50); 

  String valorLedPlanilha = lerCelula("Arapiraca", "H2");
  if (valorLedPlanilha == "1") { [cite: 28]
    digitalWrite(LED_PIN, HIGH); [cite: 29]
  } else if (valorLedPlanilha == "0") { [cite: 29]
    digitalWrite(LED_PIN, LOW); [cite: 30]
  }

  // ==================================================================
  // BLOCO DE CÓDIGO PARA VERIFICAR E EXECUTAR O RESET
  // ==================================================================
  delay(50);
  Serial.println("LOOP: Verificando comando de reset..."); [cite: 31]
  String valorReset = lerCelula("Arapiraca", "I2");
  valorReset.trim();
  // Verifica se o comando de reset foi recebido
  if (valorReset == "1") { [cite: 32]
    Serial.println("COMANDO DE RESET (1) RECEBIDO!");
    // Zera o valor na planilha para evitar loop de boot
    Serial.println("Escrevendo '0' na célula de reset para evitar novo boot...");
    escreverEmCelula("Arapiraca", "I2", "0"); [cite: 34]
    
    Serial.println("Reiniciando o ESP32 em 2 segundos...");
    delay(2000);
    // Atraso para garantir que a escrita na planilha seja concluída
    
    ESP.restart(); [cite: 36]
    // Comando para reiniciar o ESP32
  }
  // ==================================================================

  Serial.println("LOOP: Fim da execução, aguardando proximo ciclo.");
  Serial.println("--------------------------------");
  delay(INTERVALO_LOOP); [cite: 37]
}
