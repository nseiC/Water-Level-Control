// Pinos do sensor ultrassônico
#define TRIG_PIN 18
#define ECHO_PIN 19

// Pino do DAC (escolha entre GPIO 25 ou GPIO 26)
#define DAC_PIN 25

// Velocidade do som em cm/us
#define SOUND_SPEED 0.034

// Limite máximo de nível (10 cm = 3.0V no DAC)
#define MAX_DISTANCE 16.22  // Em cm
#define MAX_DAC_VOLTAGE 3.0  // Em V
#define MIN_DISTANCE 5
//o novo pode ser 5.30 em min distance
// Intervalo do timer em ms
#define TIMER_INTERVAL_MS 100

// Definir o timer
hw_timer_t *timer = NULL;

// Variáveis globais
volatile bool timerFlag = false;         // Controle da interrupção
volatile unsigned long elapsedTime = 0; // Tempo acumulado em ms
volatile float measuredDistance = 0.0;  // Distância medida
volatile int dacValue = 0;              // Valor do DAC calculado
volatile float initialDistance = 0.0;  // Distância medid
volatile float lastDistance = 4.4;  // Distância medid
// Função de interrupção do timer
void IRAM_ATTR onTimer() {
  timerFlag = true;                 // Sinaliza que o timer disparou
  elapsedTime += TIMER_INTERVAL_MS; // Incrementa o tempo acumulado
}

void setup() {
  // Configuração dos pinos do sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicialização do monitor serial
  Serial.begin(115200);
  Serial.println("Sistema iniciado: Sensor + DAC");

  // Configurar o timer
  timer = timerBegin(10000000);           // Timer 0, prescaler 80 (1 µs por tick)
  //timer = timerBegin(0, 80, true);           // Timer 0, prescaler 80 (1 µs por tick) ANTIGO
  timerAttachInterrupt(timer, &onTimer); // Vincula a função de interrupção
  timerWrite(timer, 10000); // Timer para 10 ms (convertido para µs)
  timerAlarm(timer,1000000,true,0);                    // Habilita o alarme do timer
  initialDistance = measureDistance();
}

void loop() {
  // Verifica se o timer disparou
  if (timerFlag) {
    timerFlag = false;  // Reseta o sinalizador

    // Medir a distância e armazenar
    measuredDistance = measureDistance();
  
    // Calcular a saída do DAC com base na distância
    dacValue = calculateDACValue(measuredDistance);

    // Aplicar a saída no DAC
    dacWrite(DAC_PIN, dacValue);

    // Printar as informações
    printInfo();
  }

  // Outras tarefas podem ser realizadas aqui, se necessário
}

// Função para medir a distância com o sensor ultrassônico
float measureDistance() {
  // Certifique-se de que o TRIG está em LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Envie um pulso HIGH de 10 µs no TRIG
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Leia o tempo de retorno do sinal no pino ECHO
  long duration = pulseIn(ECHO_PIN, HIGH, 8000); // Limite de 30 ms (~500 cm)

  // Calcule a distância em cm
  float distance = (duration * SOUND_SPEED) / 2;

  // Limitar a distância ao máximo configurado

  return distance; // Retorna a distância calculada
}

// Função para calcular o valor do DAC com base na distância
int calculateDACValue(float distance) {
  // Relacionar distância com o valor do DAC
  //float voltage = map(distance * 1000, MIN_DISTANCE * 1000, MAX_DISTANCE * 1000, MAX_DAC_VOLTAGE * 1000, 0) / 1000.0; 
  //float voltage = map(MAX_DISTANCE * 1000, distance * 1000, MAX_DISTANCE * 1000, 0, 3*1000) / 1000.0; ANTIGO
  float voltage = 3*(MAX_DISTANCE-distance)/(MAX_DISTANCE-MIN_DISTANCE);
  // Mapear a tensão para o valor do DAC (0-255)
  int dacValue = map(voltage*1000 , 0, 3300, 0, 255);

  // Garantir que o valor esteja no intervalo permitido
  return constrain(dacValue, 0, 255);
}

// Função para printar informações no monitor serial
void printInfo() {
  Serial.print("Tempo: ");
  Serial.print(elapsedTime);
  Serial.print(" ms, Nível: ");
  Serial.print(MAX_DISTANCE-measuredDistance);
  Serial.print(" cm, DistânciA: ");
  Serial.print(measuredDistance);
  Serial.print(" cm, Saída DAC: ");
  Serial.print((float)dacValue / 255 * 3.3); // Conversão para tensão
  Serial.println(" V");
}