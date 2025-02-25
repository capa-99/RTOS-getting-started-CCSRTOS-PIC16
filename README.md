# RTOS-getting-started-CCSRTOS-PIC16

# 1. RTOS-Based Stopwatch using PIC16F84A and CCS RTOS

## Project Overview
This project implements a **digital stopwatch** on a **PIC16F84A microcontroller** using **CCS RTOS**. The stopwatch features **start, stop, and reset functionality** while demonstrating the benefits of using **RTOS in embedded systems**.

## RTOS Usage in This Project
### **Task Management**
- The project uses **multiple RTOS tasks** to handle different aspects of the stopwatch, such as:
  - Counting time
  - Display updates
  - Button handling

### **Task Scheduling**
- The **RTOS scheduler ensures precise timing** for incrementing the stopwatch counter.
- Tasks run at **defined time intervals**, eliminating the need for manual delays.

###  **Concurrency Handling**
- The RTOS allows the stopwatch to **simultaneously update the display and respond to user inputs**.
- **No blocking delays**—tasks execute independently as needed.

## Key Advantages of Using RTOS
✔ **Accurate timing**: The scheduler ensures that the stopwatch increments correctly.  
✔ **Better task separation**: Input handling, display updates, and counting are managed independently.  
✔ **Efficient CPU usage**: The processor isn't stuck in loops waiting for input.  

This project serves as a **beginner-friendly example** of using **CCS RTOS with a PIC16F84A** to manage real-time events efficiently.

![PIC16 CCSRTOS Stopwatch Demo](https://github.com/user-attachments/assets/99da0364-3f0e-4a50-9686-30c6b6171c22)


# 2. RTOS-Based Elevator Simulator using PIC16F84A and CCS RTOS

## Project Overview
This project implements a **basic elevator simulation** on a **PIC16F84A microcontroller** using **CCS RTOS**. The system controls **button inputs, elevator movement, and floor indication**, demonstrating how an **RTOS can efficiently manage sequential operations** in real-time.

## RTOS Usage in This Project
### **Task Management**
- The system uses **multiple RTOS tasks** to handle:
  - Button input processing
  - Elevator movement logic
  - Floor indication updates

### **Task Scheduling**
- The **RTOS scheduler ensures proper timing** between tasks.
- Each task runs at a **specific rate**, preventing blocking delays.

### **Task Execution Flow**
- The **user selects a floor when the elevator is idle**.
- The elevator **moves toward the selected floor, one step at a time**.
- While moving, **no new floor selections are accepted**.
- Once the elevator reaches the destination, it **becomes idle again**, ready for a new command.

## Key Advantages of Using RTOS
✔ **Structured execution**: Movement, input handling, and floor updates run in separate tasks.  
✔ **Efficient processing**: The RTOS prevents unnecessary delays while keeping task execution controlled.  

This project serves as a **real-world example of using RTOS for sequential control systems**, showing how **PIC16F84A and CCS RTOS** can manage real-time processes efficiently.

![PIC16 CCSRTOS Elevator Demo](https://github.com/user-attachments/assets/2dc2f989-dd8f-45ca-bdd8-d3c0b5822aef)


# 3. RTOS-Based Oven Control System using PIC16F870 and CCS RTOS

## Project Overview
This project implements a **basic oven control system** on a **PIC16F870 microcontroller** using **CCS RTOS**. The system manages **temperature regulation, heating control, and user input handling**, demonstrating how an **RTOS can efficiently coordinate multiple real-time tasks**.

## RTOS Usage in This Project
### **Task Management**
- The system uses **multiple RTOS tasks** to handle:
  - Temperature measurement and regulation
  - Heater activation and deactivation
  - User input processing (temperature and timer settings)
  - Display updates for real-time monitoring

### **Task Scheduling**
- The **RTOS scheduler ensures precise timing** for temperature sampling and heater control.
- Each task runs at a **fixed interval**, preventing blocking delays.

### **Task Execution Flow**
- The user **sets the desired temperature and timer**.
- The system **monitors and regulates temperature** using a heater.
- **Heaters turn on/off automatically** to maintain the set temperature.
- A **timer tracks the cooking duration**, turning off the oven when finished.

## Key Advantages of Using RTOS
✔ **Accurate temperature regulation**: The oven adjusts heating dynamically without delays.  
✔ **Independent task execution**: Temperature control, timer, and user inputs work simultaneously.  
✔ **Energy efficiency**: The RTOS prevents unnecessary heating by switching the heater only when needed.  

This project serves as a **real-world example of RTOS usage in temperature-controlled systems**, showcasing how **PIC16F870 and CCS RTOS** can efficiently manage real-time processes.

![PIC16 CCSRTOS Oven Demo](https://github.com/user-attachments/assets/5f60f531-2117-499b-8a1e-6a434efb6b1c)


# Running projects and reviewing code
Open .pdsprj files in Proteus
