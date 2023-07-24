
#define BUZZER_PIN 12

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Liberação
  buzzerLibera();
  delay(1000); // Espere 1 segundo
  
  // Bloqueio
  buzzerBloqueia();
  delay(1000); // Espere 1 segundo
}

// Função para gerar o som de liberação
void buzzerLibera() {
  // Frequência do som para liberação
  int frequencia = 1000; // Altere para a frequência desejada
  int duracao = 500; // Altere para o tempo de duração desejado
  
  // Gera o som no buzzer
  tone(BUZZER_PIN, frequencia, duracao);
}

// Função para gerar o som de bloqueio
void buzzerBloqueia() {
  // Frequência do som para bloqueio
  int frequencia = 500; // Altere para a frequência desejada
  int duracao = 1000; // Altere para o tempo de duração desejado
  
  // Gera o som no buzzer
  tone(BUZZER_PIN, frequencia, duracao);
}
