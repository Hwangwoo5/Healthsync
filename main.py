from tts_stt.google_stt import recognize_speech
from alerts.notify_kakao import send_alert

def main():
    print("🩺 HealthSync 시스템 시작")
    text = recognize_speech()
    if text:
        send_alert(f"인식된 음성: {text}")

if __name__ == "__main__":
    main()
