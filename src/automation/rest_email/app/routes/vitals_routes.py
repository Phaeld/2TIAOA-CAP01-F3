"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import libraries
from flask import Blueprint, request, jsonify
from app.models.vital_signs import VitalSigns
from app.services.risk_analysis_service import RiskAnalysisService
from app.services.email_service import EmailService
from app.services.alert_log_service import AlertLogService

vitals_blueprint = Blueprint("vitals", __name__)


@vitals_blueprint.route("/health", methods=["GET"])
def health_check():
    # Simple endpoint to verify if the API is working
    return jsonify({
        "status": "online",
        "service": "CardioIA REST Monitoring API"
    }), 200


@vitals_blueprint.route("/vitals", methods=["POST"])
def receive_vital_signs():
    try:
        payload = request.get_json()

        if not payload:
            return jsonify({
                "status": "error",
                "message": "JSON payload is required"
            }), 400

        vital_signs = VitalSigns.from_dict(payload)

        # Analyze the risks based on the signals received
        risk_result = RiskAnalysisService.analyze(vital_signs)

        email_sent = False

        # If there is a risk, trigger email automation
        if risk_result["risk_level"] != "NORMAL":
            email_sent = EmailService.send_alert_email(vital_signs, risk_result)

            # Saves logs only when there is an alert
            AlertLogService.save_alert(
                vital_signs=vital_signs.to_dict(),
                risk_result=risk_result,
                email_sent=email_sent
            )

        return jsonify({
            "status": "received",
            "message": "Vital signs processed successfully",
            "data": vital_signs.to_dict(),
            "risk_analysis": risk_result,
            "email_sent": email_sent
        }), 200

    except ValueError as error:
        return jsonify({
            "status": "error",
            "message": f"Invalid payload format: {error}"
        }), 400

    except Exception as error:
        return jsonify({
            "status": "error",
            "message": f"Internal server error: {error}"
        }), 500