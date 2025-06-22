import requests

def send_prediction_request(patient_id, prediction_type):
    url = "http://your-server-ip:8000/predict"
    payload = {
        "id": patient_id,
        "type": prediction_type  # 'colon' or 'dementia'
    }

    try:
        response = requests.post(url, json=payload)
        if response.status_code == 200:
            print("✅ 예측 결과 수신:", response.json())
            return response.json()
        else:
            print("⚠️ 서버 응답 오류:", response.status_code)
    except Exception as e:
        print("❌ 예측 요청 실패:", str(e))
    return None
