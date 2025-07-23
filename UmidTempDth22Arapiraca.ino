Esse erro de compilação acontece porque a função `esp_task_wdt_init` foi atualizada em versões mais recentes do framework do ESP32. Em vez de aceitar o tempo e um booleano como parâmetros diretos, ela agora espera um ponteiro para uma estrutura de configuração.

Para corrigir, você precisa criar uma estrutura de configuração (`esp_task_wdt_config_t`), definir os parâmetros dentro dela e então passá-la para a função de inicialização.

-----

### Código Corrigido

Abaixo está o seu código anterior com a correção aplicada na função `setup()`. A lógica do watchdog e os outros recursos que você solicitou permanecem os mesmos.

```cpp
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

// Tempo limite do Watchdog em segundos.
#define WDT_TIMEOUT_S 2460 // 41 minutos em segundos

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
  
  // --- CORREÇÃO NA CONFIGURAÇÃO DO WATCHDOG ---
  Serial.println("Configurando o Watchdog Timer...");
  // 1. Cria a estrutura de configuração
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = WDT_TIMEOUT_S * 1000, // Define o timeout em milissegundos
      .idle_core_mask = (1 << 0) | (1 << 1), // Monitora os cores 0 e 1 no estado ocioso
      .trigger_panic = true, // Habilita a reinicialização por pânico
  };
  // 2. Inicializa o WDT com um ponteiro para a configuração
  esp_task_wdt_init(&wdt_config);
  
  // Adiciona a tarefa atual (loop) ao watchdog para monitoramento
  esp_task_wdt_add(NULL); 
  
  pinMode(RELE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP);
  digitalWrite(RELE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  dht.begin();

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Conectado!");
  montarCabecalho("Arapiraca", "A", {"Data completa", "Data", "Hora", "Umidade", "Temperatura", "Botao", "Rele_Planilha", "Led_Planilha", "Reset"});
  Serial.println("--- SETUP CONCLUÍDO ---");
  
  // Acende o LED para indicar que o dispositivo está ligado e operacional.
  digitalWrite(LED_PIN, HIGH);
}

// --- LOOP ---
void loop() {
  Serial.println("\nLOOP: Início da execução.");
  
  // --- ALIMENTAÇÃO DO WATCHDOG ---
  // Reseta o temporizador do watchdog no início de cada loop para evitar o reinício.
  esp_task_wdt_reset();
  
  bool statusBotao = (digitalRead(BOTAO_PIN) == LOW);
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("LOOP: Falha ao ler dados do sensor DHT22!");
  } else {
    Serial.print("LOOP: Leitura -> ");
    Serial.print(umidade);
    Serial.print("% Umidade, ");
    Serial.print(temperatura);
    Serial.println("°C");
    escreverEmLista("Arapiraca", 3, new float[3]{umidade, temperatura, (float)statusBotao});
  }
  
  delay(2000); 

  String valorRelePlanilha = lerCelula("Arapiraca", "G2");
  if (valorRelePlanilha == "1") {
    digitalWrite(RELE_PIN, HIGH);
  } else if (valorRelePlanilha == "0") {
    digitalWrite(RELE_PIN, LOW);
  }

  delay(50); 

  String valorLedPlanilha = lerCelula("Arapiraca", "H2");
  if (valorLedPlanilha == "1") {
    digitalWrite(LED_PIN, HIGH);
  } else if (valorLedPlanilha == "0") {
    digitalWrite(LED_PIN, LOW);
  }

  // ==================================================================
  // BLOCO DE CÓDIGO PARA VERIFICAR E EXECUTAR O RESET
  // ==================================================================
  delay(50);
  Serial.println("LOOP: Verificando comando de reset...");
  String valorReset = lerCelula("Arapiraca", "I2");
  valorReset.trim();
  // Verifica se o comando de reset foi recebido
  if (valorReset == "1") {
    Serial.println("COMANDO DE RESET (1) RECEBIDO!");
    // Zera o valor na planilha para evitar loop de boot
    Serial.println("Escrevendo '0' na célula de reset para evitar novo boot...");
    escreverEmCelula("Arapiraca", "I2", "0");
    
    Serial.println("Reiniciando o ESP32 em 2 segundos...");
    delay(2000);
    // Atraso para garantir que a escrita na planilha seja concluída
    
    ESP.restart();
    // Comando para reiniciar o ESP32
  }
  // ==================================================================

  Serial.println("LOOP: Fim da execução, aguardando proximo ciclo.");
  Serial.println("--------------------------------");
  delay(INTERVALO_LOOP);
}
```
