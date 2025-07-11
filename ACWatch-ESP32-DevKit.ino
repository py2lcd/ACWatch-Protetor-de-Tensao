/*
  ACWatch PLUS- Sketch com Web UI, RTC DS3231 e histórico de falhas
  By Billy Heinz Dorsch
  */

// =================================================================
// === BIBLIOTECAS: As "caixas de ferramentas" do projeto ========
// =================================================================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiScan.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <RTClib.h>
#include <SPIFFS.h>

// =================================================================
// === DEFINIÇÕES DO DISPLAY: Configurações da telinha OLED ========
// =================================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

// =================================================================
// === CONFIGURAÇÕES DE REDE: Dados do Wi-Fi =======================
// =================================================================
const char* ssidAP = "ACWatch_AP";
const char* passwordAP = "12345678";
WebServer server(80);
Preferences preferences;

// =================================================================
// === PINOS: Mapeamento das conexões físicas ======================
// =================================================================
const int pinSensor   = 34;
const int pinRele1    = 26;
const int pinRele2    = 27;
const int pinBotaoModo  = 25;
const int pinResetWiFi  = 15;

// =================================================================
// === VARIÁVEIS DE ESTADO: A "memória" do programa ===============
// =================================================================
bool modo220V = false;
bool estadoRele = false;
unsigned long tempoPressionado = 0;
unsigned long ultimoAtualizacao = 0;
unsigned long tempoUltimaTensaoSegura = 0;

// =================================================================
// === PARÂMETROS DE TENSÃO: Limites de operação ===================
// =================================================================
// Valores padrão que podem ser sobrescritos pelos ajustes na página web.
float limiteMin127 = 95.0;
float limiteMax127 = 135.0;
float limiteMin220 = 200.0;
float limiteMax220 = 265.0;

// Fatores de calibração (ainda fixos no código)
const float fatorCalibracao127 = 492.7;
const float fatorCalibracao220 = 546.9;


// =================================================================
// === DECLARAÇÃO DE FUNÇÕES: Uma "lista de tarefas" para o compilador ===
// =================================================================
void setup();
void loop();
void configurarWiFi();
void handleMainPage();
void handleGetStatus();
void handleSetTime();
void handleSetParams();
void handleAlternarModo();
void handleScanWiFi();
void handleConfigurarWiFi();
void handleFalhas();
void handleLimparFalhas();
float lerTensaoRMS();
void desligarRele();
void ligarRele();
void printCentralizado(String texto, int y, int tamanho);
void atualizarDisplay(float tensao, DateTime now);
void registrarFalha(float tensao);
String formatDigit(int digit);


// =================================================================
// === PÁGINA WEB PRINCIPAL (HTML, CSS, JAVASCRIPT) ================
// =================================================================
const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Painel ACWatch</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            box-sizing: border-box;
            background-color: #111827;
            color: #f9fafb;
        }
        .card {
            background-color: #1f2937;
            border-radius: 16px;
            padding: 32px;
            width: 100%;
            max-width: 400px;
            box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
            text-align: center;
        }
        h1 { font-size: 28px; font-weight: 700; margin: 0 0 8px 0; }
        .card > p { color: #9ca3af; margin: 0 0 24px 0; }
        .display-box { background-color: #374151; border-radius: 8px; padding: 16px; margin-bottom: 16px; }
        .display-box .label { font-size: 12px; font-weight: 500; color: #9ca3af; text-transform: uppercase; letter-spacing: 0.05em; }
        .voltage-display { background-color: #374151; border-radius: 8px; padding: 20px; margin-bottom: 10px; }
        #currentVoltage { font-size: 48px; font-weight: 700; }
        .status-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; margin-bottom: 24px; }
        .status-item { background-color: #374151; border-radius: 8px; padding: 16px; }
        .status-value { font-size: 24px; font-weight: 700; }
        .relay-on { color: #4ade80; }
        .relay-off { color: #f87171; }
        #currentTime { font-size: 40px; font-weight: 700; margin: 4px 0; letter-spacing: 2px; }
        #currentDate { font-size: 18px; color: #9ca3af; }
        form { display: flex; flex-direction: column; gap: 16px; }
        label { display: block; text-align: left; font-size: 14px; font-weight: 500; margin-bottom: 4px; }
        input[type="number"], select {
            width: 100%; padding: 10px; border: 1px solid #4b5563; border-radius: 8px; box-sizing: border-box;
            font-size: 16px; background-color: #374151; color: #f9fafb;
        }
        select {
            -webkit-appearance: none; -moz-appearance: none; appearance: none;
            background-image: url('data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%22292.4%22%20height%3D%22292.4%22%3E%3Cpath%20fill%3D%22%239ca3af%22%20d%3D%22M287%2069.4a17.6%2017.6%200%200%200-13-5.4H18.4c-5%200-9.3%201.8-12.9%205.4A17.6%2017.6%200%200%200%200%2082.2c0%205%201.8%209.3%205.4%2012.9l128%20127.9c3.6%203.6%207.8%205.4%2012.8%205.4s9.2-1.8%2012.8-5.4L287%2095c3.5-3.5%205.4-7.8%205.4-12.8%200-5-1.9-9.2-5.5-12.8z%22%2F%3E%3C%2Fsvg%3E');
            background-repeat: no-repeat; background-position: right 1rem center; background-size: 0.65em auto;
        }
        .form-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; }
        .form-grid-2 { display: grid; grid-template-columns: repeat(2, 1fr); gap: 10px; }
        button, .button-link {
            width: 100%; padding: 12px; border: none; border-radius: 8px; background-color: #4f46e5; color: white;
            font-size: 16px; font-weight: 500; cursor: pointer; transition: background-color 0.2s;
            text-decoration: none; display: inline-block; box-sizing: border-box;
        }
        button:hover, .button-link:hover { background-color: #4338ca; }
        #statusMessage { height: 24px; font-size: 16px; font-weight: 500; margin-top: 16px;}
        .status-success { color: #4ade80; }
        .status-error { color: #f87171; }
        .links { padding-top: 16px; margin-top: 24px; border-top: 1px solid #374151; }
        .links p { font-weight: 600; margin: 0 0 8px 0; }
        .links-container { display: flex; flex-direction: column; gap: 10px; }
        details > summary { cursor: pointer; color: #9ca3af; margin-top: 16px; padding: 8px; border-radius: 8px; }
        details > summary:hover { background-color: #374151; }
    </style>
</head>
<body>
    <div class="card">
        <h1>ACWatch</h1>
        <p>Painel de controle do dispositivo.</p>
        
        <div class="voltage-display">
            <div class="label">Tens&atilde;o</div>
            <div id="currentVoltage" class="status-value">-- V</div>
        </div>

        <div class="status-grid">
            <div class="status-item">
                <div class="label">Modo</div>
                <div id="currentMode" class="status-value">--</div>
            </div>
            <div class="status-item">
                <div class="label">Rel&eacute;</div>
                <div id="relayStatus" class="status-value">--</div>
            </div>
        </div>

        <div class="display-box">
            <p class="label">HORA ATUAL DO DISPOSITIVO</p>
            <p id="currentTime">--:--:--</p>
            <p id="currentDate">--/--/----</p>
        </div>

        <div id="statusMessage"></div>

        <div class="links">
            <p>Configura&ccedil;&otilde;es:</p>
            <div class="links-container">
                <form action="/modo" method="POST" style="margin:0;"><button type="submit">Alternar Modo 110V/220V</button></form>
                <a href="/scan" class="button-link">Configurar Wi-Fi</a>
                <a href="/falhas" class="button-link">Ver Falhas</a>
            </div>
        </div>

        <details>
            <summary>Ajustar Data e Hora</summary>
            <form id="timeForm" style="margin-top: 16px;">
                <div class="form-grid">
                    <div><label for="day">Dia</label><select id="day" name="day"></select></div>
                    <div><label for="month">M&ecirc;s</label><select id="month" name="month"></select></div>
                    <div><label for="year">Ano</label><select id="year" name="year"></select></div>
                    <div><label for="hour">Hora</label><select id="hour" name="hour"></select></div>
                    <div><label for="minute">Minuto</label><select id="minute" name="minute"></select></div>
                    <div><label for="second">Segundo</label><select id="second" name="second"></select></div>
                </div>
                <button type="submit">Confirmar Ajuste de Hora</button>
            </form>
        </details>
        
        <details>
            <summary>Ajustar Par&acirc;metros de Tens&atilde;o</summary>
            <form id="paramsForm" style="margin-top: 16px;">
                <div class="form-grid-2">
                    <div><label for="min110">M&iacute;n. 110V</label><input type="number" id="min110" name="min110" step="0.1"></div>
                    <div><label for="max110">M&aacute;x. 110V</label><input type="number" id="max110" name="max110" step="0.1"></div>
                    <div><label for="min220">M&iacute;n. 220V</label><input type="number" id="min220" name="min220" step="0.1"></div>
                    <div><label for="max220">M&aacute;x. 220V</label><input type="number" id="max220" name="max220" step="0.1"></div>
                </div>
                <button type="submit">Salvar Par&acirc;metros</button>
            </form>
        </details>
    </div>
<script>
    const timeForm = document.getElementById('timeForm');
    const paramsForm = document.getElementById('paramsForm');
    const statusMessage = document.getElementById('statusMessage');
    let initialParamsLoad = true; // Flag para controlar o carregamento inicial dos parâmetros

    function populateDateTimeSelectors() {
        const now = new Date();
        const currentYear = now.getFullYear();
        const daySel = document.getElementById('day');
        const monthSel = document.getElementById('month');
        const yearSel = document.getElementById('year');
        const hourSel = document.getElementById('hour');
        const minuteSel = document.getElementById('minute');
        const secondSel = document.getElementById('second');
        const pad = (n) => n.toString().padStart(2, '0');
        for (let i = 1; i <= 31; i++) daySel.options.add(new Option(pad(i), i));
        for (let i = 1; i <= 12; i++) monthSel.options.add(new Option(pad(i), i));
        for (let i = currentYear - 5; i <= currentYear + 5; i++) yearSel.options.add(new Option(i, i));
        for (let i = 0; i <= 23; i++) hourSel.options.add(new Option(pad(i), i));
        for (let i = 0; i <= 59; i++) minuteSel.options.add(new Option(pad(i), i));
        for (let i = 0; i <= 59; i++) secondSel.options.add(new Option(pad(i), i));
        daySel.value = now.getDate();
        monthSel.value = now.getMonth() + 1;
        yearSel.value = currentYear;
        hourSel.value = now.getHours();
        minuteSel.value = now.getMinutes();
        secondSel.value = now.getSeconds();
    }

    async function fetchStatus() {
        try {
            const response = await fetch('/get-status');
            if (!response.ok) throw new Error('Network response was not ok');
            const data = await response.json();
            
            document.getElementById('currentTime').textContent = data.time;
            document.getElementById('currentDate').textContent = data.date;
            document.getElementById('currentVoltage').textContent = data.voltage + ' V';
            document.getElementById('currentMode').textContent = data.mode;
            
            const relayStatusEl = document.getElementById('relayStatus');
            relayStatusEl.textContent = data.relay;
            relayStatusEl.className = 'status-value ' + (data.relay === 'LIGADO' ? 'relay-on' : 'relay-off');

            // Apenas preenche os campos de parâmetros no carregamento inicial
            // ou após salvar, para não atrapalhar a digitação.
            if (initialParamsLoad) {
                document.getElementById('min110').value = data.limMin110;
                document.getElementById('max110').value = data.limMax110;
                document.getElementById('min220').value = data.limMin220;
                document.getElementById('max220').value = data.limMax220;
                initialParamsLoad = false; // Desativa a flag após o primeiro carregamento
            }

        } catch (error) {
            console.error('Erro ao buscar status:', error);
        }
    }
    async function handleTimeSubmit(event) {
        event.preventDefault();
        const day = document.getElementById('day').value;
        const month = document.getElementById('month').value;
        const year = document.getElementById('year').value;
        const hour = document.getElementById('hour').value;
        const minute = document.getElementById('minute').value;
        const second = document.getElementById('second').value;
        const pad = (n) => n.toString().padStart(2, '0');
        const formData = new URLSearchParams();
        formData.append('date', `${year}-${pad(month)}-${pad(day)}`);
        formData.append('time', `${pad(hour)}:${pad(minute)}:${pad(second)}`);
        try {
            const response = await fetch('/set-time', { method: 'POST', body: formData });
            const resultText = await response.text();
            showStatusMessage(resultText, response.ok);
            if(response.ok) fetchStatus();
        } catch (error) {
            showStatusMessage('Erro de conexao ao ajustar.', false);
        }
    }
    async function handleParamsSubmit(event) {
        event.preventDefault();
        const formData = new URLSearchParams(new FormData(paramsForm));
        try {
            const response = await fetch('/set-params', { method: 'POST', body: formData });
            const resultText = await response.text();
            showStatusMessage(resultText, response.ok);
            if (response.ok) {
                initialParamsLoad = true; // Permite que os campos sejam atualizados com os novos valores na próxima busca
                fetchStatus();
            }
        } catch (error) {
            showStatusMessage('Erro de conexao ao salvar parametros.', false);
        }
    }
    function showStatusMessage(message, isSuccess) {
        statusMessage.textContent = message;
        statusMessage.className = isSuccess ? 'status-success' : 'status-error';
        setTimeout(() => { statusMessage.textContent = ''; }, 5000);
    }
    document.addEventListener('DOMContentLoaded', () => {
        populateDateTimeSelectors();
        fetchStatus();
        setInterval(fetchStatus, 1000);
        timeForm.addEventListener('submit', handleTimeSubmit);
        paramsForm.addEventListener('submit', handleParamsSubmit);
    });
</script>
</body>
</html>
)=====";


// =================================================================
// === FUNÇÃO SETUP: O que acontece quando o ESP32 liga ==========
// =================================================================
void setup() {
  Serial.begin(115200);

  pinMode(pinRele1, OUTPUT);
  pinMode(pinRele2, OUTPUT);
  pinMode(pinBotaoModo, INPUT_PULLUP);
  pinMode(pinResetWiFi, INPUT_PULLUP);
  
  desligarRele(); 
  tempoUltimaTensaoSegura = millis(); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display nao encontrado");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ACWatch iniciando...");
  display.display();

  if (!SPIFFS.begin(true)) {
    Serial.println("Falha ao montar SPIFFS");
  }

  // Inicia a memória para salvar as preferências
  preferences.begin("ACWatch", false);

  // Carrega os parâmetros de tensão salvos ou usa os padrões
  limiteMin127 = preferences.getFloat("limMin127", 95.0);
  limiteMax127 = preferences.getFloat("limMax127", 135.0);
  limiteMin220 = preferences.getFloat("limMin220", 200.0);
  limiteMax220 = preferences.getFloat("limMax220", 265.0);

  configurarWiFi();

  // Configura as "páginas" do servidor web
  server.on("/", HTTP_GET, handleMainPage);
  server.on("/get-status", HTTP_GET, handleGetStatus);
  server.on("/set-time", HTTP_POST, handleSetTime);
  server.on("/set-params", HTTP_POST, handleSetParams); // <-- ROTA NOVA
  server.on("/modo", HTTP_POST, handleAlternarModo);
  server.on("/scan", HTTP_GET, handleScanWiFi);
  server.on("/configurar", HTTP_POST, handleConfigurarWiFi);
  server.on("/falhas", HTTP_GET, handleFalhas);
  server.on("/limpar", HTTP_POST, handleLimparFalhas);
  
  server.begin();

  if (MDNS.begin("acwatch")) {
    Serial.println("mDNS iniciado: http://acwatch.local");
  } else {
    Serial.println("Falha ao iniciar mDNS");
  }

  if (!rtc.begin()) {
    Serial.println("RTC nao encontrado");
    while (true);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC perdeu a hora, configurando padrao");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// =================================================================
// === FUNÇÃO LOOP: O que fica repetindo sem parar ===============
// =================================================================
void loop() {
  server.handleClient();

  static unsigned long ultimoClique = 0;
  if (digitalRead(pinBotaoModo) == LOW && millis() - ultimoClique > 1000) {
    modo220V = !modo220V;
    ultimoClique = millis();
    Serial.println(modo220V ? "Modo 220V" : "Modo 110V");
  }

  if (digitalRead(pinResetWiFi) == LOW) {
    if (tempoPressionado == 0) tempoPressionado = millis();
    if (millis() - tempoPressionado > 5000) {
      Serial.println("Reset Wi-Fi solicitado!");
      preferences.clear();
      delay(1000);
      ESP.restart();
    }
  } else {
    tempoPressionado = 0;
  }

  if (millis() - ultimoAtualizacao > 500) {
    atualizarDisplay(lerTensaoRMS(), rtc.now());
    ultimoAtualizacao = millis();
  }

  // A lógica de proteção agora usa as variáveis que podem ser ajustadas
  float tensao = lerTensaoRMS();
  float limiteMin = modo220V ? limiteMin220 : limiteMin127;
  float limiteMax = modo220V ? limiteMax220 : limiteMax127;

  if (tensao < limiteMin || tensao > limiteMax) {
    if (estadoRele) {
      desligarRele();
      Serial.println("Tensao fora da faixa! Rele desligado.");
      registrarFalha(tensao);
    }
    tempoUltimaTensaoSegura = millis();
  }
  else {
    if (!estadoRele && (millis() - tempoUltimaTensaoSegura > 10000)) {
      ligarRele();
      Serial.println("Tensao estavel por 10s. Rele religado.");
    }
  }
}

// =================================================================
// === FUNÇÕES DO SERVIDOR WEB: O que cada endereço faz ==========
// =================================================================

void handleMainPage() {
  server.send(200, "text/html", PAGE_MAIN);
}

void handleGetStatus() {
  DateTime now = rtc.now();
  
  String dateStr = String(formatDigit(now.day())) + "/" + String(formatDigit(now.month())) + "/" + String(now.year());
  String timeStr = String(formatDigit(now.hour())) + ":" + String(formatDigit(now.minute())) + ":" + String(formatDigit(now.second()));
  String voltageStr = String(lerTensaoRMS(), 1);
  String modeStr = modo220V ? "220V" : "110V";
  String relayStr = estadoRele ? "LIGADO" : "DESLIGADO";

  String json = "{";
  json += "\"date\":\"" + dateStr + "\",";
  json += "\"time\":\"" + timeStr + "\",";
  json += "\"voltage\":" + voltageStr + ",";
  json += "\"mode\":\"" + modeStr + "\",";
  json += "\"relay\":\"" + relayStr + "\",";
  // Adiciona os limites atuais ao JSON para preencher o formulário na página
  json += "\"limMin110\":" + String(limiteMin127) + ",";
  json += "\"limMax110\":" + String(limiteMax127) + ",";
  json += "\"limMin220\":" + String(limiteMin220) + ",";
  json += "\"limMax220\":" + String(limiteMax220);
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleSetTime() {
  if (server.hasArg("date") && server.hasArg("time")) {
    String dateStr = server.arg("date");
    String timeStr = server.arg("time");
    int year, month, day, hour, minute, second;
    sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day);
    sscanf(timeStr.c_str(), "%d:%d:%d", &hour, &minute, &second);
    rtc.adjust(DateTime(year, month, day, hour, minute, second));
    server.send(200, "text/plain", "Data e Hora ajustadas com sucesso!");
  } else {
    server.send(400, "text/plain", "Erro: Dados incompletos.");
  }
}

// <-- NOVA FUNÇÃO PARA SALVAR OS PARÂMETROS DE TENSÃO -->
void handleSetParams() {
  // Verifica se todos os 4 argumentos foram recebidos
  if (server.hasArg("min110") && server.hasArg("max110") && server.hasArg("min220") && server.hasArg("max220")) {
    // Atualiza as variáveis globais com os novos valores
    limiteMin127 = server.arg("min110").toFloat();
    limiteMax127 = server.arg("max110").toFloat();
    limiteMin220 = server.arg("min220").toFloat();
    limiteMax220 = server.arg("max220").toFloat();

    // Salva os novos valores na memória permanente
    preferences.putFloat("limMin127", limiteMin127);
    preferences.putFloat("limMax127", limiteMax127);
    preferences.putFloat("limMin220", limiteMin220);
    preferences.putFloat("limMax220", limiteMax220);
    
    Serial.println("Novos parametros de tensao salvos!");
    server.send(200, "text/plain", "Parametros salvos com sucesso!");
  } else {
    server.send(400, "text/plain", "Erro: Dados de parametros incompletos.");
  }
}

void configurarWiFi() {
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  if (ssid != "" && password != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Conectando ao Wi-Fi: ");
    Serial.println(ssid);
    unsigned long inicio = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - inicio < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConectado ao Wi-Fi!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      return;
    } else {
      Serial.println("\nFalha ao conectar ao Wi-Fi.");
    }
  }
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Modo AP ativado. Acesse: http://");
  Serial.println(IP);
}

void handleAlternarModo() {
  modo220V = !modo220V;
  tempoUltimaTensaoSegura = millis();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleScanWiFi() {
  int n = WiFi.scanNetworks();
  String html = R"=====(
   <!DOCTYPE html><html lang="pt-BR"><head>
   <meta name="viewport" content="width=device-width, initial-scale=1"><title>Scan Wi-Fi</title>
   <style>
        body {font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;margin: 0;padding: 20px;display: flex;justify-content: center;align-items: center;min-height: 100vh;box-sizing: border-box;background-color: #111827;color: #f9fafb;}
        .card {background-color: #1f2937;border-radius: 16px;padding: 32px;width: 100%;max-width: 400px;box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);text-align: center;}
        h1 {font-size: 28px;font-weight: 700;margin: 0 0 24px 0;}
        .wifi-item {padding: 12px; border-bottom: 1px solid #4b5563; cursor: pointer; text-align: left;}
        .wifi-item:hover {background-color: #374151;}
        form { display: flex; flex-direction: column; gap: 16px; margin-top: 24px;}
        input {width: 100%;padding: 10px;border: 1px solid #4b5563;border-radius: 8px;box-sizing: border-box;font-size: 16px;background-color: #374151;color: #f9fafb;}
        button, .button-link {width: 100%;padding: 12px;border: none;border-radius: 8px;background-color: #4f46e5;color: white;font-size: 16px;font-weight: 500;cursor: pointer;transition: background-color 0.2s;text-decoration: none;display: inline-block;box-sizing: border-box; margin-top: 8px;}
        button:hover, .button-link:hover { background-color: #4338ca; }
   </style>
   </head><body>
   <div class="card">
     <h1>Redes Wi-Fi Dispon&iacute;veis</h1>
     <div style="max-height: 200px; overflow-y: auto; border: 1px solid #4b5563; border-radius: 8px;">
  )=====";

  if (n > 0) {
    for (int i = 0; i < n; i++) {
      html += "<div class='wifi-item' onclick=\"document.getElementById('ssid').value='" + WiFi.SSID(i) + "'\">" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + "dBm)</div>";
    }
  } else {
    html += "<p style='padding: 12px;'>Nenhuma rede encontrada.</p>";
  }

  html += R"=====(
     </div>
     <form action="/configurar" method="POST">
       <input type="text" id="ssid" name="ssid" placeholder="Nome da Rede (SSID)">
       <input type="password" id="password" name="password" placeholder="Senha">
       <button type="submit">Salvar e Reiniciar</button>
     </form>
     <a href="/" class="button-link">Voltar</a>
   </div>
   </body></html>
  )=====";

  server.send(200, "text/html", html);
}

void handleConfigurarWiFi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    server.send(200, "text/plain", "Credenciais salvas! Reiniciando...");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Falha ao salvar credenciais.");
  }
}

void handleFalhas() {
  File log = SPIFFS.open("/falhas.log", FILE_READ);
  String html = R"=====(
  <!DOCTYPE html><html lang="pt-BR"><head><meta name="viewport" content="width=device-width, initial-scale=1"><title>Hist&oacute;rico de Falhas</title>
  <style>
        body {font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;margin: 0;padding: 20px;display: flex;justify-content: center;align-items: center;min-height: 100vh;box-sizing: border-box;background-color: #111827;color: #f9fafb;}
        .card {background-color: #1f2937;border-radius: 16px;padding: 32px;width: 100%;max-width: 600px;box-shadow: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);text-align: center;}
        h1 {font-size: 28px;font-weight: 700;margin: 0 0 24px 0;}
        table {width: 100%; border-collapse: collapse; }
        th, td {border: 1px solid #4b5563; padding: 12px; text-align: left;}
        th {background-color: #374151;}
        button, .button-link {width: 100%;padding: 12px;border: none;border-radius: 8px;background-color: #4f46e5;color: white;font-size: 16px;font-weight: 500;cursor: pointer;transition: background-color 0.2s;text-decoration: none;display: inline-block;box-sizing: border-box; margin-top: 16px;}
        button:hover, .button-link:hover { background-color: #4338ca; }
        .button-danger { background-color: #dc2626; }
        .button-danger:hover { background-color: #b91c1c; }
  </style>
  </head><body>
  <div class="card">
    <h1>Hist&oacute;rico de Falhas</h1>
    <div style="max-height: 400px; overflow-y: auto;">
    <table>
  )=====";

  if (!log || log.size() == 0) {
    html += "<tr><td colspan='2' style='text-align: center;'>Nenhuma falha registrada.</td></tr>";
  } else {
    html += "<tr><th>Data e Hora</th><th>Evento</th></tr>";
    while (log.available()) {
      String linha = log.readStringUntil('\n');
      html += "<tr><td colspan='2'>" + linha + "</td></tr>";
    }
  }
  log.close();
  
  html += R"=====(
    </table>
    </div>
    <form action="/limpar" method="POST" onsubmit="return confirm('Tem certeza que deseja apagar todo o historico?');">
      <button type="submit" class="button-danger">Limpar Hist&oacute;rico</button>
    </form>
    <a href="/" class="button-link">Voltar</a>
  </div>
  </body></html>
  )=====";
  server.send(200, "text/html", html);
}

void handleLimparFalhas() {
  if (SPIFFS.exists("/falhas.log")) {
    SPIFFS.remove("/falhas.log");
    server.sendHeader("Location", "/falhas");
    server.send(303);
  } else {
    server.send(200, "text/plain", "Nenhum historico para apagar.");
  }
}


// =================================================================
// === FUNÇÕES AUXILIARES: Tarefas de apoio ========================
// =================================================================

String formatDigit(int digit) {
  if (digit < 10) {
    return "0" + String(digit);
  }
  return String(digit);
}

float lerTensaoRMS() {
  const int N = 1000;
  float soma = 0;
  for (int i = 0; i < N; i++) {
    int leitura = analogRead(pinSensor);
    float tensao = (leitura - 2048) * (3.3 / 4096.0);
    soma += tensao * tensao;
  }
  float vrms = sqrt(soma / N);
  float fator = modo220V ? fatorCalibracao220 : fatorCalibracao127;
  return vrms * fator;
}

void desligarRele() {
  digitalWrite(pinRele1, LOW);
  digitalWrite(pinRele2, LOW);
  estadoRele = false;
}

void ligarRele() {
  digitalWrite(pinRele1, HIGH);
  digitalWrite(pinRele2, HIGH);
  estadoRele = true;
}

void printCentralizado(String texto, int y, int tamanho) {
  int16_t x1, y1;
  uint16_t largura, altura;
  display.setTextSize(tamanho);
  display.getTextBounds(texto, 0, y, &x1, &y1, &largura, &altura);
  int x = (SCREEN_WIDTH - largura) / 2;
  display.setCursor(x, y);
  display.print(texto);
}

void atualizarDisplay(float tensao, DateTime now) {
  bool alertaTensao = false;
  float limiteMin = modo220V ? limiteMin220 : limiteMin127;
  float limiteMax = modo220V ? limiteMax220 : limiteMax127;
  if (tensao < limiteMin || tensao > limiteMax) {
    alertaTensao = true;
  }

  if (alertaTensao && (millis() % 1000 < 500)) {
    display.invertDisplay(true);
  } else {
    display.invertDisplay(false);
  }

  display.clearDisplay();

  printCentralizado("ACWatch", 0, 2);

  String leitura = String(tensao, 1) + " V";
  printCentralizado(leitura, 18, 2);

  String status = String(modo220V ? "MODO: 220V" : "MODO: 110V") +
                  (estadoRele ? " LIGADO" : " DESLIGADO");
  printCentralizado(status, 36, 1);

  String horaBR = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + " " +
                  String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  printCentralizado(horaBR, 48, 1);

  display.display();
}

void registrarFalha(float tensao) {
  DateTime now = rtc.now();
  String registro = now.timestamp() + " - Tensao fora da faixa: " + String(tensao, 1) + "V\n";

  File log = SPIFFS.open("/falhas.log", FILE_APPEND);
  if (log) {
    log.print(registro);
    log.close();
    Serial.println("Falha registrada no SPIFFS.");
  } else {
    Serial.println("Erro ao abrir falhas.log");
  }
}
