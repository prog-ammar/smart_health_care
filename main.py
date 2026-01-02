import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import re
import hospital_backend  # Your pybind11 compiled module

# --- MODERN STYLING ---
QSS_STYLE = """
QMainWindow { background-color: #f8fafc; }
#SideBar { background-color: #1e293b; min-width: 240px; }
#SideBar QPushButton { 
    color: #94a3b8; border: none; padding: 15px 20px; text-align: left; 
    font-size: 14px; border-radius: 8px; margin: 5px 10px; font-weight: 500;
}
#SideBar QPushButton:hover { background-color: #334155; color: white; }
#SideBar QPushButton:checked { background-color: #3b82f6; color: white; font-weight: bold; border-left: 4px solid #60a5fa; }
#ContentArea { background-color: white; border-top-left-radius: 25px; margin-left: 5px; }
QTableWidget { border: 1px solid #e2e8f0; gridline-color: #f1f5f9; alternate-background-color: #f8fafc; }
QHeaderView::section { background-color: #f1f5f9; padding: 12px; border: none; font-weight: bold; color: #475569; }
QGroupBox { font-weight: bold; border: 1px solid #e2e8f0; border-radius: 8px; margin-top: 10px; padding-top: 15px; }
QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #3b82f6; }
QLineEdit, QComboBox, QSpinBox, QTextEdit { padding: 8px; border: 1px solid #cbd5e1; border-radius: 6px; background: white; }
QLineEdit:focus { border: 2px solid #3b82f6; }
QPushButton#ActionBtn { background-color: #3b82f6; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }
QPushButton#ActionBtn:hover { background-color: #2563eb; }
QPushButton#DangerBtn { background-color: #ef4444; color: white; padding: 10px; border-radius: 6px; font-weight: bold; }
QPushButton#SuccessBtn { background-color: #10b981; color: white; padding: 8px; border-radius: 6px; font-weight: bold; }
QLabel#StatLabel { font-size: 28px; font-weight: bold; color: #1e293b; }
QLabel#StatTitle { font-size: 14px; color: #64748b; }
"""

class DoctorBookingDialog(QDialog):
    def __init__(self, parent, hms, patient_id, predicted_disease, ignore_availability=False):
        super().__init__(parent)
        self.hms = hms
        self.p_id = patient_id
        self.condition = predicted_disease
        self.ignore_availability = ignore_availability
        self.setWindowTitle("Assign Doctor")
        self.resize(500, 400)

        layout = QVBoxLayout(self)
        layout.addWidget(QLabel(f"Patient ID: {self.p_id}"))
        layout.addWidget(QLabel(f"<b>AI Diagnosis:</b> {self.condition}"))

        self.doc_combo = QComboBox()
        layout.addWidget(self.doc_combo)

        self.btn_book = QPushButton("Book Appointment & Queue")
        self.btn_book.setObjectName("ActionBtn")
        self.btn_book.clicked.connect(self.finalize_booking)
        layout.addWidget(self.btn_book)

        self.load_doctors()

    def load_doctors(self):
        try:
            doctors = self.hms.get_all_doctors()
            if not doctors:
                self.doc_combo.addItem("No Doctors Registered", None)
                self.btn_book.setEnabled(False)
                return

            for d in doctors:
                # For OPD ignore availability
                if not self.ignore_availability:
                    # Emergency mode → skip unavailable
                    if not d.is_available:
                        continue

                is_match = (d.specialization.lower() == self.condition.lower()) or (d.specialization == "General Physician")
                status_txt = "Available" if d.is_available else "Busy"
                rec_txt = " [RECOMMENDED]" if is_match else ""
                display_text = f"{d.doctor_id} | Dr. {d.first_name} {d.last_name} ({d.specialization}) - {status_txt}{rec_txt}"
                self.doc_combo.addItem(display_text, d.doctor_id)

            # Enable button even if OPD ignores availability
            if self.doc_combo.count() == 0:
                self.doc_combo.addItem("No Doctors Available", None)
                self.btn_book.setEnabled(False)
            else:
                self.btn_book.setEnabled(True)

        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to load doctors: {str(e)}")


    def finalize_booking(self):
        doc_id = self.doc_combo.currentData()
        if not doc_id:
            QMessageBox.warning(self, "Selection Error", "Please select a doctor.")
            return
    
        try:
            # OPD booking ignores doctor's availability
            result = self.hms.Book_Appointment_Workflow(self.p_id, doc_id, True)  # <--- True = ignore availability
    
            if "Success" in result:
                QMessageBox.information(self, "Success", f"{result}\nPatient added to the queue.")
                self.accept()
            else:
                QMessageBox.warning(self, "Booking Failed", result)
        except Exception as e:
            QMessageBox.critical(self, "Backend Error", str(e))


class HospitalManagementSystem(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HMS Pro v3.0 - Integrated Workflow Edition")
        self.resize(1400, 950)
        
        # --- 1. INITIALIZE UNIFIED BACKEND ---
        try:
            self.hms = hospital_backend.Hospital_Management()
            
            # Extract individual handlers
            self.patient_h = self.hms.return_Patient_Handler()
            self.doctor_h = self.hms.return_Doctor_Handler()
            self.appoint_h = self.hms.return_Appoinment_Handler()
            self.billing_h = self.hms.return_Bill_Handler()
            self.recom_s = self.hms.return_Recom_System()

            # Link handlers (Crucial for cross-communication)
            self.appoint_h.set_doctor_handler(self.doctor_h)
            self.appoint_h.set_patient_handler(self.patient_h)
        except Exception as e:
            QMessageBox.critical(self, "Fatal Error", f"Could not initialize Backend: {e}")
            sys.exit(1)

        self.init_ui()
        self.setStyleSheet(QSS_STYLE)

    def init_ui(self):
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        layout = QHBoxLayout(main_widget)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        # --- Sidebar ---
        self.sidebar = QFrame()
        self.sidebar.setObjectName("SideBar")
        side_layout = QVBoxLayout(self.sidebar)
        side_layout.setContentsMargins(10, 20, 10, 20)
        
        logo = QLabel("HOSPITAL OS")
        logo.setAlignment(Qt.AlignCenter)
        logo.setStyleSheet("color: #60a5fa; font-size: 24px; font-weight: 900; padding: 20px 0; letter-spacing: 1px;")
        side_layout.addWidget(logo)

        self.nav_group = QButtonGroup(self)
        menu_items = [
            ("Dashboard", 0), 
            ("Registration Desk", 1),
            ("Doctor Registry", 2),
            ("Appointments", 3), 
            ("Treatment Queue", 4),
            ("Billing & Discharge", 5), 
            ("AI Diagnostics", 6)
        ]

        for text, idx in menu_items:
            btn = QPushButton(text)
            btn.setCheckable(True)
            if idx == 0: btn.setChecked(True)
            btn.clicked.connect(lambda _, i=idx: self.change_page(i))
            self.nav_group.addButton(btn)
            side_layout.addWidget(btn)
        
        side_layout.addStretch()
        layout.addWidget(self.sidebar)

        # --- Main Content ---
        self.stack = QStackedWidget()
        self.stack.setObjectName("ContentArea")
        layout.addWidget(self.stack)

        # Initialize Pages
        self.stack.addWidget(self.create_dashboard())          # 0
        self.stack.addWidget(self.create_registration_page())  # 1
        self.stack.addWidget(self.create_doctor_page())        # 2
        self.stack.addWidget(self.create_appointment_page())   # 3
        self.stack.addWidget(self.create_treat_patients_page())# 4
        self.stack.addWidget(self.create_billing_page())       # 5
        self.stack.addWidget(self.create_ai_page())            # 6

    def change_page(self, idx):
        self.stack.setCurrentIndex(idx)
        # Auto-refresh data when entering pages
        if idx == 0: self.refresh_dashboard()
        if idx == 1: self.refresh_patients()
        if idx == 2: self.refresh_doctors()
        if idx == 3: self.refresh_appointments()
        if idx == 4: self.refresh_queue_tables()
        if idx == 5: self.refresh_billing_page()

    # ==========================
    # PAGE 0: DASHBOARD
    # ==========================
    def create_dashboard(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(40, 40, 40, 40)
        
        layout.addWidget(QLabel("Hospital Overview", font=QFont("Segoe UI", 24, QFont.Bold)))
        
        # Stats Grid
        grid = QGridLayout()
        self.lbl_doc_count = QLabel("0"); self.lbl_doc_count.setObjectName("StatLabel")
        self.lbl_pat_count = QLabel("0"); self.lbl_pat_count.setObjectName("StatLabel")
        self.lbl_inc_count = QLabel("$0"); self.lbl_inc_count.setObjectName("StatLabel")
        self.lbl_em_count = QLabel("0"); self.lbl_em_count.setObjectName("StatLabel")
        
        cards = [
            ("Total Doctors", self.lbl_doc_count),
            ("Total Patients", self.lbl_pat_count),
            ("Total Revenue", self.lbl_inc_count),
            ("Emergency Cases", self.lbl_em_count)
        ]
        
        row, col = 0, 0
        for title, lbl in cards:
            frame = QFrame()
            frame.setStyleSheet("background: white; border-radius: 12px; border: 1px solid #e2e8f0;")
            fl = QVBoxLayout(frame)
            t = QLabel(title); t.setObjectName("StatTitle")
            fl.addWidget(t)
            fl.addWidget(lbl)
            grid.addWidget(frame, row, col)
            col += 1
            if col > 1: col=0; row+=1
            
        layout.addLayout(grid)
        layout.addStretch()
        return page

    def refresh_dashboard(self):
        try:
            docs = len(self.doctor_h.get_all_doctors_vec())
            pats = len(self.patient_h.return_all_opd_patients())
            ems = len(self.patient_h.return_all_emergency_patients())
            bills = self.billing_h.return_all_bills()
            total_rev = sum(b.total_amount for b in bills if b.status == "PAID")
            
            self.lbl_doc_count.setText(str(docs))
            self.lbl_pat_count.setText(str(pats + ems))
            self.lbl_em_count.setText(str(ems))
            self.lbl_inc_count.setText(f"${total_rev}")
        except: pass

    # ==========================
    # PAGE 1: REGISTRATION
    # ==========================
    def create_registration_page(self):
        page = QWidget()
        layout = QHBoxLayout(page)

        # --- Form ---
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        form_widget = QWidget()
        form_layout = QVBoxLayout(form_widget)
        
        # OPD Group
        opd_group = QGroupBox("OPD Registration (Standard)")
        opd_form = QFormLayout(opd_group)
        self.o_name = QLineEdit(); self.o_name.setPlaceholderText("First Name")
        self.o_age = QSpinBox(); self.o_age.setRange(0, 120)
        self.o_gender = QComboBox(); self.o_gender.addItems(["Male", "Female", "Other"])
        self.o_blood = QComboBox(); self.o_blood.addItems(["A+", "A-", "B+", "B-", "O+", "O-", "Unknown"])
        self.o_contact = QLineEdit(); self.o_contact.setPlaceholderText("0300-XXXXXXX")
        self.o_symptoms = QLineEdit(); self.o_symptoms.setPlaceholderText("fever, headache, nausea...")
        
        opd_form.addRow("Name:", self.o_name)
        opd_form.addRow("Age:", self.o_age)
        opd_form.addRow("Gender:", self.o_gender)
        opd_form.addRow("Blood Type:", self.o_blood)
        opd_form.addRow("Contact:", self.o_contact)
        opd_form.addRow("Symptoms:", self.o_symptoms)
        
        reg_btn = QPushButton("Proceed to Doctor Selection")
        reg_btn.setObjectName("ActionBtn")
        reg_btn.clicked.connect(self.submit_opd_workflow)
        opd_form.addRow(reg_btn)
        form_layout.addWidget(opd_group)

        # Emergency Group
        em_group = QGroupBox("Emergency Intake (Priority)")
        em_form = QFormLayout(em_group)
        self.e_name = QLineEdit()
        self.e_age = QSpinBox(); self.e_age.setRange(0, 120)
        self.e_relative = QLineEdit()
        self.e_sens = QSlider(Qt.Horizontal); self.e_sens.setRange(1, 10); self.e_sens.setTickPosition(QSlider.TicksBelow)
        
        em_form.addRow("Patient Name:", self.e_name)
        em_form.addRow("Age:", self.e_age)
        em_form.addRow("Relative Contact:", self.e_relative)
        em_form.addRow("Severity (1-10):", self.e_sens)
        
        # Vitals
        self.v_bp = QSpinBox(); self.v_bp.setRange(0, 300)
        self.v_hr = QSpinBox(); self.v_hr.setRange(0, 250)
        em_form.addRow("BP (sys):", self.v_bp)
        em_form.addRow("Heart Rate:", self.v_hr)
        
        em_btn = QPushButton("ADMIT EMERGENCY NOW")
        em_btn.setObjectName("DangerBtn")
        em_btn.clicked.connect(self.submit_emergency_workflow)
        em_form.addRow(em_btn)
        form_layout.addWidget(em_group)
        
        form_layout.addStretch()
        scroll.setWidget(form_widget)
        layout.addWidget(scroll, 1)


        del_pat_btn = QPushButton("🗑 Delete Selected Patient Record")
        del_pat_btn.setObjectName("DangerBtn")
        del_pat_btn.clicked.connect(self.delete_patient_workflow)
        form_layout.addWidget(del_pat_btn)

        # --- Tables ---
        self.patient_tabs = QTabWidget()
        self.opd_table = QTableWidget(0, 4)
        self.opd_table.setHorizontalHeaderLabels(["ID", "Name", "Department", "Contact"])
        self.opd_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.patient_tabs.addTab(self.opd_table, "OPD Records")
        
        self.em_table = QTableWidget(0, 3)
        self.em_table.setHorizontalHeaderLabels(["ID", "Name", "Severity"])
        self.em_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.patient_tabs.addTab(self.em_table, "Emergency Records")
        
        layout.addWidget(self.patient_tabs, 2)
        return page

    def submit_opd_workflow(self):
    # --- Input validation ---
        is_valid = True
        name = self.o_name.text().strip()
        age = self.o_age.value()
        contact = self.o_contact.text().strip()
        symptoms_txt = self.o_symptoms.text().strip()

        if not name:
            QMessageBox.warning(self, "Input Error", "Name is required.")
            is_valid = False
        elif not self.is_valid_name(name):
            QMessageBox.warning(self, "Input Error", "Name must contain letters only.")
            is_valid = False

        if age <= 0:
            QMessageBox.warning(self, "Input Error", "Age must be greater than 0.")
            is_valid = False

        if not contact:
            QMessageBox.warning(self, "Input Error", "Contact number is required.")
            is_valid = False
        elif not self.is_valid_phone(contact):
            QMessageBox.warning(self, "Input Error", "Contact must be like 03001234567.")
            is_valid = False

        if not symptoms_txt:
            QMessageBox.warning(self, "Input Error", "Please enter symptoms.")
            is_valid = False

        if not is_valid:
            return  # ❌ Do not call backend

        # --- Process Symptoms ---
        symptom_list = [s.strip().lower() for s in symptoms_txt.split(",") if s.strip()]

        try:
            # Calls C++ workflow
            res = self.hms.Register_OPD_Workflow(
                name,
                age,
                self.o_blood.currentText(),
                self.o_gender.currentText(),
                contact,
                symptom_list
            )

            if res.success:
                # OPD patients: assign doctor but ignore availability
                dlg = DoctorBookingDialog(self, self.hms, res.patient_id, res.predicted_disease, ignore_availability=True)
                if dlg.exec_() == QDialog.Accepted:
                    self.o_name.clear()
                    self.o_contact.clear()
                    self.o_symptoms.clear()
                    self.refresh_patients()
            else:
                QMessageBox.warning(self, "Error", "OPD registration failed.")

        except Exception as e:
            QMessageBox.critical(self, "Error", str(e))



    def is_valid_name(self, name):
        return bool(re.fullmatch(r"[A-Za-z ]{2,}", name))

    def is_valid_phone(self, phone):
        return bool(re.fullmatch(r"03\d{9}", phone))

    # -----------------------------
# EMERGENCY WORKFLOW
# -----------------------------
    def submit_emergency_workflow(self):
        is_valid = True

        name = self.e_name.text().strip()
        age = self.e_age.value()
        contact = self.e_relative.text().strip()
        severity = self.e_sens.value()
        bp = self.v_bp.value()
        hr = self.v_hr.value()

        if not name:
            QMessageBox.warning(self, "Input Error", "Patient name is required.")
            is_valid = False
        elif not self.is_valid_name(name):
            QMessageBox.warning(self, "Input Error", "Invalid patient name.")
            is_valid = False

        if age <= 0:
            QMessageBox.warning(self, "Input Error", "Age must be greater than 0.")
            is_valid = False

        if not self.is_valid_phone(contact):
            QMessageBox.warning(self, "Input Error", "Relative contact must be valid.")
            is_valid = False

        if severity < 5:
            QMessageBox.warning(self, "Input Error", "Emergency severity must be 5 or higher.")
            is_valid = False

        if bp < 60 or bp > 250:
            QMessageBox.warning(self, "Input Error", "Blood pressure value is abnormal.")
            is_valid = False

        if hr < 40 or hr > 200:
            QMessageBox.warning(self, "Input Error", "Heart rate value is abnormal.")
            is_valid = False

        if not is_valid:
            return  # stops backend call

        symps = hospital_backend.PatientSymptoms()
        symps.bloodPressure = bp
        symps.heartRate = hr

        try:
            res_str = self.hms.Admit_Emergency_Workflow(
                name,
                age,
                "Unknown",
                "Unknown",
                contact,
                severity,
                symps
            )

            QMessageBox.critical(self, "🚨 EMERGENCY ADMITTED", res_str)
            self.e_name.clear()
            self.refresh_patients()

        except Exception as e:
            QMessageBox.critical(self, "Error", str(e))


    def refresh_patients(self):
        # OPD
        try:
            opds = self.patient_h.return_all_opd_patients()
            self.opd_table.setRowCount(0)
            for p in opds:
                r = self.opd_table.rowCount(); self.opd_table.insertRow(r)
                self.opd_table.setItem(r, 0, QTableWidgetItem(p.id))
                self.opd_table.setItem(r, 1, QTableWidgetItem(p.name))
                self.opd_table.setItem(r, 2, QTableWidgetItem(p.department))
                self.opd_table.setItem(r, 3, QTableWidgetItem(p.contact_number))
            
            # Emergency
            ems = self.patient_h.return_all_emergency_patients()
            self.em_table.setRowCount(0)
            for p in ems:
                r = self.em_table.rowCount(); self.em_table.insertRow(r)
                self.em_table.setItem(r, 0, QTableWidgetItem(p.id))
                self.em_table.setItem(r, 1, QTableWidgetItem(p.name))
                sev_item = QTableWidgetItem(str(p.sensitivity))
                if p.sensitivity > 7: sev_item.setBackground(QColor("#fee2e2"))
                self.em_table.setItem(r, 2, sev_item)
        except Exception as e:
            print(f"Error refreshing patients: {e}")


    def delete_patient_workflow(self):
        """Deletes patient based on which Tab (OPD or Emergency) is active"""
        current_tab_idx = self.patient_tabs.currentIndex()
        
        if current_tab_idx == 0:
            # OPD Tab is active
            table = self.opd_table
            patient_type = "OPD"
        else:
            # Emergency Tab is active
            table = self.em_table
            patient_type = "Emergency"

        row = table.currentRow()
        if row < 0:
            QMessageBox.warning(self, "Selection Error", f"Please select an {patient_type} patient to delete.")
            return

        p_id = table.item(row, 0).text()
        p_name = table.item(row, 1).text()

        confirm = QMessageBox.question(
            self, "Confirm Deletion", 
            f"Are you sure you want to delete {patient_type} Patient: {p_name} ({p_id})?",
            QMessageBox.Yes | QMessageBox.No
        )

        if confirm == QMessageBox.Yes:
            try:
                # Calls C++ function: remove_patient(string id)
                # Ensure this function exists in your C++ Patient_Handler
                self.patient_h.remove_patient(p_id)
                
                QMessageBox.information(self, "Success", "Patient record deleted.")
                self.refresh_patients() # Refresh list
            except Exception as e:
                QMessageBox.critical(self, "Backend Error", f"Error removing patient: {str(e)}")

    # ==========================
    # PAGE 2: DOCTOR REGISTRY
    # ==========================
    def create_doctor_page(self):
        page = QWidget(); layout = QVBoxLayout(page)
        
        top = QHBoxLayout()
        top.addWidget(QLabel("Medical Staff Directory", font=QFont("Segoe UI", 16, QFont.Bold)))
        add_btn = QPushButton("+ Register New Doctor")
        add_btn.setObjectName("ActionBtn")
        add_btn.clicked.connect(self.add_doctor_dialog)
        top.addWidget(add_btn)
        layout.addLayout(top)

        del_btn = QPushButton("🗑 Delete Doctor")
        del_btn.setObjectName("DangerBtn") # Uses your existing red style
        del_btn.clicked.connect(self.delete_doctor_workflow)
        top.addWidget(del_btn)

        layout.addLayout(top)
        
        self.doc_table = QTableWidget(0, 5)
        self.doc_table.setHorizontalHeaderLabels(["ID", "Name", "Specialization", "Branch", "Status"])
        self.doc_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        layout.addWidget(self.doc_table)
        return page
    
    def add_doctor_dialog(self):
        d = QDialog(self)
        d.setWindowTitle("Add Doctor")
        f = QFormLayout(d)

        fn = QLineEdit(); ln = QLineEdit(); spec = QLineEdit(); ph = QLineEdit()
        yr = QSpinBox(); yr.setRange(0, 50)  # optional: experience range
        br = QLineEdit(); em = QLineEdit()

        f.addRow("First Name:", fn)
        f.addRow("Last Name:", ln)
        f.addRow("Specialization:", spec)
        f.addRow("Phone:", ph)
        f.addRow("Experience (Yrs):", yr)
        f.addRow("Branch:", br)
        f.addRow("Email:", em)

        btn = QPushButton("Save")
        btn.setObjectName("ActionBtn")
        btn.clicked.connect(lambda: self.save_doctor(d, fn, ln, spec, ph, yr, br, em))
        f.addRow(btn)

        d.exec_()


    def save_doctor(self, d, fn, ln, spec, ph, yr, br, em):
        is_valid = True
    
        first = fn.text().strip()
        last = ln.text().strip()
        specialization = spec.text().strip()
        phone = ph.text().strip()
        experience = yr.value()
        branch = br.text().strip()
        email = em.text().strip()
    
        # --- Validation ---
        if not first:
            QMessageBox.warning(d, "Input Error", "First name is required.")
            is_valid = False
        elif not re.fullmatch(r"[A-Za-z ]{2,}", first):
            QMessageBox.warning(d, "Input Error", "First name must contain letters only.")
            is_valid = False
    
        if not last:
            QMessageBox.warning(d, "Input Error", "Last name is required.")
            is_valid = False
        elif not re.fullmatch(r"[A-Za-z ]{2,}", last):
            QMessageBox.warning(d, "Input Error", "Last name must contain letters only.")
            is_valid = False
    
        if not specialization:
            QMessageBox.warning(d, "Input Error", "Specialization is required.")
            is_valid = False
    
        if not phone:
            QMessageBox.warning(d, "Input Error", "Phone number is required.")
            is_valid = False
        elif not re.fullmatch(r"03\d{9}", phone):
            QMessageBox.warning(d, "Input Error", "Phone must be like 03001234567.")
            is_valid = False
    
        if experience <= 0:
            QMessageBox.warning(d, "Input Error", "Experience must be greater than 0.")
            is_valid = False
    
        if not branch:
            QMessageBox.warning(d, "Input Error", "Branch is required.")
            is_valid = False
    
        if not email or "@" not in email:
            QMessageBox.warning(d, "Input Error", "Valid email is required.")
            is_valid = False
    
        # --- Stop if invalid ---
        if not is_valid:
            return  # ❌ backend not called
    
        # --- Call backend safely ---
        try:
            self.hms.add_Doctor(first, last, specialization, phone, experience, branch, email)
            QMessageBox.information(d, "Success", "Doctor added successfully!")
            self.refresh_doctors()
            d.accept()  # close the dialog
        except Exception as e:
            QMessageBox.warning(d, "Error", str(e))


    def delete_doctor_workflow(self):
        """Deletes the selected doctor from the backend and updates UI"""
        row = self.doc_table.currentRow()
        if row < 0:
            QMessageBox.warning(self, "Selection Error", "Please select a doctor to delete.")
            return

        # Get Doctor ID from the first column (index 0)
        doc_id = self.doc_table.item(row, 0).text()
        doc_name = self.doc_table.item(row, 1).text()

        confirm = QMessageBox.question(
            self, "Confirm Deletion", 
            f"Are you sure you want to remove Dr. {doc_name} ({doc_id})?\nThis cannot be undone.",
            QMessageBox.Yes | QMessageBox.No
        )

        if confirm == QMessageBox.Yes:
            try:
                # Calls C++ function: remove_doctor(string id)
                # You must ensure this function exists in your C++ Doctor_Handler
                success = self.doctor_h.remove_doctor(doc_id) 
                
                # Assuming C++ returns a boolean or string. Adjust check accordingly:
                if success:
                    QMessageBox.information(self, "Success", f"Doctor {doc_id} removed successfully.")
                    self.refresh_doctors()
                else:
                    QMessageBox.warning(self, "Failed", "Backend could not remove the doctor.")
            except Exception as e:
                QMessageBox.critical(self, "Backend Error", f"Function not found or error: {str(e)}\n\n(Ensure 'remove_doctor' is exposed in pybind11)")

    def refresh_doctors(self):
        try:
            docs = self.doctor_h.get_all_doctors_vec()
            self.doc_table.setRowCount(0)
            for d in docs:
                r = self.doc_table.rowCount(); self.doc_table.insertRow(r)
                self.doc_table.setItem(r, 0, QTableWidgetItem(d.doctor_id))
                self.doc_table.setItem(r, 1, QTableWidgetItem(f"{d.first_name} {d.last_name}"))
                self.doc_table.setItem(r, 2, QTableWidgetItem(d.specialization))
                self.doc_table.setItem(r, 3, QTableWidgetItem(d.hospital_branch))
                st = "Available" if d.is_available else "Off Duty"
                self.doc_table.setItem(r, 4, QTableWidgetItem(st))
        except: pass

    # ==========================
    # PAGE 4: TREATMENT QUEUE
    # ==========================
    def create_treat_patients_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        
        # --- 1. EMERGENCY SECTION (Top Priority) ---
        em_group = QGroupBox("🚨 EMERGENCY QUEUE (Critical Priority)")
        # Add a red border style to emphasize emergency
        em_group.setStyleSheet("QGroupBox { border: 2px solid #ef4444; font-weight: bold; } QGroupBox::title { color: #ef4444; }")
        em_layout = QVBoxLayout(em_group)

        # Header & Button
        h_em = QHBoxLayout()
        h_em.addWidget(QLabel("Critical Care Unit", font=QFont("Segoe UI", 12)))
        
        btn_treat_em = QPushButton("TREAT EMERGENCY PATIENT")
        btn_treat_em.setObjectName("DangerBtn") # Uses the red styling
        btn_treat_em.clicked.connect(self.treat_emergency_patient)
        h_em.addWidget(btn_treat_em)
        em_layout.addLayout(h_em)

        # Emergency Table
        self.em_q_table = QTableWidget(0, 4)
        self.em_q_table.setHorizontalHeaderLabels(["ID", "Name", "Severity (1-10)", "Vitals"])
        self.em_q_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        em_layout.addWidget(self.em_q_table)
        
        layout.addWidget(em_group)

        # --- 2. OPD SECTION (Standard) ---
        opd_group = QGroupBox("Standard OPD Queue")
        opd_layout = QVBoxLayout(opd_group)
        
        # Header & Button
        h_opd = QHBoxLayout()
        h_opd.addWidget(QLabel("General Waiting Room", font=QFont("Segoe UI", 12)))
        
        btn_treat_opd = QPushButton("Call Next OPD Patient")
        btn_treat_opd.setObjectName("SuccessBtn") # Uses the green/blue styling
        btn_treat_opd.clicked.connect(self.treat_opd_patient)
        h_opd.addWidget(btn_treat_opd)
        opd_layout.addLayout(h_opd)

        # OPD Table
        self.q_table = QTableWidget(0, 4)
        self.q_table.setHorizontalHeaderLabels(["Token", "Patient Name", "Department", "Status"])
        self.q_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        opd_layout.addWidget(self.q_table)
        
        layout.addWidget(opd_group)

        # --- FIX: Initial Load ---
        # This forces the tables to populate immediately when the page is created
        self.refresh_queue_tables()

        return page

    # -----------------------------
# QUEUE LOGIC
# -----------------------------
    def refresh_queue_tables(self):
    # OPD Queue (FIFO)
        try:
            queue_vec = self.patient_h.get_opd_queue()
            self.q_table.setRowCount(0)
            for p in queue_vec:
                r = self.q_table.rowCount(); self.q_table.insertRow(r)
                self.q_table.setItem(r, 0, QTableWidgetItem(p.id))
                self.q_table.setItem(r, 1, QTableWidgetItem(p.name))
                self.q_table.setItem(r, 2, QTableWidgetItem(p.department))
                item_status = QTableWidgetItem("Waiting")
                item_status.setForeground(QColor("orange"))
                self.q_table.setItem(r, 3, item_status)
        except Exception as e:
            print(f"OPD Queue Refresh Error: {e}")

        # Emergency Queue (Priority)
        try:
            em_vec = self.patient_h.get_emergency_queue()
            self.em_q_table.setRowCount(0)
            for p in em_vec:
                r = self.em_q_table.rowCount(); self.em_q_table.insertRow(r)
                self.em_q_table.setItem(r, 0, QTableWidgetItem(p.id))
                self.em_q_table.setItem(r, 1, QTableWidgetItem(p.name))
                sev_item = QTableWidgetItem(str(p.sensitivity))
                if p.sensitivity >= 8:
                    sev_item.setBackground(QColor("#fee2e2"))
                    sev_item.setFont(QFont("Segoe UI", 9, QFont.Bold))
                self.em_q_table.setItem(r, 2, sev_item)
                vitals = f"BP: {p.symptoms.bloodPressure} | HR: {p.symptoms.heartRate}"
                self.em_q_table.setItem(r, 3, QTableWidgetItem(vitals))
        except Exception as e:
            print(f"Emergency Queue Refresh Error: {e}")


    def treat_opd_patient(self):
        queue_vec = self.patient_h.get_opd_queue()
        if not queue_vec:
            QMessageBox.information(self, "Queue Empty", "No OPD patients waiting.")
            return

        try:
            msg = self.hms.Treat_OPD_Patient()
            QMessageBox.information(self, "OPD Treatment", f"{msg}\nPatient removed from active queue.")
            self.refresh_queue_tables()
        except Exception as e:
            QMessageBox.critical(self, "Error", str(e))

    def treat_emergency_patient(self):
        em_vec = self.patient_h.get_emergency_queue()
        if not em_vec:
            QMessageBox.information(self, "Queue Empty", "No Emergency patients waiting.")
            return

        try:
            # Calls the C++ function to pop the highest priority patient
            msg = self.hms.Treat_Emergency_Patient()
            QMessageBox.warning(self, "Emergency Cleared", f"{msg}\nPatient treated and moved to discharge/observation.")
            self.refresh_queue_tables()
        except Exception as e:
            QMessageBox.critical(self, "Error", str(e))

    # ==========================
    # PAGE 5: BILLING
    # ==========================
    def create_billing_page(self):
        page = QWidget(); layout = QHBoxLayout(page)
        
        # Table
        grp = QGroupBox("Financial Records"); gl = QVBoxLayout(grp)
        self.bill_table = QTableWidget(0, 6)
        self.bill_table.setHorizontalHeaderLabels(["PID", "Name", "Type", "Status", "Total", "Date"])
        self.bill_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        gl.addWidget(self.bill_table)
        
        ref_btn = QPushButton("Refresh"); ref_btn.clicked.connect(self.refresh_billing_page)
        gl.addWidget(ref_btn)
        layout.addWidget(grp, 2)
        
        # Actions
        actions = QWidget(); al = QVBoxLayout(actions)
        
        p_grp = QGroupBox("Pharmacy / Lab"); pl = QFormLayout(p_grp)
        self.b_pid = QLineEdit(); self.b_item = QLineEdit(); self.b_qty = QSpinBox()
        pl.addRow("Patient ID:", self.b_pid); pl.addRow("Item Name:", self.b_item); pl.addRow("Qty:", self.b_qty)
        p_btn = QPushButton("Add Charge"); p_btn.clicked.connect(self.perform_prescription); pl.addRow(p_btn)
        al.addWidget(p_grp)
        
        d_grp = QGroupBox("Checkout"); dl = QVBoxLayout(d_grp)
        self.d_pid = QLineEdit(); self.d_pid.setPlaceholderText("Patient ID")
        dl.addWidget(self.d_pid)
        d_btn = QPushButton("Pay & Discharge"); d_btn.setObjectName("SuccessBtn")
        d_btn.clicked.connect(self.perform_discharge)
        dl.addWidget(d_btn)
        al.addWidget(d_grp)
        
        al.addStretch()
        layout.addWidget(actions, 1)
        return page

    def perform_prescription(self):
        pid = self.b_pid.text()
        item = self.b_item.text()
        qty = self.b_qty.value()

        if not pid or not item: 
            return

        try:
            # Check if the patient is already discharged/paid
            bills = self.billing_h.return_all_bills()
            # Find the specific patient's bill
            patient_bill = next((b for b in bills if b.patient_id == pid), None)

            if patient_bill and patient_bill.status == "PAID":
                QMessageBox.warning(self, "Action Denied", 
                                    f"Cannot add charges. Patient {pid} has already been discharged.")
                return

            res = self.hms.Prescribe_Medication(pid, item, qty)
            QMessageBox.information(self, "Result", res)
            self.refresh_billing_page()
        except Exception as e: 
            QMessageBox.warning(self, "Error", str(e))

    
    def perform_discharge(self):
        pid = self.d_pid.text()
        if not pid: return
        
        try:
            # Fetch current status
            bills = self.billing_h.return_all_bills()
            patient_bill = next((b for b in bills if b.patient_id == pid), None)
    
            if patient_bill and patient_bill.status == "PAID":
                QMessageBox.information(self, "Notice", "This patient is already discharged.")
                return
    
            res = self.hms.Discharge_Patient(pid)
            QMessageBox.information(self, "Discharge", res)
            self.refresh_billing_page()
        except Exception as e: 
            QMessageBox.warning(self, "Error", str(e))

    

    def refresh_billing_page(self):
        try:
            bills = self.billing_h.return_all_bills()
            self.bill_table.setRowCount(0)
            for b in bills:
                r = self.bill_table.rowCount(); self.bill_table.insertRow(r)
                self.bill_table.setItem(r, 0, QTableWidgetItem(b.patient_id))
                self.bill_table.setItem(r, 1, QTableWidgetItem(b.patient_name))
                self.bill_table.setItem(r, 2, QTableWidgetItem(b.bill_type))
                
                st = QTableWidgetItem(b.status)
                st.setForeground(QColor("green") if b.status == "PAID" else QColor("red"))
                self.bill_table.setItem(r, 3, st)
                
                self.bill_table.setItem(r, 4, QTableWidgetItem(str(b.total_amount)))
                self.bill_table.setItem(r, 5, QTableWidgetItem(b.created_at))
        except: pass

    # ==========================
    # OTHER PAGES
    # ==========================
    def create_appointment_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)  # Single main layout

        # --- Top Header & Actions ---
        top_bar = QHBoxLayout()
        
        # Title
        title = QVBoxLayout()
        title.addWidget(QLabel("Appointment Management", font=QFont("Segoe UI", 18, QFont.Bold)))
        title.addWidget(QLabel("Manage schedules and visit statuses", font=QFont("Segoe UI", 10)))
        top_bar.addLayout(title)
        
        top_bar.addStretch()
        
        # Action Buttons
        btn_complete = QPushButton("✓ Mark Completed")
        btn_complete.setStyleSheet("background-color: #10b981; color: white; padding: 10px; border-radius: 6px; font-weight: bold;")
        btn_complete.clicked.connect(self.mark_appointment_complete)
        top_bar.addWidget(btn_complete)

        btn_cancel = QPushButton("✕ Cancel")
        btn_cancel.setStyleSheet("background-color: #ef4444; color: white; padding: 10px; border-radius: 6px; font-weight: bold;")
        btn_cancel.clicked.connect(self.cancel_appointment)
        top_bar.addWidget(btn_cancel)

        # Spacer
        top_bar.addSpacing(20)

        # Book Button
        btn_book = QPushButton("+ Book New")
        btn_book.setObjectName("ActionBtn")
        btn_book.clicked.connect(self.open_booking_dialog)
        top_bar.addWidget(btn_book)
        
        layout.addLayout(top_bar)

        # --- Table ---
        self.app_table = QTableWidget(0, 5)
        self.app_table.setHorizontalHeaderLabels(["Appt ID", "Patient ID", "Doctor ID", "Time", "Status"])
        self.app_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.app_table.setSelectionBehavior(QAbstractItemView.SelectRows) # Select full row
        
        layout.addWidget(self.app_table)
        
        return page

    # --- STATUS UPDATE LOGIC ---

    def mark_appointment_complete(self):
        """Marks selected appointment as Completed"""
        row = self.app_table.currentRow()
        if row < 0:
            QMessageBox.warning(self, "Selection Error", "Please select an appointment from the table first.")
            return

        appt_id = self.app_table.item(row, 0).text()
        status = self.app_table.item(row, 4).text()

        if status == "Completed":
            QMessageBox.information(self, "Info", "This appointment is already completed.")
            return

        confirm = QMessageBox.question(self, "Confirm", f"Mark appointment {appt_id} as COMPLETED?", QMessageBox.Yes | QMessageBox.No)
        if confirm == QMessageBox.Yes:
            try:
                # Call the new C++ function
                self.appoint_h.complete_appointment(appt_id) 
                self.refresh_appointments() # Refresh table to show changes
                QMessageBox.information(self, "Success", "Appointment marked as Completed.")
            except Exception as e:
                QMessageBox.critical(self, "Backend Error", str(e))
        self.refresh_doctors()

    def cancel_appointment(self):
        """Cancels selected appointment"""
        row = self.app_table.currentRow()
        if row < 0:
            QMessageBox.warning(self, "Selection Error", "Please select an appointment from the table first.")
            return

        appt_id = self.app_table.item(row, 0).text()
        
        confirm = QMessageBox.question(self, "Confirm", f"Are you sure you want to CANCEL appointment {appt_id}?", QMessageBox.Yes | QMessageBox.No)
        if confirm == QMessageBox.Yes:
            try:
                # Call existing C++ function
                self.appoint_h.cancel_appointment(appt_id)
                self.refresh_appointments()
                QMessageBox.information(self, "Cancelled", "Appointment has been cancelled.")
            except Exception as e:
                QMessageBox.critical(self, "Backend Error", str(e))
        self.refresh_doctors()
    
    def open_booking_dialog(self):
        """Dialog to select Patient and Doctor then call Workflow"""
        diag = QDialog(self)
        diag.setWindowTitle("Book Appointment")
        form = QFormLayout(diag)
        
        p_id = QLineEdit(); p_id.setPlaceholderText("e.g. O1")
        d_id = QLineEdit(); d_id.setPlaceholderText("e.g. D1")
        
        form.addRow("Patient ID:", p_id)
        form.addRow("Doctor ID:", d_id)
        
        btn = QPushButton("Confirm Booking")
        btn.setObjectName("ActionBtn")
        def confirm():
            # CALL UNIFIED C++ WORKFLOW
            res = self.hms.Book_Appointment_Workflow(p_id.text(), d_id.text())
            if "Success" in res:
                QMessageBox.information(diag, "Booked", res)
                self.refresh_appointments()
                self.refresh_billing_page() # Consultation fee added automatically
                diag.accept()
            else:
                QMessageBox.warning(diag, "Booking Failed", res)
        
        self.refresh_doctors()
        
        btn.clicked.connect(confirm)
        form.addRow(btn)
        diag.exec_()


    def refresh_appointments(self):
        try:
            apps = self.appoint_h.get_all_appointments_vec()
            self.app_table.setRowCount(0)
            for a in apps:
                r = self.app_table.rowCount(); self.app_table.insertRow(r)
                self.app_table.setItem(r, 0, QTableWidgetItem(a.appointment_id))
                self.app_table.setItem(r, 1, QTableWidgetItem(a.patient_id))
                self.app_table.setItem(r, 2, QTableWidgetItem(a.doctor_id))
                self.app_table.setItem(r, 3, QTableWidgetItem(a.appointment_time))
                self.app_table.setItem(r, 4, QTableWidgetItem(a.status))
        except: pass

    def create_ai_page(self):
        page = QWidget(); l = QVBoxLayout(page)
        l.addWidget(QLabel("AI Diagnostic Assistant", font=QFont("Segoe UI", 18, QFont.Bold)))
        
        self.ai_input = QTextEdit(); self.ai_input.setPlaceholderText("Enter symptoms here...")
        l.addWidget(self.ai_input)
        
        btn = QPushButton("Analyze Symptoms"); btn.setObjectName("ActionBtn")
        btn.clicked.connect(self.run_ai_tool)
        l.addWidget(btn)
        
        self.ai_out = QTextBrowser()
        l.addWidget(self.ai_out)
        return page

    def run_ai_tool(self):
        txt = self.ai_input.toPlainText()
        if not txt: return
        try:
            s_list = [s.strip() for s in txt.split(",") if s.strip()]
            disease = self.recom_s.get_condition(s_list)
            prec = self.recom_s.get_precautions(disease)
            meds = self.recom_s.get_medicines(disease)
            
            html = f"<h2>Possible Condition: <span style='color:red'>{disease}</span></h2>"
            html += "<h3>Precautions:</h3><ul>" + "".join([f"<li>{x}</li>" for x in prec]) + "</ul>"
            html += "<h3>Medicines:</h3><ul>" + "".join([f"<li>{x}</li>" for x in meds]) + "</ul>"
            self.ai_out.setHtml(html)
        except Exception as e: self.ai_out.setText(f"Error: {e}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setFont(QFont("Segoe UI", 10))
    window = HospitalManagementSystem()
    window.show()
    sys.exit(app.exec_())