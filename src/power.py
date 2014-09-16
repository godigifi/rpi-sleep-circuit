import smbus
import time
from subprocess import call
import sys
import logging
logger = logging.getLogger(__name__)

"""
Sample interface for sending shutdown or sleep request
via i2c bus to the trinket-controlled power regulator.
"""

SLEEP_STATE = 1
SHUTDOWN_STATE = 2
DEV_ADDR = 0x04 # I2c address

def __invoke_sleep(state, delay_before_shutdown, resume_in_seconds):
    """
    Request shutting down power in specified state. 
    If state == SHUTDOWN_STATE, the power is turned off from Raspberry Pi and the trinket
    after specified delay.
    If state == SLEEP_STATE, the power is turned off from Raspberry Pi and 
    resumed back on after specified time interval. 
    """
    bus = smbus.SMBus(1)
    # Request shutdown
    bus.write_i2c_block_data(DEV_ADDR,0,[state, delay_before_shutdown, 
        resume_in_seconds & 255, 
        resume_in_seconds >> 8,
        0,0]) 
    # Verify result
    data = read_power_controller_state()
    if data[0] == state and data[1] > 0:
        # Shutdown system
        call(['sudo', 'shutdown', '-h', '0'])
        return True
    else:
        return False

def enter_sleep_mode(delay_before_shutdown, resume_in_seconds):
    try:
        logger.log("Going to sleep for %s seconds after %s delay.", 
            resume_in_seconds, delay_before_shutdown)
        return __invoke_sleep(SLEEP_STATE, 
            delay_before_shutdown, resume_in_seconds)
    except:
        logger.exception("Entering sleep state sleep failed.")
        return False

def shutdown(delay_before_shutdown):
    try:
        logger.log("Going to shut down after %s delay.", 
            delay_before_shutdown)
        return __invoke_sleep(SHUTDOWN_STATE, delay_before_shutdown, 0)
    except:
        logger.exception("Shutting down failed.")
        return False


def read_power_controller_state():
    bus = smbus.SMBus(1)
    bus.write_byte(DEV_ADDR, 0)
    b0 = bus.read_byte(DEV_ADDR) # Controller state 
    b1 = bus.read_byte(DEV_ADDR) # Delay before shutdown
    b2 = bus.read_byte(DEV_ADDR) # Sleep timer low byte
    b3 = bus.read_byte(DEV_ADDR) # Sleep timer high byte
    b4 = bus.read_byte(DEV_ADDR) # Current sensor low byte
    b5 = bus.read_byte(DEV_ADDR) # Current sensor high byte
    timer = (b3 << 8) + b2  # Convert timer bytes to integer
    value = (b5 << 8) + b4  # Convert current sensor value to integer
    return (b0, b1, timer, value)


if __name__ == '__main__':
    # As an example, read & print data from the controller until interrupted.
    try:
        while True:
            print(read_power_controller_state())
    except KeyboardInterrupt:
        print("Keyboard interrupt.")
    except:
        print(sys.exc_info()(1))
