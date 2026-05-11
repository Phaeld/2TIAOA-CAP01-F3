"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import libraries
import smtplib
from email.mime.text import MIMEText
from app.config import Config
from app.models.vital_signs import VitalSigns

class EmailService:
    @staticmethod
    def send_alert_email(vital_signs: VitalSigns, risk_result: dict) -> bool:
        subject = f"[CardioIA] Alerta {risk_result['risk_level']} - Paciente {vital_signs.patient_id}"
        body = EmailService._build_email_body(vital_signs, risk_result)

        # Simulated mode: recommended for academic presentations and local tests
        if Config.EMAIL_SIMULATION_MODE:
            print("\n========== SIMULAÇÃO DE ENVIO DE E-MAIL ==========")
            print(f"Para: {Config.ALERT_RECEIVER_EMAIL}")
            print(f"Assunto: {subject}")
            print(body)
            print("==================================================\n")
            return True
        
        # Actual transmission via SMTP
        if not Config.SMTP_USER or not Config.SMTP_PASSWORD:
            print("[EMAIL] Credenciais SMTP não configuradas.")
            return False
        
        message = MIMEText(body, "plain", "utf-8")
        message["Subject"] = subject
        message["From"] = Config.SMTP_USER
        message["To"] = Config.ALERT_RECEIVER_EMAIL

        try:
            with smtplib.SMTP(Config.SMTP_HOST, Config.SMTP_PORT) as server:
                server.starttls()
                server.login(Config.SMTP_USER, Config.SMTP_PASSWORD)
                server.send_message(message)

            print("[EMAIL] Alerta enviado com sucesso.")
            return True
        except Exception as error:
            print(f"[EMAIL] Falha ao enviar e-mail: {error}")
            return False
        
    @staticmethod
    def _build_email_body(vital_signs: VitalSigns, risk_result: dict) -> str:
        # Email body automatically generated based on detected risks.
        alerts_text = "\n".join(
            f"- {alert['type']} ({alert['severity']}): {alert['message']}"
            for alert in risk_result["alerts"]
        )

        return f"""
ALERTA AUTOMATIZADO - CARDIOIA

Paciente: {vital_signs.patient_id}
Dispositivo: {vital_signs.device_id}
Data/Hora: {vital_signs.timestamp}

Nível geral de risco {risk_result['risk_level']}

Sinais vitais recebidos:
- Temperatura: {vital_signs.temperature_c} ºC
- Frequência cardíaca: {vital_signs.heart_rate_bpm} BPM
- Movimento detectado: {vital_signs.movement_detected}
- Botão de emergência pressionado: {vital_signs.emergency_button_pressed}

Alertas identificados:
{alerts_text}

Recomendação:
Verificar imediatamente o estado do paciente e acionar atendimento responsável caso necessário.
"""