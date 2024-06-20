import os
import cv2
import face_recognition
import numpy as np
import time
import subprocess
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QMessageBox, QInputDialog, QLabel, QPushButton, QVBoxLayout, QWidget

class VeloVentureApp(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('VeloVenture')
        self.setGeometry(100, 100, 800, 500)
        self.setStyleSheet("background-color: #54585A;")
        layout = QVBoxLayout()

        title_label = QLabel('Welcome to VeloVenture')
        title_label.setAlignment(QtCore.Qt.AlignCenter)
        title_label.setStyleSheet("background-color: #58A6F0; color: white; font-size: 24px; font-weight: bold;")
        layout.addWidget(title_label)
        # Spacer for vertical space
        layout.addStretch()

        # Button container
        button_layout = QVBoxLayout()
        button_layout.setAlignment(QtCore.Qt.AlignCenter)

        for text, func in [('Login', self.login), ('Register', self.register), ('Quit', QtCore.QCoreApplication.instance().quit)]:
            btn = QPushButton(text)
            btn.setStyleSheet("background-color: #54585A; color: yellow; font-size: 18px; font-weight: bold;")
            btn.clicked.connect(func)
            button_layout.addWidget(btn)

        layout.addLayout(button_layout)
        # Spacer for vertical space
        layout.addStretch()

        self.setLayout(layout)

    def register_user(self, username, filename='usernames.txt'):
        current_directory = os.getcwd()
        try:
            with open(filename, 'r') as file:
                if username in file.read().splitlines():
                    return f"{username} is taken"

            with open(filename, 'a') as file:
                file.write(username + '\n')

            self.take_photo(username)
            return "Successfully registered."
        except FileNotFoundError:
            with open(filename, 'w') as file:
                file.write(username + '\n')
            self.take_photo(username)
            return "Successfully registered."

    def take_photo(self, file_name):
        os.makedirs('users', exist_ok=True)
        self.capture_window = CaptureWindow(file_name, self)
        self.capture_window.show()

    def recognize_person(self, video_capture, known_face_encodings, known_face_names):
        process_this_frame = True
        start_time = time.time()
        while time.time() - start_time < 2:
            ret, frame = video_capture.read()
            if not ret:
                break
            if process_this_frame:
                rgb_small_frame = cv2.cvtColor(cv2.resize(frame, (0, 0), fx=0.25, fy=0.25), cv2.COLOR_BGR2RGB)
                face_encodings = face_recognition.face_encodings(rgb_small_frame, face_recognition.face_locations(rgb_small_frame))

                for face_encoding in face_encodings:
                    matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
                    face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
                    if matches and np.argmin(face_distances) < len(known_face_names):
                        return known_face_names[np.argmin(face_distances)]
            process_this_frame = not process_this_frame
        return "Unknown"

    def Login(self):
        known_face_encodings, known_face_names = [], []
        for filename in os.listdir("users"):
            if filename.endswith((".jpg", ".jpeg")):
                user_image = face_recognition.load_image_file(os.path.join("users", filename))
                encodings = face_recognition.face_encodings(user_image)
                if encodings:
                    known_face_encodings.append(encodings[0])
                    known_face_names.append(os.path.splitext(filename)[0])

        if not known_face_encodings:
            QMessageBox.critical(self, "Error", "No registered users found.")
            return "Unknown"

        self.capture_window = CaptureWindowForLogin(self, known_face_encodings, known_face_names)
        self.capture_window.show()

    def handle_login_result(self, username):
        home_dir = os.path.expanduser("~")
        exe_path = os.path.join(home_dir, "VeloVenture/build", "MyProject.sh")  # Ensure this path points to your executable
        if username != "Unknown":
            print(f"Welcome back, {username}!")
            with open(os.path.join(home_dir, "VeloVenture/build", "CurLogin.txt"), 'w') as file:
                file.write(username)
            subprocess.Popen([exe_path])
            self.close()
        else:
            QMessageBox.critical(self, "Login", "Login failed. Please try again.")

    def login(self):
        self.Login()

    def register(self):
        text, ok = QInputDialog.getText(self, 'Register', 'Enter Username:')
        if ok:
            result = self.register_user(text)
            if result == "Successfully registered.":
                self.capture_window.show()
            else:
                QMessageBox.critical(self, "Registration", result)


class CaptureWindow(QWidget):
    def __init__(self, file_name, parent=None):
        super().__init__(parent)
        self.file_name = file_name
        self.initUI()
        self.cap = cv2.VideoCapture(0)
        if not self.cap.isOpened():
            QMessageBox.critical(self, "Error", "Cannot access camera.")
            self.close()
            return
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_frame)
        self.timer.start(30)

    def initUI(self):
        self.setWindowTitle('Capture Photo')
        self.setGeometry(100, 100, 800, 600)

        layout = QVBoxLayout()
        self.indicator_label = QLabel('Press Enter to capture photo', self)
        self.indicator_label.setStyleSheet("color: white; font-size: 16px; font-weight: bold;")
        self.indicator_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(self.indicator_label)

        self.image_label = QLabel(self)
        layout.addWidget(self.image_label)

        self.capture_button = QPushButton('Capture', self)
        self.capture_button.clicked.connect(self.capture_photo)
        self.capture_button.setShortcut(QtCore.Qt.Key_Return)
        layout.addWidget(self.capture_button)

        self.setLayout(layout)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            self.display_image(frame)

    def display_image(self, img):
        out_image = QtGui.QImage(img, img.shape[1], img.shape[0], img.strides[0], QtGui.QImage.Format_RGB888).rgbSwapped()
        self.image_label.setPixmap(QtGui.QPixmap.fromImage(out_image))

    def capture_photo(self):
        ret, frame = self.cap.read()
        if ret:
            cv2.imwrite(os.path.join('users', f"{self.file_name}.jpg"), frame)
            print(f"Photo saved: {self.file_name}.jpg")
            self.cap.release()
            self.close()
            QMessageBox.information(self, "Capture", "Photo captured successfully!")
            self.parent().show()

    def closeEvent(self, event):
        self.cap.release()
        event.accept()

class CaptureWindowForLogin(QWidget):
    def __init__(self, parent, known_face_encodings, known_face_names):
        super().__init__(parent)
        self.known_face_encodings = known_face_encodings
        self.known_face_names = known_face_names
        self.initUI()
        self.cap = cv2.VideoCapture(0)
        if not self.cap.isOpened():
            QMessageBox.critical(self, "Error", "Cannot access camera.")
            self.close()
            return
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_frame)
        self.timer.start(30)
        self.start_time = time.time()

    def initUI(self):
        self.setWindowTitle('Login')
        self.setGeometry(100, 100, 800, 600)

        layout = QVBoxLayout()
        self.indicator_label = QLabel('Recognizing face...', self)
        self.indicator_label.setStyleSheet("color: white; font-size: 16px; font-weight: bold;")
        self.indicator_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(self.indicator_label)

        self.image_label = QLabel(self)
        layout.addWidget(self.image_label)

        self.setLayout(layout)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            self.display_image(frame)
        if time.time() - self.start_time >= 3:
            self.timer.stop()
            username = self.parent().recognize_person(self.cap, self.known_face_encodings, self.known_face_names)
            self.cap.release()
            self.close()
            self.parent().handle_login_result(username)

    def display_image(self, img):
        out_image = QtGui.QImage(img, img.shape[1], img.shape[0], img.strides[0], QtGui.QImage.Format_RGB888).rgbSwapped()
        self.image_label.setPixmap(QtGui.QPixmap.fromImage(out_image))

    def closeEvent(self, event):
        self.cap.release()
        event.accept()

if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    ex = VeloVentureApp()
    ex.show()
    app.exec_()
