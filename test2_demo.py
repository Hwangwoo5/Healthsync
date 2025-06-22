# test2_demo.py (기능 시연용 데모 코드)

import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QPushButton, QMessageBox

class HealthSyncDemo(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HealthSync 데모")
        self.setGeometry(100, 100, 400, 200)
        self.layout = QVBoxLayout()

        self.label = QLabel("환자 ID: demo_user_001")
        self.layout.addWidget(self.label)

        self.fan_button = QPushButton("팬 켜기")
        self.fan_button.clicked.connect(self.toggle_fan)
        self.layout.addWidget(self.fan_button)

        self.predict_button = QPushButton("용종 예측 요청")
        self.predict_button.clicked.connect(self.send_prediction)
        self.layout.addWidget(self.predict_button)

        self.setLayout(self.layout)

    def toggle_fan(self):
        print("✅ FAN_ON 명령 전송 (시뮬레이션)")
        QMessageBox.information(self, "팬 제어", "팬을 켰습니다. (시뮬레이션)")

    def send_prediction(self):
        print("🧠 colon 예측 요청 전송 (시뮬레이션)")
        result = {"result": "정상"}
        QMessageBox.information(self, "AI 예측 결과", f"용종 예측 결과: {result['result']}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = HealthSyncDemo()
    window.show()
    sys.exit(app.exec_())
