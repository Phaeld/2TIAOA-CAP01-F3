"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import libraries
import requests
from datetime import datetime


API_URL = "http://127.0.0.1:5000/vitals"


def simulate_emergency_event():
    # Simulates a patient with tachycardia who has pressed the emergency button.
    vital_signs_payload = {
        "device_id": "cardioia_watch_001",
        "patient_id": "patient_001",
        "temperature_c": 37.4,
        "heart_rate_bpm": 132,
        "movement_detected": False,
        "emergency_button_pressed": True,
        "timestamp": datetime.now().isoformat()
    }

    try:
        response = requests.post(API_URL, json=vital_signs_payload, timeout=10)

        print("Status Code:", response.status_code)
        print("Response JSON:")
        print(response.json())

    except requests.exceptions.ConnectionError:
        print("[CLIENT] Não foi possível conectar à API. Verifique se o servidor Flask está rodando.")

    except requests.exceptions.Timeout:
        print("[CLIENT] Tempo limite excedido ao tentar enviar os dados.")

    except Exception as error:
        print(f"[CLIENT] Erro inesperado: {error}")


if __name__ == "__main__":
    simulate_emergency_event()