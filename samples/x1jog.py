import APEXDirectPySDK as a
import time

transport = a.gantry.Transport()
transport.connect("192.168.2.102", 23)

print("Connected")

x1 = a.gantry.Axis(transport)
x1.configure(
    "module_idx: 1\n"       # 1...7 modules
    "inverted: true\n"      # invert the motor
    "current: 3\n"          # set motor current = 3A
    "unit_per_rev: 15\n"    # set 1 revolution = 15 mm
    "bounds: [0, 2000]\n"   # bound the axis from home (0) to +2m
    "speed_unit_per_s: 30\n"# speed = 30 mm/s
    "accel_unit_per_s2: 15" # accel = 15 mm/s/s
)

print("Configured axis")

x1.setMotor(True)
x1.forceCurrentLoc(0)   # home the axis
time.sleep(1)
x1.moveTo(400)          # move +400 mm
time.sleep(20)
print(x1.fetchCurrentLoc()) # --> (error_code, encoderLoc, motorLoc)
                            # should give (0, 400, 400)
time.sleep(1)
x1.moveBy(-200)         # move 200 mm back
time.sleep(15)
print(x1.fetchCurrentLoc()) # should give (0, 200, 200)
