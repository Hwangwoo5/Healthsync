import speech_recognition as sr

def recognize_speech():
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("🎤 말해주세요...")
        try:
            audio = r.listen(source, timeout=5)
            text = r.recognize_google(audio, language="ko-KR")
            print("🗣️ 인식 결과:", text)
            return text
        except sr.UnknownValueError:
            print("❌ 음성 인식 실패")
        except sr.RequestError:
            print("⚠️ Google API 요청 오류")
        return None
