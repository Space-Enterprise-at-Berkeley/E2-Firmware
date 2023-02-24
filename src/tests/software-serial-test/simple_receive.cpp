#include <Arduino.h>

#define RX_PIN 14
#define RX_REG portInputRegister(digitalPinToPort(RX_PIN))
#define RX_BITMASK digitalPinToBitMask(RX_PIN)

void IRAM_ATTR rxBitISR() {
    const bool level = RX_REG & RX_BITMASK;
    const uint32_t curTick = cpu_hal_get_cycle_count();
    const bool empty = !self->m_isrBuffer->available();

    // Store level and tick in the buffer unless we have an overflow
    // tick's LSB is repurposed for the level bit
    if (!self->m_isrBuffer->push((curTick | 1U) ^ !level)) self->m_isrOverflow.store(true);
    // Trigger rx callback only when receiver is starved
    if (empty && self->m_rxHandler) self->m_rxHandler();
}

void setup() {
  // attachInterrupt(digitalPinToInterrupt(RX_PIN), rxBitISR, CHANGE);


  bool interrupt_initialized = false;

  esp_err_t err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3);
  interrupt_initialized = (err == ESP_OK) || (err == ESP_ERR_INVALID_STATE);

  // if new attach without detach remove old info
  if (__pinInterruptHandlers[pin].functional && __pinInterruptHandlers[pin].arg)
  {
    cleanupFunctional(__pinInterruptHandlers[pin].arg);
  }
  __pinInterruptHandlers[pin].fn = (voidFuncPtr)userFunc;
  __pinInterruptHandlers[pin].arg = arg;
  __pinInterruptHandlers[pin].functional = functional;

  gpio_set_intr_type((gpio_num_t)pin, (gpio_int_type_t)(intr_type & 0x7));
  if(intr_type & 0x8){
    gpio_wakeup_enable((gpio_num_t)pin, (gpio_int_type_t)(intr_type & 0x7));
  }
  gpio_isr_handler_add((gpio_num_t)pin, __onPinInterrupt, &__pinInterruptHandlers[pin]);


  //FIX interrupts on peripherals outputs (eg. LEDC,...)
  //Enable input in GPIO register
  gpio_hal_context_t gpiohal;
  gpiohal.dev = GPIO_LL_GET_HW(GPIO_PORT_0);
  gpio_hal_input_enable(&gpiohal, pin);

}

void loop() {

}
