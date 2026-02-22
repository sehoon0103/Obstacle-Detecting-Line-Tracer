# Obstacle-Detecting Line Tracer (라인트레이서 + 장애물 감지)

1학년 때 Arduino IDE로 만든 초보 단계 프로젝트입니다.  
IR 센서로 Line Following을 하고, 초음파 센서로 전방 장애물을 감지해 Warning(경고) / Stop(정지) 동작을 추가했습니다.

---

## 1) 한 줄 소개(What)
라인 추종 주행 중 전방 장애물 접근 시 부저/LED 경고 후 일정 거리 이하에서 모터 정지하는 라인트레이서입니다.

---

## 2) 구성(HW)

- Arduino UNO
- Motor Driver: TB6612FNG
  - 참고: STBY는 실제 결선에서 5V(HIGH)로 고정
- IR 센서(좌/우) 2개: 라인 감지
- Ultrasonic sensor(**HC-SR04**): 거리 측정
- Buzzer(부저), Warning LED

---

## 3) 동작 로직(Logic)

### (1) Obstacle Handling
- HC-SR04로 거리(cm) 측정
- 거리 `<= WARN_DIST`:
  - Warning LED ON
  - 부저 beep(거리 따라 간격 변화)
- 거리 `<= STOP_DIST`:
  - 모터 즉시 정지

### (2) Line Following (장애물 없을 때만)
- IR 센서(analog) 값을 threshold로 판정
- 주행 제어
  - 좌/우 모두 감지 → 직진
  - 좌만 감지 → 좌회전 보정
  - 우만 감지 → 우회전 보정
  - 둘 다 미감지 → 정지

---

## 4) 주요 파라미터(코드 기준)

- IR threshold: `thrL = 750`, `thrR = 750`
- speed: `80`
- Distance threshold:
  - `STOP_DIST = 5cm`
  - `WARN_DIST = 12cm`
- Serial debug: `9600 bps`

---

## 5) 검증(Testing / Verification)

- 거리 조건 테스트: 15cm / 5cm 케이스에서 경고/정지 동작 확인
- PDF에 구성/동작 흐름 및 테스트 결과 정리

---

## 6) 한계점(Limitations)

- IR 센서가 환경(조명/바닥)에 따라 오작동 가능
- 초음파 센서 사각지대(각도)에 따른 미감지 가능
- 연속 장애물/곡선 구간에서 타이밍 처리 한계

---

## 7) 개선 아이디어(Next)

- IR 센서 감도/threshold 보정으로 안정화
- `millis()` 기반으로 루프 타이밍 개선(Blocking 줄이기)

---
## 8) 혼자 끝까지 해본 경험(Ownership)

이 프로젝트는 1학년 때 Arduino UNO 키트 + IR 센서 + 모터 드라이버 섀시 키트를 직접 구매해서,  
납땜 → 조립 → 코딩 → 동작 검증 → 유지/수정 까지 전 과정을 개인 단독으로 완료했습니다.


