#!/usr/bin/env python3
"""
    Write a Python program that uses machine learning to map binary data (formatted as lists of C++-style hex int literals) to human-readable text.
    The program should:
    ✅ Load a JSON dataset where each record contains:

    "binary": A list of C++-style hex literals (e.g., ["0xAB", "0xCD", "0x12"])
    "text": Corresponding human-readable text output
    ✅ Convert binary hex lists into numerical feature arrays.
    ✅ Train a Random Forest model to predict text labels from binary sequences.
    ✅ Save the trained model and label encoder for later use.
    ✅ Support command-line input to:
    Train the model from a JSON file (data.json)
    Predict text from a provided list of hex literals (--predict 0xAB 0xCD 0x12)
    ✅ Ensure the script follows a modular structure with:
    main() to handle CLI interactions
    A predict() function that loads the trained model and infers text from a hex list
    A train_model() function that processes the dataset, extracts features, and trains the model
    ✅ Handle dynamic binary message lengths by padding sequences to the max observed length.
    ✅ Use argparse to handle CLI parameters.
    ✅ Save the trained model as "binary_to_text_model.pkl" and the label encoder as "label_encoder.pkl".
"""

import json
import numpy as np
import argparse
import joblib
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import LabelEncoder

# --- Constants ---
MODEL_FILE = "var/binary_to_text_model.pkl"
ENCODER_FILE = "var/label_encoder.pkl"

# --- Function to load JSON data ---
def load_data(json_file):
    """Load binary-text mapping records from a JSON file."""
    with open(json_file, "r") as f:
        data = json.load(f)
    return data["records"]

# --- Helper Functions ---
def hex_list_to_bytes(hex_list):
    """Convert a list of C++-style hex literals (e.g., ['0xAB', '0xCD']) to bytes."""
    return bytes(int(h, 16) for h in hex_list)

def extract_features(binary_data):
    """Convert binary sequence to a numerical feature array."""
    return np.frombuffer(binary_data, dtype=np.uint8)

def prepare_dataset(records):
    """Convert JSON records into numerical feature arrays and labels."""
    X, y = [], []
    for record in records:
        binary_data = hex_list_to_bytes(record["binary"])
        features = extract_features(binary_data)
        X.append(features)
        y.append(record["text"])
    return X, y

# --- Train Model ---
def train_model(json_file):
    """Train a RandomForest model on the given JSON dataset."""
    records = load_data(json_file)
    X, y = prepare_dataset(records)

    # Determine the max length for padding
    max_length = max(len(seq) for seq in X)
    X_padded = [np.pad(seq, (0, max_length - len(seq)), mode='constant') for seq in X]

    # Convert to NumPy arrays
    X_padded = np.array(X_padded)

    # Encode text labels
    label_encoder = LabelEncoder()
    y_encoded = label_encoder.fit_transform(y)

    # Train/Test Split
    X_train, X_test, y_train, y_test = train_test_split(X_padded, y_encoded, test_size=0.2, random_state=42)

    # Train the model
    model = RandomForestClassifier(n_estimators=100, random_state=42)
    model.fit(X_train, y_train)

    # Save the model and encoder
    joblib.dump(model, MODEL_FILE)
    joblib.dump(label_encoder, ENCODER_FILE)

    print(f"Model trained and saved as '{MODEL_FILE}'.")
    print(f"Label encoder saved as '{ENCODER_FILE}'.")

# --- Predict Function ---
def predict(hex_list):
    """Predict the human-readable text for a given binary input in hex list format."""
    binary_data = hex_list_to_bytes(hex_list)
    features = extract_features(binary_data)

    # Load trained model and encoder
    model = joblib.load(MODEL_FILE)
    label_encoder = joblib.load(ENCODER_FILE)

    # Pad input to match the training max length
    max_length = model.n_features_in_  # Get the expected feature size
    features_padded = np.pad(features, (0, max_length - len(features)), mode='constant').reshape(1, -1)

    # Predict
    predicted_label = model.predict(features_padded)
    predicted_text = label_encoder.inverse_transform(predicted_label)[0]

    return predicted_text

# --- Main Function ---
def main():
    """Parse command-line arguments and execute the appropriate action."""
    parser = argparse.ArgumentParser(description="Binary-to-Text ML Model")
    parser.add_argument("json_file", help="Path to input JSON file for training")
    parser.add_argument("--predict", nargs="+", metavar="HEX", help="Predict text from a list of hex literals (e.g., 0xAB 0xCD 0x12)")
    
    args = parser.parse_args()

    if args.predict:
        print(f"Predicted Text: {predict(args.predict)}")
    else:
        train_model(args.json_file)

if __name__ == "__main__":
    main()
