from flask import Flask, request, jsonify
import pandas as pd
import base64
from tinydb import TinyDB, Query
import datetime as dt

app = Flask(__name__)

# db = TinyDB('db.json')
dataset = pd.read_csv('dataset.csv')
if not dataset.empty:
    print("Dataset loaded successfully")
else:
    print("Failed to load dataset")

@app.route('/model', methods=['GET'])
def get_model():
    response = {}
    with open('../FederatedLearning/model.tflite', 'rb') as f:
        response['model_encoded'] = base64.b64encode(f.read()).decode('ascii')
    print("model sent successfully")
    return jsonify(response)

@app.route('/update_weights', methods=['POST'])
def update_weights():
    try:
        data = request.get_json()
        if 'weights' not in data:
            return jsonify({'error': 'Weights not provided'}), 400

        weights_base64 = data['weights']
        weights_bytes = base64.b64decode(weights_base64)
        weights = np.frombuffer(weights_bytes, dtype=np.float32)

        return jsonify({'message': 'Weights send successfully'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)