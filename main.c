/*
  2륜 라인트레이서 + 장애물 경고
  (디지털 IR + HC-SR04 초음파 + 부저 tone())
  모터 드라이버: TB6612FNG (STBY 핀은 물리적으로 5V 레일에 연결)
*/

#define AIN1_PIN   4   // 좌 모터 방향1
#define AIN2_PIN   3   // 좌 모터 방향2
#define PWMA_PIN   5   // 좌 모터 속도(PWM)

#define LED_WARN_PIN 7 // 거리감지 led

#define BIN1_PIN  10   // 우 모터 방향1
#define BIN2_PIN   2   // 우 모터 방향2
#define PWMB_PIN   6   // 우 모터 속도(PWM)

#define IRL_PIN   A3    // 왼쪽 IR 센서
#define IRR_PIN   A2    // 오른쪽 IR 센서

#define TRIG_PIN   13    // 초음파 Trig
#define ECHO_PIN   A0    // 초음파 Echo
#define BUZZER_PIN 11    // 부저 tone

// 라인센서 임계값
const int thrL = 750;
const int thrR = 750;

// 속도 및 거리
const int speed = 80;
const int STOP_DIST = 5;     // 즉시 정지 거리
const int WARN_DIST = 12;    // 경고음 발생 거리

// 상태 변수
bool obstacleDetected = false;
bool isBeeping = false;
unsigned long lastBeepTime = 0;

void setup() {

  pinMode(LED_WARN_PIN, OUTPUT);

  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);

  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);

  pinMode(IRL_PIN, INPUT);
  pinMode(IRR_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("STBY=HIGH");
}

// 거리 측정 (cm)
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 60000);
  if (duration == 0) return 0;
  return duration / 58;
}

// 모터 구동
void drive(int spd, bool leftFwd, bool rightFwd) {
  digitalWrite(AIN1_PIN, leftFwd ? HIGH : LOW);
  digitalWrite(AIN2_PIN, leftFwd ? LOW  : HIGH);
  analogWrite(PWMA_PIN, spd);

  digitalWrite(BIN1_PIN, rightFwd ? HIGH : LOW);
  digitalWrite(BIN2_PIN, rightFwd ? LOW  : HIGH);
  analogWrite(PWMB_PIN, spd);
}

// 장애물 감지 처리
void handleObstacle(long dist) {
  unsigned long now = millis();

  if (dist > 0 && dist <= WARN_DIST) {
    obstacleDetected = true;
    digitalWrite(LED_WARN_PIN, HIGH);  // LED ON

    int interval = map(dist, STOP_DIST, WARN_DIST, 150, 500);

    if (!isBeeping && now - lastBeepTime >= interval) {
      tone(BUZZER_PIN, 1000, 150);  // 1kHz, 150ms
      isBeeping = true;
      lastBeepTime = now;
    }

    if (isBeeping && now - lastBeepTime >= 150) {
      noTone(BUZZER_PIN);
      isBeeping = false;
    }

    if (dist <= STOP_DIST) {
      drive(0, true, true);  // 즉시 정지
    }
  } else {
    digitalWrite(LED_WARN_PIN, LOW);   // 🚨 LED OFF
    noTone(BUZZER_PIN);
    isBeeping = false;
    obstacleDetected = false;
  }
}

void loop() {
  // 1. 거리 측정
  long dist = measureDistance();

  // 2. 장애물 감지 및 경고 처리
  handleObstacle(dist);

  // 3. 라인트레이싱 (장애물 없을 때만 동작)
  int vL = analogRead(IRL_PIN);
  int vR = analogRead(IRR_PIN);
  bool onL = (vL > thrL);
  bool onR = (vR > thrR);

  if (!obstacleDetected) {
    if      (onL && onR) drive(speed, true,  true);   // 직진
    else if (onL)        drive(speed, false, true);   // 좌회전
    else if (onR)        drive(speed, true,  false);  // 우회전
    else                 drive(0,     true,  true);   // 정지
  }

  // 4. 디버깅 출력
  Serial.print("vL="); Serial.print(vL);
  Serial.print(" vR="); Serial.print(vR);
  Serial.print(" | L="); Serial.print(onL);
  Serial.print(" R="); Serial.print(onR);
  Serial.print(" | Dist="); Serial.print(dist);
  Serial.print("cm | Obstacle="); Serial.println(obstacleDetected);

  delay(10);
}
